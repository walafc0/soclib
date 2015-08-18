
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

__author__ = 'Nicolas Pouillon, <nipo@ssji.net>'
__copyright__ = 'UPMC, Lip6, SoC, 2007-2010'
__license__ = 'GPL-v2'
__id__ = "$Id: specialization.py 2367 2013-06-19 11:16:12Z porquet $"
__version__ = "$Revision: 2367 $"

import operator
try:
    from functools import reduce
except:
    pass

from soclib_desc import module
from soclib_desc import description_files
from soclib_desc import parameter
from soclib_desc import error
from soclib_desc import specialization as sds

class Specialization:

    def __init__(self, module, local = False, **params):
        self.__module = module
        self.__local = local
#        self.__used_parameters = self.__find_useful_parameters(**params)

        try:
            self.__passed_params = parameter.resolve(params, params)
            self.__entity_name = self.__get_cxx_type()
            self.__hash = hash(self.__entity_name) ^ hash(self.__module)

            self.__tmpl_dependencies = set()
            for i in self.__module.get_info('tmpl_parameters'):
                if isinstance(i, parameter.Module):
                    val = parameter.value(i, self.__passed_params, 'tmpl')
                    if not isinstance(val, parameter.Foreign):
                        self.__tmpl_dependencies |= val.__get_subtree()

            self.__uses = self.__module._get_uses(**self.__passed_params)
            self.__dependencies = self.__uses | self.__tmpl_dependencies
            self.__used_modules = self.__get_used_modules()
        except Exception, e:
            raise error.ModuleSpecializationError(self.__module.name, e)

    def __get_subtree(self, pfx = ''):
        r = set()
        for m in set(self.__dependencies):
            r |= m.__get_subtree(pfx + '  ')
        if self.__module.get_info('implementation_type') == 'mpy_vhdl':
            for s in r:
                s.externally_handled = (s.__module['implementation_type'] == 'mpy_vhdl')
        r.add(self)
        return r

    CXX_LANGUAGES = ['systemc']
    HDL_HANGUAGES = ['vhdl', 'verilog']

    def builder(self):
        ty = self.__module.get_info('implementation_type')
        if ty in self.CXX_LANGUAGES:
            return self.__cxx_builder()
        elif ty in self.HDL_HANGUAGES:
            return self.__hdl_builder()
        else:
            raise NotImplementedError(
                'Handling of language '
                +ty)

    def __hdl_builder(self):
        import component_builder

        deps = self.get_used_modules()
        deps = filter(lambda x:x.implementation_language() in self.HDL_HANGUAGES,
                     deps)
        iuf = reduce(operator.add,
                     map(lambda x:x.__module.get_info('abs_implementation_files'), deps),
                     [])
        uhf = reduce(operator.add,
                     map(lambda x:x.__module.get_info('abs_header_files'), deps),
                     [])
        

        return component_builder.HdlComponentBuilder(
            self.__module.name,
            classname = self.__entity_name,
            language = self.__module.get_info('implementation_type'),
            header_files = uhf,
            implementation_files = self.__module.get_info('abs_implementation_files'),
            used_implementation_files = iuf,
            )

    def __cxx_builder(self):
        import component_builder

        tmpl_headers = set()
        for s in self.get_used_modules():
            tmpl_headers |= set(s.__module.get_info('abs_header_files'))

        return component_builder.CxxComponentBuilder(
            self.__module.name,
            self.__entity_name,
            local = self.__local,
            implementation_files = self.__module.get_info('abs_implementation_files'),
            header_files = self.__module.get_info('abs_header_files'),
            tmpl_header_files = tmpl_headers,
            interface_files = self.__module.get_info('abs_interface_files'),
            defines = self.__module.get_info('defines'),
            force_debug = self.__module.debug_mode)

    def port_list(self, **inst_params):
        '''
        Retrieves the port dict. Some ports may depend from instance
        parameters. If all parameters are passed in, all ports are
        fully specified.
        '''
        args = {}
        args.update(self.__passed_params)
        args.update(inst_params)
        
        pl = {}
        for port in self.__module.get_info('ports'):
            spec = port.specialize(**args)
            count = port.count
            if isinstance(count, parameter.Constant):
                count = self.__get_constant(count.name())
            elif isinstance(count, parameter.Reference):
                try:
                    count = parameter.resolve(port.count, args)
                except KeyError:
                    pass
            p = sds.Port(spec, count, port.auto)
            pl[port.name] = p
        return pl

    def implementation_language(self):
        '''
        Retrieves the native language of the module. It should probably
        be vhdl, systemc, verilog, ...
        '''
        return self.__module.get_info('implementation_type')

    def __get_constant(self, name):
        try:
            return self.__module.get_info('constants')[name]
        except:
            for t in self.__tmpl_dependencies:
                try:
                    return t.__get_constant(name)
                except KeyError:
                    pass
        raise KeyError(name)

    def get_used_modules(self):
        return self.__used_modules

    def __get_used_modules(self):
        use = self.__uses
        ports = self.port_list()
        module_templates = self.__tmpl_dependencies

        todo = set(use)
        todo |= set([p.specialization for p in ports.values()])
        todo |= module_templates
        u = set()
        while todo:
            m = todo.pop()
            u.add(m)
            u |= m.get_used_modules()
            todo -= u
        u.add(self)
        return u

    def __hash__(self):
        '''
        Returns an uid for the specialization
        '''
        return self.__hash

    def __eq__(self, other):
        '''
        Tests whether the two specializations are actually equal
        '''
        return self.__entity_name == other.__entity_name and \
               self.__module == other.__module

    def get_extensions(self, namespace):
        '''
        Gets extensions defined for a given namespace. Extensions are
        each in a string.
        '''
        strip = len(namespace)+1
        exts = []
        for e in self.__module.get_info('extensions'):
            if not e.startswith(namespace+':'):
                continue
            exts.append(e[strip:])
        return exts

    def get_entity_name(self):
        '''
        Retrieves a string corresponding to the whole entity
        name.

        * For SystemC modules, this is a class name with qualified
          namespace and template parameters.
        * For rtl modules, this is a simple entity name
        '''
        return self.__entity_name

    def get_instance_parameters(self):
        '''
        List of parameters to be passed at run time, these are the
        necessary parameters for getting a valid module instanciation.
        Depending on the module implementation language, this may be a
        string, a list of parameters, or a dictionary.
        '''
        return self.__module.get_info('instance_parameters')

    def get_header_files(self):
        return self.__module.get_info('abs_header_files')

    def get_interface_files(self):
        return self.__module.get_info('abs_interface_files')

    def get_implementation_files(self):
        return self.__module.get_info('abs_implementation_files')

    def __get_cxx_type(self):
        tmpl_parameters = map(
            lambda x:parameter.value(x, self.__passed_params, 'tmpl'),
            self.__module.get_info('tmpl_parameters'))

        cn = self.__module.get_info('classname')
        if not cn:
            return ''
        tp = ','.join(map(lambda x:str(x), tmpl_parameters))
        if tp:
            tp = '<'+tp+' > '
        return cn+tp

    def __str__(self):
        return self.__entity_name

    def __repr__(self):
        return '%s(%r, %s)' % (
            self.__class__.__name__,
            self.__module,
            ', '.join(map(
            lambda x:'%s = %s'%x, self.__passed_params.items()))
            )


    @staticmethod
    def __find_useful_parameters(**passed_params):
        used_parameters = {}

