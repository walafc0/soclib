
# SOCLIB_GPL_HEADER_BEGIN
# 
# This file is part of SoCLib, GNU GPLv2.
# 
# SoCLib is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# SoCLib is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SoCLib; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
# 
# SOCLIB_GPL_HEADER_END
# 
# Copyright (c) UPMC, Lip6, SoC
#         Nicolas Pouillon <nipo@ssji.net>, 2009

import warnings
import operator

__id__ = "$Id: parameter.py 2053 2011-02-16 09:16:41Z gut $"
__version__ = "$Revision: 2053 $"

class ParameterError(Exception): pass

class Base:
    @staticmethod
    def get_tmpl_value(value, **args):
        return value

    @staticmethod
    def get_inst_value(value, **args):
        return value

    @staticmethod
    def get_internal_value(value, **args):
        return value

    def __mul__(self, other):
        return BinaryOp(operator.mul, self, other, '%(left)s * %(right)s')
    def __add__(self, other):
        return BinaryOp(operator.add, self, other, '%(left)s + %(right)s')
    def __pow__(self, other):
        return BinaryOp(operator.pow, self, other, '::pow(%(left)s, %(right)s)')
    def __div__(self, other):
        return BinaryOp(operator.div, self, other, '%(left)s / %(right)s')
    def __sub__(self, other):
        return BinaryOp(operator.sub, self, other, '%(left)s - %(right)s')
    def __mod__(self, other):
        return BinaryOp(operator.mod, self, other, '%(left)s % %(right)s')
    def __rmul__(self, other):
        return BinaryOp(operator.mul, other, self, '%(right)s * %(left)s')
    def __radd__(self, other):
        return BinaryOp(operator.add, other, self, '%(right)s + %(left)s')
    def __rpow__(self, other):
        return BinaryOp(operator.pow, other, self, '::pow(%(right)s, %(left)s)')
    def __rdiv__(self, other):
        return BinaryOp(operator.div, other, self, '%(right)s / %(left)s')
    def __rsub__(self, other):
        return BinaryOp(operator.sub, other, self, '%(right)s - %(left)s')
    def __rmod__(self, other):
        return BinaryOp(operator.mod, other, self, '%(right)s % %(left)s')

class Foreign(Base):
    def __init__(self, name, original):
        self.name = name
        self.original = original
        self.value = self

    def resolve(self, args):
        return self
    
    def assertValid(self, value):
        return True

    def __str__(self):
        return self.name

    def __repr__(self):
        return 'parameter.%s(%r)'%(self.__class__.__name__,
                                   self.name)

    @staticmethod
    def get_internal_value(value, **args):
        return value.name

class Implicit(Foreign):
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def resolve(self, args):
        return self.value

    def __str__(self):
        return self.value

    def __repr__(self):
        return 'parameter.%s(%r)'%(self.__class__.__name__,
                                   self.name)

class Parameter(Base):
    valid_types = ()
    def __init__(self, name, default = None, auto = None):
        self.name = name
        self.default = default
        self.auto = auto

    def resolve(self, args):
#        print "resolve %s %s %s %s"%(self, args, self.default, self.auto)
        value = None
        if self.name in args:
            value = args[self.name]
        if value is None and self.auto and self.auto in args:
            value = args[self.auto]
        if value is None:
            value = self.default
        if value is None:
            raise ValueError("Please give a value for parameter `%s'"%self.name)
        self.assertValid(value)
        return value
        
    def assertValid(self, value):
        if isinstance(value, Foreign):
            return
        try:
            value = self.valid_types[0](value)
        except:
            pass
#        print type(value), self.valid_types, self
        ok = isinstance(value, self.valid_types)
        if not ok:
            raise ParameterError("Invalid type `%s' for parameter `%s'"%(value, self.name))

    def __str__(self):
        return '<Parameter %s: %s>'%(self.__class__.__name__, self.name)

    def __repr__(self):
        return 'parameter.%s(%r, %r, %r)'%(self.__class__.__name__,
                                 self.name,
                                 self.default,
                                 self.auto)


