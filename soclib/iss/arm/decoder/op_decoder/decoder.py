
from pprint import pprint
import re
import packer
import codegen
import sys

try:
    import psyco
    psyco.full()
    print "Psyco enabled"
except:
    print "Psyco failed to load"

class Func:
    def __init__(self, func, uid):
        self.__func = func
        self.__uid = uid
    def __str__(self):
        return "<Func %d: %s>"%(self.__uid, self.__func)
    def __repr__(self):
        return "Func('%s', %d)"%(self.__func, self.__uid)
    def __cmp__(self, other):
        return cmp(self.__uid, other.__uid)
    def name(self):
        return self.__func
    def __hash__(self):
        return self.__uid
    def uid(self):
        return self.__uid

class Op:
    def __init__(self, name, op, mask, func):
        self.__name = name
        self.__op = op
        self.__mask = mask
        self.__func = func
    def __str__(self):
        return "<Op %08x: %s (%s)>"%(self.__op, self.__name, self.__func)
    def __repr__(self):
        return "Op('%s')"%(self.__name)
    def func(self):
        return self.__func
    def mask(self):
        return self.__mask
    def __hash__(self):
        return hash(self.__func)
    def name(self):
        return self.__name

class OpTable:
    def __init__(self, packer, *ops):
        self.__packer = packer
        self.__ops = ops
        assert len(ops) == (1<<packer.size())
    def slices(self):
        return self.__packer.slices()
    def printall(self):
        for i in range(1<<self.__packer.size()):
            if i % 16 == 0:
                print
            print "%3d,"%self.__ops[i].func().uid(),
        print
    def func_equals(self, other):
        return cmp(self.uid_list(), other.uid_list())
    def equals(self, other):
        return cmp(self.__ops, other.__ops)
    def size(self):
        return 1<<self.__packer.size()
    def uid_list(self):
        return map(lambda x:x.func().uid(), self.__ops)

def get_useless_bits(slices, values):
    useful_bits = list(sorted(packer.slices_to_bitlist(slices)))
    useless_bits = []

    for n, bit in enumerate(useful_bits):
        idem = True
        for i in range(1<<len(useful_bits)):
            idem &= ( values[i | (1<<n)] == values[i & ~(1<<n)] )
        if idem:
            useless_bits.append((n,bit))
    return useless_bits

class OpRegistry:
    """
    Entry point for a complete decoder. This creates a decoder
    handling opcodes for a given subset of the instruction word.
    """
    def __init__(self, base_width, mask, format, *ops):
        """
        Constructor
        
        Parameters::
      
          base_width: base width of instruction word, in bits
      
          mask: mask to AND with instruction word to get the useful bits
      
          format: format string of subsequent instructions, with spaces and X
      
          *ops: instruction set definition
    
        Instruction set definition elements::
    
          a tuple: ("ins_format", "function")
    
            ins_format: a string corresponding to format above, with some 1
            and 0 imposed
           
            function: a function name. if there are some [key]-enclosed
            strings in function name string, they get expanded to if_false
            or if_true strings defined below
        
          a dict: {"key": (bit, "if_false", "if_true")}
    
            key: an arbitrary name
    
            bit: bit (0-based little endian) in instruction word
            (corresponding bit in ins_format will probably be X)
        """
        format_re = re.compile(r'\[([a-z A-Z_0-9-]*)\]')

        self.__base_width = base_width
        self.__packer = packer.Packer(((self.__base_width-1, 0),),
                                      mask)
        assert self.__packer.size() == len(filter(lambda x:x=='X', format))
        self.__base_format = format
        self.__format = format.replace("X", "%d")

        self.__ops = {}
        self.__funcs = {}
        self.__ops_by_mop = {}
        self.__func('ill')
        self.__ill = Op("ill", 0, 0, self.__func('ill'))
        
        cor = {}
        for op in ops:
            if isinstance(op, dict):
                cor.update(op)
                continue
            try:
                format, func, name = op
            except:
                format, func = op
                name = func
            x = '%(\\1)s'
            name = format_re.sub(x, name)
            func = format_re.sub(x, func)
            mask = self.__packer.parse_mask(format)
            plain = self.__packer.parse_bits(format)
            assert (mask & plain) == 0
            for mid in self.__packer.ids_for_op_mask(mask):
                opcod = mid|plain
                fdict = self.fdict(opcod, cor)
                mname = name%fdict
                mfunc = func%fdict
                op = self.__mkop(mname, opcod, mask, self.__func(mfunc))
