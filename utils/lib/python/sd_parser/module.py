
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
__id__ = "$Id: module.py 1849 2010-08-23 08:42:57Z nipo $"
__version__ = "$Revision: 1849 $"

import os.path
import traceback
import warnings

from soclib_desc.error import *
from soclib_desc import module
from soclib_desc import parameter
from soclib_desc.abstraction_levels import checker

__all__ = ['Module', 'PortDecl',
           'MetaSignal', 'MetaPortDecl', 'SubSignal',
           'Signal', 'Port', 'Uses']

def dict_filtered_str(d):
    vals = []
    for k in sorted(d):
        v = d[k]
        if not isinstance(v, (str, unicode, int)):
            continue
        if ':' in k:
            continue
        vals.append('%s = %r'%(k, v))
    return ', '.join(vals)

class Module(module.ModuleCommon):

    # instance part
    module_attrs = {
        'classname' : '',
        'tmpl_parameters' : [],
        'header_files' : [],
        'global_header_files' : [],
        'implementation_files' : [],
        'implementation_type' : 'systemc',
        'object_files' : [],
        'interface_files' : [],
        'uses' : [],
        'accepts' : {},
        'defines' : {},
        'ports' : [],
        'sub_signals' : {},
        'signal' : None,
        'instance_parameters' : [],
        'local' : False,
        'extensions' : [],
        'constants' : {},
        "debug" : False,
        "debug_saved" : False,
        "deprecated":'',
        'can_metaconnect': False,
        }
    tb_delta = -3

    def __set_origin(self):
        filename, lineno = traceback.extract_stack()[self.tb_delta-1][:2]
        self.__filename = filename
        self.__lineno = lineno

    def __init__(self, typename, **attrs):
        """
        Creation of a module, with any overrides to defaults
        parameters defined in module_attrs class member.
        """
        self.__use_count = 0
        self.__typename = typename
        self.__set_origin()
        self.__used = False

        self.__spec_cache = {}

        # Call Module, Port or Signal interface ctor...
        module.ModuleCommon.__init__(
            self, self.__typename, self.__filename, self.__lineno)

        # Populate attributes
        self.__attrs = {}
        self.__attrs.update(self.module_attrs)
        for name, value in attrs.iteritems():
            if not name in self.module_attrs:
                warnings.warn(SpuriousDeclarationWarning(name, typename),
                              stacklevel = -self.tb_delta)
                continue
            self.__attrs[name] = value

        self.__attrs['abstraction_level'] = self.__typename.split(':', 1)[0]