#        print '__init_parameters', parameters

        parameters_to_resolve = list(passed_params.iteritems())
        last_parameters_to_resolve = []
        while parameters_to_resolve:
            if parameters_to_resolve == last_parameters_to_resolve:
                raise RuntimeError("Unresolved parameters: %r"%parameters_to_resolve)
            unresolved = []
            for k, v in parameters_to_resolve:
                try:
                    a = {}
                    a.update(passed_params)
                    a.update(used_parameters)
                    v = parameter.resolve(v, a)
                    if isinstance(v, parameter.BinaryOp):
                        v = v.resolve(a)
                    elif isinstance(v, parameter.Base):
                        v = v.value
                    used_parameters[k] = v
                except KeyError, e:
#                    print 'KeyError', e, k, v, used_parameters.keys(), passed_params.keys()
                    if e == k:
                        unresolved.append((k, v))
                    else:
                        raise
                except Exception, e:
                    raise
                    unresolved.append((k, v))
            last_parameters_to_resolve = parameters_to_resolve
            parameters_to_resolve = unresolved
        return used_parameters

    # Signal interface

    def port_max_count(self, port):
        import module
        assert isinstance(self.__module, module.Signal)
        acc = self.__module.get_info('accepts')
        return acc.get(port.__module.name, 0)

    def can_meta_connect(self):
        return self.__module.get_info('can_metaconnect')

    # Port interface

    def can_connect(self, signal):
        import module
        assert isinstance(self.__module, module.PortDecl)
        return False

    def get_signal(self):
        import module
        assert isinstance(self.__module, module.PortDecl)
        mname = self.__module.get_info('signal')
        mod = description_files.get_module(mname)
        return mod.specialize(**self.__passed_params)