#               print cor
#               print hex(plain), hex(mid), hex(mask), mname, mfunc, op, fdict
                
    def lookup(self, mop):
        """
        Gets an instruction name for a given instruction word
        """
        return self.__ops.get(mop, self.__ill)
                
    def __func(self, name):
        """
        Gets a function definition by its name. Functions all get an
        unique number.
        """
        if name not in self.__funcs:
            self.__funcs[name] = Func(name, len(self.__funcs))
        return self.__funcs[name]

    def __mkop(self, name, bits, mask, func):
        """
        Creates an entry for an opcode.

        name: name of the opcode
        bits: used bits for decoding the opcode
        mask: mask string
        func: corresponding function
        """
        op = Op(name, bits, mask, func)
        assert bits not in self.__ops, ValueError(
            'Collision:', self.__ops[bits], op)
        self.__ops[bits] = op
        return op

    def fdict(self, ident, cor):
        d = {}
        for (name, (bit, fval, tval)) in cor.iteritems():
            d[name] = [fval, tval][bool((1<<bit)&ident)]
        return d

    def id_table(self):
        t = []
        for n in range(1<<self.__width):
            op_list = self.__ops_by_id[n]
            assert len(set(map(lambda x:x.func_name(), op_list))) == 1
            t.append(op_list[0].uid())
        return t

    def funcs(self):
        return self.__funcs

    def functable(self):
        """
        Gets the list of function names sorted by their number
        """
        r = self.__funcs.values()
#       print r
        r.sort(lambda x, y:cmp(x.uid(), y.uid()))
#       print r
        return r

    def ops_for_func(self, func):
        """
        Gets all the opcodes calling a given function
        """
        return filter(lambda x:x.func() == func, self.__ops.itervalues())

    def _table(self, format, exact = True):
        left_mask = self.__packer.parse_mask(format)
        matched = self.__packer.parse_bits(format)

        assert (matched & left_mask) == 0
        ops = {}
        for opcod, op in self.__ops.iteritems():
            if (opcod & ~left_mask) != matched or opcod == 'ill':
                continue
            lid = opcod & left_mask# & op.mask()
            ops[lid] = op
            
        used_bits = 0
        for k, op in ops.iteritems():
            for l, op in ops.iteritems():
                used_bits |= (k^l)

        if exact:
            used_bits |= left_mask
        else:
            if used_bits != left_mask:
                print "Using less bits than it seems: %x -> %x"%(left_mask, used_bits)

        pack = packer.Packer([(self.__base_width-1, 0)], used_bits)
        rops = {}
        for k, op in ops.iteritems():
            rops[pack.mop_to_plain(k)] = op
        for i in range(1<<pack.size()):
            if i not in rops:
                rops[i] = self.__ill
        return OpTable(
            pack,
            *[rops[i] for i in range(1<<pack.size())])

    def table(self, format = None, exact = True):
        if format == None:
            format = self.__base_format
        format = ''.join(filter(lambda x:x in '01X', format))
        if exact:
            return self._table(format)
        #bla
        nx = len(filter(lambda x:x == 'X', format))
        x_poss = [i for i in range(len(format)) if format[i] == 'X']
        useless = set()
        mb = reversed(self.__packer.mask_bits(format))
        for bit, i in zip(mb, x_poss):
            f0 = format[:i] + '0' + format[i+1:]
            f1 = format[:i] + '1' + format[i+1:]
            t0 = self._table(f0, exact)
            t1 = self._table(f1, exact)
#           print format
#           print f0
#           print f1
#           t0.printall()
#           t1.printall()
            if t0.func_equals(t1) == 0:
                format = f0
#               format = format[:i] + 'X' + format[i+1:]
#               print "matches:", f0, f1, i
#           print
#       print '>', format, slices
        return self._table(format, exact)

    def reduced_table(self, format):
        if format == None:
            format = self.__base_format
        format = ''.join(filter(lambda x:x in '01X', format))

        left_mask = self.__packer.parse_mask(format)
        useful_mask = self.__packer.mask()
        nleft_mask = ~left_mask & useful_mask
        matched = self.__packer.parse_bits(format)

#       print "Reduced_table %x %x"%(left_mask, matched)

        assert (matched & left_mask) == 0
        ops = {}
        for opcod, op in self.__ops.iteritems():