#        self.__attrs['uses'] = set(self.__attrs['uses']) | set(map(lambda p:p.Use(), self.__attrs['ports']))

        # Absolution :)
        self.__mk_abs_paths(os.path.dirname(self.__filename))

        # Sanity checks for classname (entity name)
        self.__check_classname()
        self.__check_interface_files()

        if self.__attrs['debug']:
            self.set_debug_mode()

    def get_info(self, key):
        from copy import copy
        return copy(self.__attrs[key])

    def __check_classname(self):
        if self.__attrs['classname']:
            c = checker[self.__attrs["abstraction_level"]]
            if not c.validClassName(self.__attrs['classname']):
                raise InvalidComponent("Invalid class name '%s' level %s: '%s'"%(
                    self.__typename, c, self.__attrs['classname']))

    def __check_interface_files(self):
        for f in self.__attrs['interface_files']:
            b = os.path.basename(f)
            d = os.path.dirname(f)
            soclib = os.path.basename(d)
            if soclib != 'soclib':
                warnings.warn(BadInterfacePath(f, 'path should end with "soclib/%s"'%b))

    def __mk_abs_paths(self, basename):
        relative_path_files = ['header_files', 'implementation_files', 'object_files', 'interface_files']
        def mkabs(name):
            return os.path.isabs(name) \
                   and name \
                   or os.path.abspath(os.path.join(basename, name))
        for attr in relative_path_files:
            self.__attrs['abs_'+attr] = map(mkabs, self.__attrs[attr])
        self.__attrs['abs_header_files'] += self.__attrs['global_header_files']
        self.__attrs['abs_header_files'] += self.__attrs['abs_interface_files']

    def __str__(self):
        return '<Module %s in %s>'%(self.__typename, self.__filename)

    def __repr__(self):
        kv = []
        for k, v in self.__attrs.iteritems():
            if k not in self.module_attrs:
                continue
            if v == self.module_attrs[k]:
                continue
            if k in ['ports', 'uses']:
                v = sorted(v)
            if isinstance(v, list):
                kv.append("%s = [%s]"%(k, ',\n\t\t'.join(map(repr, v))))
            elif isinstance(v, dict):
                kvkv = ',\n\t\t'.join(map(lambda x:'"%s": %s'%(x,repr(v[x])), sorted(v.iterkeys())))
                kv.append("%s = {\n\t\t%s}"%(k, kvkv))
            elif isinstance(v, Module):
                pass
            else:
                kv.append("%s = %s"%(k, repr(v)))
        return '''%(type)s("%(type_name)s",
\t%(kv)s
)
'''%dict(type_name = self.__typename,
         type = self.__class__.__name__,
         kv = ',\n\t'.join(kv))

    # Internal specialization interface

    def _get_uses(self, **parent_args):
        us = set()
        for u in self.__attrs['uses']:
            us.add(u._specialize(**parent_args))
        return us

    # Common interface:

    def related_files(self):
        ks = ['header_files', 'implementation_files', 'object_files', 'interface_files']
        l = []
        for k in ks:
            l += self.__attrs['abs_'+k]
        return l

    # Module interface

    def is_used(self):
        return self.__used

    def get_template_parameters(self):
        return self.__attrs['tmpl_parameters']

    def specialize(self, **params):
        self.__used = True

        deprecated = self.__attrs['deprecated']
        if deprecated:
            warnings.warn(ModuleDeprecationWarning(self.__typename, deprecated),
                          stacklevel = 2)

        # Put specialized modules in a cache, this saves ~3sec on
        # a complete build
        h = 0
        for k in sorted(params.keys()):
            v = params[k]
            if isinstance(v, (list, tuple)):
                vh = 0
                for vv in v:
                    vh <<= 1
                    vh ^= hash(vv)
            else:
                vh = hash(v)
            h <<= 1
            h ^= hash(k) ^ vh
        h = hash(h)
        if h in self.__spec_cache:
            return self.__spec_cache[h]

        import specialization
        self.__spec_cache[h] = specialization.Specialization(
            self,
            local = self.__attrs['local'],
            **params)
        return self.__spec_cache[h]


class Signal(Module):
    pass

class PortDecl(Module):
    pass

class MetaPortDecl(PortDecl):
    pass

class MetaSignal(Signal):
    pass

class Port:
    def __init__(self, type, name, count = None, auto = None, **params):
        self.__type = type
        self.__name = name
        self.__count = count
        self.__auto = auto
        self.__params = params
        self.__where = traceback.extract_stack()[-2][0:2]

    @property
    def name(self):
        return self.__name

    @property
    def count(self):
        return self.__count

    @property
    def auto(self):
        return self.__auto

    def specialize(self, **params):
        params.update(self.__params)
        from soclib_desc.description_files import get_module
        mod = get_module(self.__type)
        spec = mod.specialize(**params)
#        print mod.name, spec.get_entity_name(), params
        return spec

    def __str__(self):
        return '<Port %s %s>'%(self.__type, self.__name)

    def __repr__(self):
        params = dict_filtered_str(self.__params)
        return 'Port("%s", "%s", %s)'%(self.__type,
                                       self.__name,
                                       params)

class Uses:
    """
    A statement declaring the module uses a specific component (in a
    global meaning, ie hardware component or utility).

    module_name is the module_name of the component, it the
    abstraction_level:module_name as in *.sd

    params is the list of arguments useful for compile-time definition
    (ie template parameters)
    """
    def __init__(self, module_name, **params):
        self.__module_name = module_name
        self.__params = params

        for k in self.__params.keys():
            if isinstance(self.__params[k], parameter.Foreign) or ':' in k:
                del self.__params[k]

        self.__hash = (
            hash(self.__module_name)^
            hash(str([(k, self.__params[k]) for k in sorted(self.__params)])))

        # This is for error feedback purposes
        self.__where = traceback.extract_stack()[-2][0:2]
        
    def __str__(self):
        return '<Use %s from %s>'%(self.__module_name, self.__where)

    def __repr__(self):
        params = dict_filtered_str(self.__params)
        return 'Uses(%r, %s)'%(self.__module_name, params)

    def __cmp__(self, other):
        return (
            cmp(self.__module_name, other.__module_name) or
            cmp(self.__params, other.__params)
            )

    def __hash__(self):
        return self.__hash

    def _specialize(self, **params):
        from soclib_desc import description_files
        
        params.update(self.__params)
        mod = description_files.get_module(self.__module_name)
        return mod.specialize(**params)

class SubSignal(Uses):
    pass
