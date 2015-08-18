
class CodeGenerator:
    def __init__(self, verbose):
        self.__decls = []
        self.__verbose = verbose
        self.__size = {}
    def gen_table(self, ns, type, name, elems, sizeof):
        code = []
        if sizeof not in self.__size:
            self.__size[sizeof] = 0
        self.__size[sizeof] += len(elems)
        values = map(lambda x:x[0], elems)
        comms = map(lambda x:x[1], elems)
        if values and isinstance(values[0], (int, long)):
            values = map(lambda x:"%3d"%x, values)
        l = ''
        if self.__verbose:
            for n, (e, comm) in enumerate(zip(values, comms)):
                code.append('/* %3d */ %s, /* %s */'%(n, e, comm))
        else:
            for n, e in enumerate(values):
                if l and len(l)+2+len(e) > 80:
                    code.append(l)
                    l = ''
                l += e+', '
            if l:
                code.append(l)
        v = dict(
            type_name = type,
            name = name,
            size = len(values),
            code = '\n    '.join(code))
        decl = '%(type_name)s const %(name)s[%(size)d]'%v
        self.__decls.append('static '+decl.replace(ns+'::', ''))
        return decl+''' = {
    %(code)s
};
'''%v
    def gen_func(self, ns, ret, name, args, qual, dqual, code):
        v = dict(
            ret = ret,
            name = name,
            args = ', '.join(args),
            qual = qual,
            dqual = dqual,
            code = '\n    '.join(code))
        decl = '%(ret)s %(name)s(%(args)s)'%v
        if qual:
            decl += ' '+qual
        self.__decls.append(
            (((dqual+' ') if dqual else '') +
            decl).replace(ns+'::', ''))
        return decl + '''
{
    %(code)s
}
'''%v
    def decls(self):
        return '\n'.join(sorted(map(lambda x:x+';', self.__decls)))

    def size(self):
        return self.__size

def putfile(name, code):
    from datetime import datetime
    year = datetime.now().year
    fd = open(name, 'w')
    fd.write(('''\
/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, %(year)d
 *         Nicolas Pouillon <nipo@ssji.net>, %(year)d
 *
 * Maintainers: nipo becoulet
 *
 * $'''+'''Id$
 *
 */

%(code)s

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
''')%dict(code = code,
         year = year))
    fd.close()
