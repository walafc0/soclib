
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
# 
# Maintainers: group:toolmakers

import os, os.path
import subprocess
import operator
try:
    from functools import reduce
except:
    pass

from soclib_cc.config import config

from soclib_builder.action import *
from soclib_builder.bblock import *
from soclib_builder.textfile import *

from soclib_desc import component_builder

from soclib_cc.actions.cxx import *
from soclib_cc.actions.hdl import *

__id__ = "$Id: component_builder.py 2022 2011-01-29 00:54:08Z nipo $"
__version__ = "$Revision: 2022 $"

class UndefinedParam(Exception):
    def __init__(self, where, comp, param):
        Exception.__init__(self)
        self.where = ':'.join(where)
        self.comp = comp
        self.param = param
    def __str__(self):
        return '\n%s:error: parameter %s not defined for component %s'%(
            self.where, self.param, self.comp)

class ComponentInstanciationError(Exception):
    def __init__(self, where, comp, err):
        Exception.__init__(self)
        self.where = ':'.join(where)
        self.comp = comp
        self.err = err
    def __str__(self):
        return '\n%s:error: component %s: %s'%(
            self.where, self.comp, self.err)

class TooSimple(Exception):
    pass

class CxxComponentBuilder(component_builder.ComponentBuilderInterface):
    def __init__(self,
                 module_name,
                 classname,
                 implementation_files,
                 header_files,
                 tmpl_header_files = [],
                 interface_files = [],
                 defines = {},
                 local = False,
                 force_debug = False,
                 ):
        self.__module_name = module_name
        self.__classname = classname
        self.__implementation_files = list(implementation_files)
        self.__header_files = list(header_files)
        self.__tmpl_header_files = list(tmpl_header_files)
        self.__interface_files = list(interface_files)
        self.__defines = defines
        self.__force_debug = force_debug
        self.force_mode = self.__force_debug and "debug" or None
        self.local = local

    def getCxxBuilder(self, filename, *add_filenames):
        bn = self.baseName()
        if add_filenames:
            bn += '_all'
        else:
            bn += '_'+os.path.splitext(os.path.basename(filename))[0]
        source = self.cxxSource(filename, *add_filenames)
        if source:
            tx = CxxSource(
                config.reposFile(bn+".cpp", self.force_mode),
                source)
            src = tx.dests[0]
        else:
            src = filename
        headers = set(self.__header_files + self.__interface_files + self.__tmpl_header_files)
        incls = set(map(os.path.dirname, headers))

        # Take list of headers where basenames collides
        basename_to_inc = {}
        colliding_inc = set()
        for h in list(headers):
            name = os.path.basename(h)
            if name in basename_to_inc:
                colliding_inc.add(basename_to_inc[name])
                colliding_inc.add(h)
            else:
                basename_to_inc[name] = h
        colliding_inc = list(colliding_inc)
        
        if self.local:
            try:
                t = config.type
            except:
                t = 'unknown'
            out = os.path.join(
                os.path.dirname(filename),
                t+'_'+bn+"."+config.toolchain.obj_ext)
        else:
            out = config.reposFile(bn+"."+config.toolchain.obj_ext, self.force_mode)

        add = {}
        if config.get_library('systemc').vendor in ['sccom', 'modelsim']:
            add['comp_mode'] = 'sccom'
            out = os.path.abspath(os.path.join(
                config.get_library('systemc').sc_workpath,
                os.path.splitext(os.path.basename(str(src)))[0]
                +'.'+config.toolchain.obj_ext
                ))
        return CxxCompile(
            dest = out,
            src = src,
            defines = self.__defines,
            inc_paths = incls,
            includes  = colliding_inc,
            force_debug = self.__force_debug,
            **add)

    def __hash__(self):
        return hash(self.__module_name + self.__classname)

    def __cmp__(self):
        return (self.__module_name + self.__classname) == \
               (other.__module_name + other.__classname)

    def baseName(self):
        basename = self.__module_name
        tp = self.__classname
        basename += "_%08x"%hash(self)
        basename += "_" + tp.replace(' ', '_')
        params = ",".join(
            map(lambda x:'%s=%s'%x,
                self.__defines.iteritems()))
        if params:
            basename += "_" + params
        return basename.replace(' ', '_')

    def results(self):
        is_template = '<' in self.__classname
        impl = self.__implementation_files
        if is_template and len(impl) > 1:
            builders = [self.getCxxBuilder(*impl)]
        else:
            builders = map(self.getCxxBuilder, impl)
        return reduce(operator.add, map(lambda x:x.dests, builders), [])


    def cxxSource(self, *sources):
        if not '<' in self.__classname:
            if len(sources) > 1:
                source = ''
                for s in sources:
                    source += '#include "%s"\n'%s
                return source
            else:
                return ''

        inst = 'class '+self.__classname

        source = ""
        for h in set(map(os.path.basename, self.__tmpl_header_files)):
            source += '#include "%s"\n'%h
        for h in config.toolchain.always_include:
            source += '#include <%s>\n'%h
        for s in sources:
            source += '#include "%s"\n'%s
        source += "#ifndef ENABLE_SCPARSE\n"
        source += 'template '+inst+';\n'
        source += "#endif\n"
        return source

class HdlComponentBuilder(component_builder.ComponentBuilderInterface):
    def __init__(self,
                 module_name,
                 classname,
                 language,
                 header_files,
                 implementation_files,
                 used_implementation_files,
                 ):
        self.__module_name = module_name
        self.__classname = classname
        self.__language = language
        self.results = getattr(self, self.__language+'_results')
        self.__header_files = list(header_files)
        self.__implementation_files = list(implementation_files)
        self.__used_implementation_files = list(used_implementation_files)

    def verilog_results(self):
        return self.hdl_results(VerilogCompile)

    def vhdl_results(self):
        return self.hdl_results(VhdlCompile)

    def hdl_results(self, func):
        vendor = config.get_library('systemc').vendor
        if vendor not in ['sccom', 'modelsim']:
            raise NotImplementedError("Unsuported vendor %s"%vendor)

        deps = list(set(self.__used_implementation_files)
                    - set(self.__implementation_files))
        deps = bblockize(deps)

        incs = set()
        for i in self.__header_files:
            incs.add(os.path.dirname(i))
        builders = []
        for f in self.__implementation_files:
            b = func(dest = [], srcs = [f],
                     needed_compiled_sources = deps,
                     incs = incs,
                     typename = self.__classname)
            builders.append(b)
#            print b
            deps = b.sources
        return reduce(operator.add, map(lambda x:x.dests, builders), [])