#           print "%x %x %x %x"%(opcod, (opcod & nleft_mask), (opcod & left_mask), matched)
            if (opcod & nleft_mask) != matched:
                continue
            lid = opcod & left_mask
            ops[lid] = op

        pack = packer.Packer([(self.__base_width-1, 0)], matched)

        rops = {}
        for k, op in ops.iteritems():
            rops[pack.mop_to_plain(k)] = op

        ill = Op("ill", 0, 0, self.__func('ill'))
        table = [rops.get(i, ill) for i in range(1<<pack.size())]

        useless_bits = get_useless_bits(pack.slices(), table)
        if useless_bits:
            for n, bit in useless_bits:
                matched &= ~(1<<bit)
                pack = packer.Packer([(self.__base_width-1, 0)], matched)
                rops = {}
                for k, op in ops.iteritems():
                    rops[pack.mop_to_plain(k)] = op

                ill = Op("ill", 0, 0, self.__func('ill'))
                table = [rops.get(i, ill) for i in range(1<<pack.size())]

        return OpTable(pack, *table)

    def mkmasks(self, slices, base = None):
#       print "mkmasks(%r, %r)"%(slices, base)
        my_bl = packer.slices_to_bitlist(self.__packer.slices())
        de_bl = packer.slices_to_bitlist(slices)
        assert set(de_bl) <= set(my_bl), ValueError(de_bl, my_bl)
        poss = [""]
        mask = ""
        if base == None:
            base = "X"*len(my_bl)
        for n, b in enumerate(my_bl):
            if b in de_bl:
                poss = map(lambda x:x+"0", poss) \
                       + map(lambda x:x+"1", poss)
                mask = "%s"+mask
            else:
                mask = base[-1-n]+mask
        if len(poss) > 1:
            masks = []
            for x in poss:
                m = mask%tuple(list(x))
                masks.append(m)
#               print "  ", m
        else:
            masks = [mask]
        return sorted(masks)

    def pprint_mask(self, slices, mask):
        r = self.__packer.pprint_mask(slices, mask)
        return r

    def mops(self):
        return self.__packer.mops()

class Decoder:
    """
    Opcode decoder generator
    """
    prefix = "arm_"

    def __init__(self, registry, verbose, max_depth, *slices):
        """
        Creates an opcode decoder.

        Parameters::

          registry: an OpRegistry
          verbose: whether to generate C++ source code with tables
          containing comments
          max_depth: maximal depth of recursion of tables
          *slices: subparts of the instruction word to start decoding
           with (this is mostly a hint)
        """
        self.__registry = registry
        self.__funcs = self.__registry.functable()
        self.__subtables = []
        self.__tables = [0]
        self.__slices = {}
        self.__masks = {}
        self.__max_depth = max_depth
        self.__mask_to_table = {}
        self.__generator = codegen.CodeGenerator(verbose)

#       if slices:
        self.__entry = self.create_table_slices(slices)
#       else:
#           self.__entry = self.create_table(None)
#       print self.__tables[-self.__entry]
        self.__cleanup()

    def __cleanup(self):
        new_tables = []
        remap = {}
        
        done = set()
        todo = set([self.__entry])
        while todo:
            n = todo.pop()
            done.add(n)
            remap[n] = -len(new_tables)

#           print "Using table", n

            new_table = []
            
            table = self.__tables[-n]
            for i in table:
                if i < 0:
                    todo.add(i)
                new_table.append(i)
            new_tables.append(new_table)
            todo -= done
        for t in new_tables:
            for i in range(len(t)):
                if t[i] < 0:
                    t[i] = remap[t[i]]
        new_slices = {}
        new_masks = {}
#       print self.__slices, self.__masks
        print remap
        for before, after in remap.iteritems():
            new_masks[after] = self.__masks[before]
            new_slices[after] = self.__slices[before]
        self.__masks = new_masks
        self.__slices = new_slices
        self.__tables = new_tables
        self.__entry = 0

    def put_table(self, mask, slices, table):
        """
        Generate the table for a given mask, using the given slices

        mask: Mask selecting constant bits
        slices: bits to take for indexing table
        table: subtable
        """
        n = -len(self.__tables)
        self.__slices[n] = slices
        self.__masks[n] = self.__registry.pprint_mask(slices, mask)
        self.__tables.append(table)
        return n

    def subtree_size(self, n):
        if n >= 0:
            return 1
        size = 0
        done = set()
        todo = set([n])
        while todo:
            n = todo.pop()
            done.add(n)
            table = self.__tables[-n]
            size += len(table)
            for i in table:
                if i < 0:
                    todo.add(i)
            todo -= done
        return size

    def create_table_slices(self, slices, base = None, pf = 0):
        print (' '*pf+'Table slice').ljust(50), base, slices
        if base:
            t = self.__registry.reduced_table(base)
            matching_ops = set(t.uid_list())
            print t.uid_list(), t.slices(), matching_ops
            if len(matching_ops) == 1:
                return matching_ops.pop()
            
        masks = self.__registry.mkmasks(slices, base)
