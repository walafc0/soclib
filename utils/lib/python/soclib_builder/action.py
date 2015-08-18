
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

import os
import os.path
import time
import sys
import select
import tempfile
import operator
try:
    from functools import reduce
except:
    pass

from soclib_cc.config import config
import command

__author__ = 'Nicolas Pouillon, <nipo@ssji.net>'
__copyright__ = 'UPMC, Lip6, SoC, 2007-2010'
__license__ = 'GPL-v2'
__id__ = "$Id: action.py 1750 2010-05-26 09:49:35Z nipo $"
__version__ = "$Revision: 1750 $"

__all__ = ['Action', 'ActionFailed', 'NotFound', 'Noop']

class NotFound(Exception):
    pass

class ActionFailed(Exception):
    def __init__(self, rval, action):
        Exception.__init__(self, "%s failed: %s"%(action, rval))
        self.rval = rval
        self.action = action

def get_times(files, default, cmp_func, ignore_absent):
    most_time = default
    for f in files:
        if f.exists():
            most_time = cmp_func((f.last_mod, most_time))
        else:
            if ignore_absent:
                continue
            else:
                raise NotFound, f
    return most_time

def merge_hash(objs):
    return hash(reduce(
        lambda x,y:(x + (y<<1)),
        map(hash, objs),
        0))

get_newest = lambda files, ignore_absent: get_times(files, 0, max, ignore_absent)
get_oldest = lambda files, ignore_absent: get_times(files, time.time(), min, ignore_absent)

def check_exist(files):
    return reduce(operator.and_, map(lambda x:x.exists(), files), True)

class Action:
    priority = 0
    info_code = ' '
    
    WORKING = 'Work'
    TODO = 'Todo'
    DONE = 'Done'
    FAILED = 'Failed'
    BLOCKED = 'Blocked'

    def __init__(self, dests, sources, **options):
        from bblock import bblockize, BBlock, AnonymousBBlock
        if not dests:
            self.dests = [AnonymousBBlock(self)]
        else:
            self.dests = bblockize(dests, self)
        self.sources = bblockize(sources)
        self.options = options
        
        self.__hash = merge_hash(self.dests+self.sources)

        self.__state = self.TODO
        self.__depends = []
        self.__commands = []
        
        map(lambda x:x.addUser(self), self.sources)

    def prepare(self):
        import fileops
        for outdir in self.dests:
            d = os.path.dirname(str(self.dests[0]))
            if d:
                self.add_depends(*fileops.CreateDir(d).dests)

        self.__state = [self.TODO, self.DONE][self.is_valid()]

        self.prepare = lambda :None

    def is_valid(self):
        try:
            newest_dep = 0
            oldest_dest = time.time()
            for d in self.__depends + self.sources:
                if d.is_dir():
                    continue
                if not d.exists():
                    return False
                newest_dep = max((newest_dep, d.mtime()))
            for d in self.dests:
                if not d.exists():
                    return False
                oldest_dest = min((oldest_dest, d.mtime()))
            
            return oldest_dest >= newest_dep
        except NotFound:
            return False

    # Called by children to add things

    def add_depends(self, *deps):
        map(lambda x:x.addUser(self), deps)

        self.__depends += list(deps)

    def run_command(self, cmd, cwd = None):
        if isinstance(cmd, (str, unicode)):
            raise ValueError("cmd must be a list of string")

        cmd = command.Command(cmd,
                              cwd = cwd,
                              on_done = self.__set_done)
        self.__commands.append(cmd)

    def create_file(self, f_bblock, contents):
        cmd = command.CreateFile(str(f_bblock), contents,
                                 on_done = self.__set_done)
        self.__commands.append(cmd)

    # Called by children to process synchronously

    def process(self):
        self.prepare()

        if config.debug:
            print 'Synchronously processing', self,
        if self.__state == self.DONE:
            if config.debug:
                print 'already done'
            return

        for d in list(self.__depends) + list(self.sources):
            if d.generator.__state != self.DONE:
                d.generator.process()

        if config.debug:
            print 'launching...', self.__commands
        self.todo_launch(True)

    # Private subprocess handling

    def stdout_reformat(self, msg):
        '''
        Reformat stdout, to make error messages of specific tools
        compatible with what is parsed by usual compilation tools.
        '''
        return msg

    def stderr_reformat(self, msg):
        '''
        Reformat stderr, to make error messages of specific tools
        compatible with what is parsed by usual compilation tools.
        '''
        return msg

    def __set_done(self, cmd, returncode, out, err):

        if out:
            out = self.stdout_reformat(out)
            sys.stdout.write('\n')
            sys.stdout.write(out)

        if err:
            err = self.stderr_reformat(err)
            sys.stderr.write('\n')
            sys.stderr.write(err)

        if returncode:
            self.__state = self.FAILED
            if returncode == -2: # sigint
                raise KeyboardInterrupt()
            raise ActionFailed(returncode, cmd.command)

        for d in self.dests:
            d.touch()

        self.__next()

    def __next(self):
        if len(self.__commands) == self.__cpt:
            self.__state = self.DONE
            return
        cmd = self.__commands[self.__cpt]
        self.__cpt += 1

        cmd.run(self.__sync)

    # Todo API

    def todo_launch(self, synchronous = False):
        self.__cpt = 0
        self.__state = self.WORKING
        self.__sync = synchronous
        self.__next()

    def todo_state(self):
        return self.__state

    def todo_can_be_processed(self):
        if not check_exist(self.sources):
            self.__state = self.BLOCKED
            return False
        for d in list(self.__depends) + list(self.sources):
            if d.generator.__state != self.DONE:
                self.__state = self.BLOCKED
                return False
        self.__state = self.TODO
        return True

    def why_blocked(self):
        print self, 'blocked because'
        if not check_exist(self.sources):
            print "not all sources"
        for d in list(self.__depends) + list(self.sources):
            if d.generator.__state != self.DONE:
                print d.generator, "not ready:", d.generator.__state

    def todo_get_depends(self):
        return list(self.__depends) + list(self.sources)

    def todo_clean(self):
        for i in self.dests:
            i.delete()

    def source_changed(self):
        self.todo_can_be_processed()

    # Helpers

    def __hash__(self):
        return self.__hash

    def __eq__(self, other):
        return self.__class__ is other.__class__ and \
               set(self.sources) == set(other.sources) and \
               set(self.dests) == set(other.dests)

    def __str__(self):
        import bblock
        l = lambda x:map(repr, x)
        return "<%s: %s -> %s + %s>"%(
            self.__class__.__name__,
            l(self.sources),
            l(self.dests),
            l(self.__depends),
            )
            

class Noop(Action):

    def __init__(self):
        Action.__init__(self, [], [])

    def is_valid(self):
        return True
