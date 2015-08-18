
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
#         Nicolas Pouillon <nipo@ssji.net>, 2007
# 
# Maintainers: group:toolmakers

import os, os.path
import traceback
from copy import copy

from soclib_builder.todo import ToDo

from soclib_cc.config import config
from soclib_cc.actions.cxx import CxxCompile, CxxLink, CCompile

__id__ = "$Id: platform.py 2367 2013-06-19 11:16:12Z porquet $"
__version__ = "$Revision: 2367 $"

__all__ = ['Platform']

class Platform(ToDo):
    """
    Platform definition, should be passed an arbitrary number of
    Uses() and Source() statements that constitutes the platform.
    """

    def __init__(self, mode, source_file, uses = [], defines = {}, output = None, **params):
        self.__mode = mode
        self.__source = source_file
        self.__uses = uses
        self.__defines = defines
        self.__output = output or config.output

        src = Source(mode, source_file, uses, defines, **params)
        self.__spec = spec = src.specialize(**params)

        objs = set()
        for c in spec.get_used_modules():
            for o in c.builder().results():
                if isinstance(o.generator, CCompile):
                    objs.add( o )

        linked = CxxLink(self.__output, objs)
        linked.dests[0].delete()
        ToDo.__init__(self, *linked.dests )

    def __repr__(self):
        import pprint
        return '%s(%r, %r,\n%s, %r, %r)'%(
            self.__class__.__name__,
            self.__mode,
            self.__source,
            pprint.pformat(list(self.__uses)),
            self.__defines,
            self.__output,
            )

    def embedded_code_cflags(self):
        paths = set()
        for spec in self.__spec.get_used_modules():
            for d in map(os.path.dirname, spec.get_interface_files()):
                if os.path.basename(d) == 'soclib':
                    paths.add(os.path.dirname(d))
        return ' '.join(map(lambda x: '-I'+x, paths))

    def get_used_modules(self):
        return self.__spec.get_used_modules()

def Source(mode, source_file, uses = [], defines = {}, **params):
    from sd_parser import module

    name = mode+':platform_desc_'+hex(hash(source_file))[-4:]

    filename = traceback.extract_stack()[-3][0]
    d = os.path.abspath(os.path.dirname(filename))
    if not os.path.isabs(source_file):
        source_file = os.path.join(d, source_file)

    return module.Module(name,
               uses = uses,
               defines = defines,
               implementation_files = [source_file],
               local = True,
               )

def parse(filename):
    from sd_parser import module

    glbls = {}
    glbls['config'] = config
    glbls['Platform'] = Platform
    glbls['Uses'] = module.Uses

    locs = {}
    exec file(filename) in glbls, locs
    try:
        todo = locs['todo']
    except:
        raise ValueError("Can't find variable `todo' in `%s'."%platform)
    return todo