#       print pf, 'Creating masks', slices, base, masks
        t = []
        for m in masks:
            n = self.put_table(m, slices, t)
#           print (' '*pf+'Asking  table').ljust(50), m
            val = self.create_table(m, exact = False, pf = pf)
            t.append(val)
        if len(set(t)) == 1:
            return t[0]
        useless_bits = get_useless_bits(slices, t)
        if useless_bits:
            print "ohoh"
            nslices = packer.slices_diff(slices, packer.bitlist_to_slices(map(lambda x:x[1], useless_bits)))
            masks = self.__registry.mkmasks(nslices, base)
            t = []
            for m in masks:
                n = self.put_table(m, nslices, t)
                val = self.create_table(m, exact = False, pf = pf)
                t.append(val)
        return n

    def create_table(self, mask, exact = False, pf=0):
        sys.stdout.write((' '*pf+'Creating table ').ljust(50)+mask+'\r')
        sys.stdout.flush()
        complete = self.__registry.table(mask, True)
        hollow = self.__registry.table(mask, False)
#       print complete.uid_list(), hollow.uid_list()
#       if mask == "0001XXX01XX0":
#           raise
        while complete.size() != hollow.size():
            #bla
#           print "complete", mask, "hollow", mask
#           complete.printall()
#           print "hollow", mask
#           hollow.printall()
            d = packer.slices_diff(complete.slices(), hollow.slices())
            indirect_table_id = self.create_table_slices(d, mask, pf = pf+1)
            if indirect_table_id > 0:
                break
            indirect_table = self.__tables[-indirect_table_id]
            if indirect_table_id >= 0:
                return indirect_table_id
            print "***", indirect_table, hollow.uid_list(), complete.uid_list()
            if indirect_table == hollow.uid_list():
                print "replaced"
                return indirect_table_id
            if pf < self.__max_depth:
                return indirect_table_id
            break
#       table = self.__registry.reduced_table(mask)
        table = complete
        t = table.uid_list()
        slices = table.slices()
        if len(set(t)) == 1:
            n = t[0]
        else:
            if t not in self.__tables:
                n = self.put_table(mask, slices, t)
            else:
                n = -self.__tables.index(t)
#       print mask, n
        return n

    def tables(self):
        return self.__tables

    def tree(self, tn = 0, pf = ''):
        if tn == 0:
            tn = self.__entry
        table = self.__tables[-tn]
        assert isinstance(table, list), ValueError(table)
        sub = set()
#       print pf, "Table %4d:"%tn, self.__masks.get(tn, None)
        for n, i in enumerate(table):
            if i < 0:
                sub.add(i)
#           if n%16 == 0:
#               print
#               print pf,
#           print '%3d,'%i,
#       print
        depths = []
#       print sub
        for s in sub:
            depths.append(self.tree(s, pf+'  '))
        if depths:
            return max(depths)+1
        else:
            return 1

    def mkname(self, base, tn):
        """
        Creates an unique name with mask in it
        """
        assert tn <= 0
        if not base.endswith("_"):
            base += '_'
        return base + self.__masks[tn]

    def gen_tables(self, class_name, headers, namespaces):
        """
        Creates a table definition file with given C++ code constraints

        class_name: name of class in which to declare variables
        headers: headers to include
        namespaces: a list corresponding to path to namespace
        class_name is in
        """
        todo = set([(self.prefix+"table_"+"main", self.__entry)])
        done = set()
        funcs = {}
        ret = ''
        for h in headers:
            ret += '#include "%s"\n'%h
        ret += '\n'
        for n in namespaces:
            ret += 'namespace %s { '%n
        ret += '\n'
        ret += '\n'
        ret += '#undef _\n'
        ret += '#define _(...) &%s::__VA_ARGS__\n'%class_name
        ret += '#undef op\n'
        ret += '#define op(...) _('+self.prefix+'##__VA_ARGS__)\n'
        ret += '\n'
        while todo:
            name, tn = todo.pop()
            done.add(tn)
            if tn == self.__entry:
                func_name = self.prefix+"func_"+"main"
                table_name = self.prefix+"table_"+"main"
            else:
                func_name = self.mkname(self.prefix+"func_", tn)
                table_name = self.mkname(self.prefix+"table_", tn)
            funcs[-tn] = func_name

            w = 0
            slices_code = []
            for l, r in sorted(self.__slices[tn]):
                slices_code.append('((opcode >> %d) & 0x%x)'%(r-w, ((1<<(l-r+1))-1)<<w))
                w += (l-r+1)

            masks = self.__registry.mkmasks(self.__slices[tn],
                                            self.__masks[tn])