class Bool(Parameter):
    valid_types = (bool,)
    def __init__(self, name, default = None, auto = None):
        Parameter.__init__(self, name, bool(default), auto)

    @staticmethod
    def get_tmpl_value(value, **args):
        return str(value).lower()

    @staticmethod
    def get_inst_value(value, **args):
        return str(value).lower()

    def get_inst_decl(self):
        return 'bool '+self.name

    def get_tmpl_decl(self):
        return 'bool '+self.name

class Int(Parameter):
    valid_types = (int, long)
    def __init__(self, name, default = None, min = None, max = None, auto = None):
        Parameter.__init__(self, name, default, auto)
        self.min = min
        self.max = max


    def __repr__(self):
        return 'parameter.%s(%r, %r, min = %r, max = %r, auto = %r)'%(self.__class__.__name__,
                                 self.name,
                                 self.default,
                                                            self.min, self.max,
                                 self.auto)

    @staticmethod
    def getTmplType(value):
        return value

    def assertValid(self, value):
        Parameter.assertValid(self, value)
        if self.max is not None and value >= self.max:
            raise ParameterError("Invalid value `%s' for parameter `%s': above %d"%(value, self.name, self.max))
        if self.min is not None and value < self.min:
            raise ParameterError("Invalid value `%s' for parameter `%s': below %d"%(value, self.name, self.min))

    @staticmethod
    def get_inst_value(v, **args):
        if v < 4096:
            return '%d'%v
        else:
            return '0x%08x'%v

    def get_inst_decl(self):
        return 'int '+self.name

    def get_tmpl_decl(self):
        return 'int '+self.name

class Float(Parameter):
    valid_types = (float)
    def __init__(self, name, default = None, min = None, max = None, auto = None):
        Parameter.__init__(self, name, default, auto)
        self.min = min
        self.max = max


    def __repr__(self):
        return 'parameter.%s(%r, %r, min = %r, max = %r, auto = %r)'%(self.__class__.__name__,
                                 self.name,
                                 self.default,
                                                            self.min, self.max,
                                 self.auto)

    @staticmethod
    def getTmplType(value):
        return value

    def assertValid(self, value):
        Parameter.assertValid(self, value)
        if self.max is not None and value >= self.max:
            raise ParameterError("Invalid value `%s' for parameter `%s': above %d"%(value, self.name, self.max))
        if self.min is not None and value < self.min:
            raise ParameterError("Invalid value `%s' for parameter `%s': below %d"%(value, self.name, self.min))

    @staticmethod
    def get_inst_value(v, **args):
        if v < 4096:
            return '%d'%v
        else:
            return '0x%08x'%v

    def get_inst_decl(self):
        return 'float '+self.name

    def get_tmpl_decl(self):
        return 'float '+self.name

class String(Parameter):
    valid_types = (str,)
    @staticmethod
    def get_inst_value(value, **args):
        return '"%s"'%value

    def get_inst_decl(self):
        return 'std::string '+self.name

class StringArray(Parameter):
    valid_types = (list,)
    @staticmethod
    def get_inst_value(value, **args):
        elems = ', '.join(map(lambda x:'"%s"'%x.replace('"', '\\"'), value)
                          +["NULL"])
        return 'stringArray(%s)'%(elems)

    def get_inst_decl(self):
        return 'std::vector<std::string> '+self.name

class IntArray(Parameter):
    valid_types = (list,)
    @staticmethod
    def get_inst_value(value, **args):
        v = value
        elems = ', '.join(map(str, v))
        return 'intArray(%d, %s)'%(len(v), elems)

    def get_inst_decl(self):
        return 'std::vector<int> '+self.name

class IntTab(Parameter):
    valid_types = (tuple, list)

    def get_inst_decl(self):
        return 'soclib::common::IntTab '+self.name

    @staticmethod
    def get_inst_value(value, **args):
        v = value
        return 'soclib::common::IntTab(%s)'%(', '.join(map(str, v)))

class Type(Parameter):
    valid_types = (str, unicode)

#    @staticmethod
    def get_tmpl_value(self, v, **args):
        try:
            has_colon = ':' in v and not '::' in v
        except:
            has_colon = False
        if has_colon:
