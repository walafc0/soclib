
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
__id__ = "$Id: cxx.py 1750 2010-05-26 09:49:35Z nipo $"
__version__ = "$Revision: 1750 $"

import os, os.path
import sys
try:
    from functools import reduce
except:
    pass

from soclib_cc.config import config

from soclib_builder import action
from soclib_builder import command
from soclib_builder import fileops
from soclib_builder import mfparser
from soclib_builder import bblock
from soclib_builder import depends

class CCompile(action.Action):
    info_code = 'C'
    priority = 100
    tool = 'CC'
    todo_code = 'C'

    def __init__(self, dest, src, defines = {}, inc_paths = [], includes = [], force_debug = False, comp_mode = 'normal'):
        action.Action.__init__(self, [dest], [src], defines = defines, inc_paths = inc_paths, includes = includes)
        self.mode = force_debug and "debug" or None
        self.comp_mode = comp_mode
        if force_debug:
            defines["SOCLIB_MODULE_DEBUG"] = '1'

    def __arg_sort(self, args):
        libdirs = filter(lambda x:str(x).startswith('-L'), args)
        libs = filter(lambda x:str(x).startswith('-l'), args)
        args2 = filter(lambda x:x not in libs and x not in libdirs, args)
        return args2 + libdirs + libs
        
    def __command_line(self, mode = ''):
        args = config.getTool(self.tool, mode)
        args += map(lambda x:'-D%s=%s'%x, self.options['defines'].iteritems())
        args += map(lambda x:'-I%s'%x, self.options['inc_paths'])
        for i in self.options["includes"]:
            args.append('-include')
            args.append(i)
        args += config.getCflags(self.mode)
        return args

    def __process_deps(self, filename):
        if config.debug:
            print 'Computing deps for', self
            print 'Creating', repr(filename)
        filename.generator.process()
        dep_name = hex(hash(filename))+'.deps'
        try:
            return depends.load(dep_name)
        except depends.MustRehash:
            pass
        cmd = self.__command_line() + [
            '-MM', '-MT', 'foo.o', str(filename)]
        cmd = self.__arg_sort(cmd)

        process = command.Command(cmd)
        ret = process.run(True)

        try:
            deps = mfparser.MfRule(process.stdout)
        except ValueError:
            sys.stderr.write(process.stderr)
            sys.stderr.write('\n')
            raise action.ActionFailed("Unable to compute dependencies", cmd)
        
        deps = bblock.bblockize(deps.prerequisites)
        depends.dump(dep_name, deps)
        return deps

    def cc_get_command(self):
        if self.comp_mode == 'sccom':
            args = self.__command_line('SCCOM')
            args += ['-work', config.workpath]
        else:
            args = self.__command_line()
            args += ['-c', '-o', bblock.filenames(self.dests)[0]]
            
        return args + map(str, self.sources)

    def prepare(self):
        cmd = self.cc_get_command()
        if cmd:
            self.run_command(self.__arg_sort(cmd))

        self.prepare_deps()

        action.Action.prepare(self)

    def prepare_deps(self):
        for s in self.sources:
            deps = self.__process_deps(s)
            self.add_depends(*deps)

class CxxCompile(CCompile):
    tool = 'CXX'

class CLink(CCompile):
    priority = 200
    tool = 'CC_LINKER'

    def __init__(self, dest, objs):
        action.Action.__init__(self, [dest], objs)
    
    def cc_get_command(self):
        args = config.getTool(self.tool)
        if config.get_library('systemc').vendor in ['sccom', 'modelsim']:
            args += ['-link']
            args += ['-lpthread']
            args += ['-work', config.workpath]
            args += config.getLibs()
            objs = filter(lambda x:x.generator.comp_mode not in ['sccom', 'modelsim'], self.sources)
            args += bblock.filenames(objs)
        else:
            args += ['-o', bblock.filenames(self.dests)[0]]
            args += config.getLibs()
            objs = self.sources
            args += bblock.filenames(objs)

        return args

    def prepare_deps(self):
        pass

class CxxLink(CLink):
    tool = 'CXX_LINKER'
    info_code = 'L'

class CMkobj(CLink):
    priority = 200
    tool = 'LD'
    
    def cc_get_command(self):
        if config.get_library('systemc').vendor in ['sccom', 'modelsim']:
            self.tool = "CXX_LINKER"
            return CLink.cc_get_command(self)
        else:
            args = config.getTool(self.tool)
            args += ['-r', '-o', bblock.filenames(self.dests)[0]]
            args += bblock.filenames(self.sources)
        return args

class CxxMkobj(CMkobj):
    pass

if __name__ == '__main__':
    import sys
    cc = CxxCompile(sys.argv[1], sys.argv[2])
    #print cc.processDeps()
    can = cc.canBeProcessed()
    must = cc.mustBeProcessed()
    print "Can be processed:", can
    print "Must be processed:", must
    if can and must:
        print "Processing...",
        sys.stdout.flush()
        cc.process()
        print "done"