#           print table_name, self.__masks[tn], masks
            table = self.__tables[-tn]

            def f(mc):
                mask, c = mc
                if c < 0:
                    return c, mask+' '+self.mkname(self.prefix+"func_", c)
                else:
                    return c, mask+' '+self.__funcs[c].name()
            tt = map(f, zip(masks, table))
            v = dict(
                class_name = class_name,
                name = name,
                table_name = table_name,
                func_name = func_name,
                slices_code = '\n        | '.join(slices_code))
            ret += self.__generator.gen_table(
                class_name,
                'int8_t',
                class_name+'::'+table_name,
                tt, "B")
            for val in filter(lambda x:x<0, table):
                todo.add((self.mkname(self.prefix+"table_", val),val))
            code = ['size_t index = %(slices_code)s;'%v,
                    'int8_t op = %(name)s[index];'%v]
            if filter(lambda x:x<0, table):
                code += [
                    "if ( op < 0 ) {",
                    "    decod_func_t f = "+self.prefix+"func_"+"funcs[~(ssize_t)op];",
                    "    return (*f)(opcode);",
                    "}"]
            code += ['return op;']

            ret += self.__generator.gen_func(
                class_name,
                'int8_t'%v,
                '%(class_name)s::%(func_name)s'%v,
                ['data_t opcode'],
                '',
                'static',
                code)
        assert self.__entry == 0
        del funcs[-self.__entry]
        def nf(n):
            e = funcs.get(n+1, None)
            if not e:
                return "NULL", "No table"
            return '_(%s)'%(e), ''
        if funcs:
            ret += self.__generator.gen_table(
                class_name,
                class_name+'::'+'decod_func_t',
                    class_name+'::'+self.prefix+"func_"+'funcs',
                [nf(n) for n in range(max(funcs.keys()))],
                "P")
        ret += self.__generator.gen_table(
            class_name,
            class_name+'::'+'func_t',
            class_name+'::'+self.prefix+'funcs',
            map(lambda x:('op(%s)'%(x.name()), ', '.join(set(map(Op.name, self.__registry.ops_for_func(x))))),
                self.__funcs),
            "D")
        ret += self.__generator.gen_table(
            class_name,
            'const char *',
            class_name+'::'+self.prefix+'func_names',
            map(lambda x:('"%s"'%(', '.join(set(map(Op.name, self.__registry.ops_for_func(x))))), 'op(%s)'%(x.name())),
                self.__funcs),
            "D")
        ret += '\n\n'+'}'*len(namespaces)
        return ret

    def gen_decls(self):
        """
        Creates a table forward declarations corresponding to
        gen_tables entries
        """
        ret = self.__generator.decls()+'\n'
        for func in self.__funcs:
            n = func.name()
            if '<' not in n:
                ret += 'void %s%s();\n'%(self.prefix, n)
        return ret

    def gen_funcs(self, pfx):
        """
        Creates empty bodies for each function used in the opcode
        decoder.
        """
        l = []
        for func in self.__funcs:
            n = func.name()
            if '<' in n:
                n = n.split("<")[0]
                l.append((True, n))
            else:
                l.append((False, n))
        ret = ""
        for t, n in l:
            tell = ""
            if t:
                ret += 'template<...>\n'
                tell = "<...>"
            ret += '''void %(pfx)s%(name)s%(tell)s();
{

}

'''%dict(name = n, pfx = pfx, tell = tell)
        return ret

    def stats(self, sizeof_ptr = 4):
        """
        Prints statistics about in-memory size of tables and pointers
        """
        r = 0
        for t, size in (
            ('B', 1),
            ('P', sizeof_ptr),
            ('D', sizeof_ptr*2)):
            r += size * self.__generator.size().get(t, 0)
        return r

    def lookup(self, mop):
        d = 0
        tn = self.__entry
        while tn < 0 or d == 0:
            table = self.__tables[-tn]
            w = 0
            idx = 0
            for l, r in sorted(self.__slices[tn]):
                idx |= (mop >> (r-w)) & (((1<<(l-r+1))-1)<<w)
                w += (l-r+1)
            tn = table[idx]
            d += 1
        return self.__funcs[tn]

    def self_test(self):
        for opcod in self.__registry.mops():
            my_func = self.lookup(opcod)
            real_func = self.__registry.lookup(opcod).func()
            assert my_func == real_func, ValueError(opcod, my_func, real_func)
        print "Self-test OK"