#            print self.name, self.default, v,
            m = Module(self.name, self.default, self.auto)
            try:
                v = m.get_tmpl_value(v, **resolve(args, args))
#                print v
            except Exception, e:
#                print 'failed', e
                warnings.warn("Cant guarantee parameter '%s' with value '%s' is valid"%
                              (self.name, v))
        return v

class Module(Type):
    valid_types = (str, unicode)

    def get_tmpl_decl(self):
        return 'typename '+self.name

    def get_inst_decl(self):
        import description_files
        mod = description_files.get_module(self.typename)
        if mod['tmpl_parameters']:
            raise NotImplementedError("Module %s has template parameters"%self.typename)
        type = mod['classname']
        return type+' '+self.name

    def __init__(self, name, typename = None, default = None, auto = None, **kwargs):
        Parameter.__init__(self, name, default, auto)
        self.typename = typename
        self.kwargs = kwargs

    def __repr__(self):
        r = 'parameter.%s(%r'%(
            self.__class__.__name__,
            self.name,
            )
        if self.typename:
            r += ', typename = %r'%self.typename
        if self.default:
            r += ', default = %r'%self.default
        if self.auto:
            r += ', auto = %r'%self.auto
        for k, v in sorted(self.kwargs.items()):
            r += ', %s = %r'%(k, v)
        return r + ')'

    @staticmethod
    def get_inst_value(value, **args):
        return value.ref()

    def get_tmpl_value(self, value, **args):
        import description_files
        mod = description_files.get_module(value)
        args.update(self.kwargs)
#        print value, mod, args
        return mod.specialize(**args)

class Reference(Base):
    def __init__(self, name, mode = 'val'):
        self.__name = name
        self.__mode = mode

    @property
    def name(self):
        return self.__name

    def __repr__(self):
        return 'parameter.%s(%r, %r)'%(
            self.__class__.__name__,
            self.__name,
            self.__mode)
        
#   def setValue(self, args, value):
#       args[self.__name] = value

    def resolve(self, args):
        if self.__mode == 'val':
#            print list(sorted(args.keys()))
#            print '***', self.__name, self.__name in args
            r = args[self.__name]
        elif self.__mode == 'len':
            r = len(args[self.__name])
        else:
            raise ValueError("Unsupported mode %s for parameter.Reference"%self.__mode)
        return r

class Constant:
    def __init__(self, name):
        self.__name = name
    def name(self):
        return self.__name

class BinaryOp(Base):
    def __init__(self, op, left, right, fmt):
        self.__op = op
        self.__left = left
        self.__right = right
        self.__fmt = fmt

    def __repr__(self):
        return 'parameter.%s(%r, %r)'%(
            self.__op,
            self.__left,
            self.__right)

    def resolve(self, args):
        left = value(self.__left, args, 'internal')
        right = value(self.__right, args, 'internal')
#        print repr(self.__op), repr(self.__left), repr(self.__right)
        if isinstance(self.__left, Foreign) or isinstance(self.__right, Foreign):
            return self.__fmt%dict(left = left, right = right)
        return self.__op(left, right)

class StringExt(BinaryOp):
    def __init__(self, st, *a):
        BinaryOp.__init__(self, operator.mod, st, a, '')

def resolve(v, args):
    if isinstance(v, Foreign):
        return v
    elif isinstance(v, (list, tuple)):
        v = type(v)(map(lambda x:resolve(x, args), v))
#       print "pwet", v
    elif isinstance(v, dict):
        res = {}
        for kk, vv in v.items():
            res[kk] = resolve(vv, v)
        return res
    elif isinstance(v, Base):
        return v.resolve(args)
    return v

def value(thing, args, value_type):
    if isinstance(thing, Base):
        try:
            va = args[thing.name]
        except Exception:
#            print args, thing.name
            va = None
        if isinstance(va, Foreign):
            return va
        v = resolve(thing, args)
        f = getattr(thing, 'get_%s_value'%value_type)
        r = f(v, **args)
#        print "***", thing, f, v, r
        return resolve(r, dict(**args))
    if isinstance(thing, (list, tuple)):
        return type(thing)(map(lambda x:value(x, args, value_type), thing))
    return thing

