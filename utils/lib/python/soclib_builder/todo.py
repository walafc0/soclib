
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
__id__ = "$Id: todo.py 1864 2010-08-31 14:45:01Z nipo $"
__version__ = "$Revision: 1864 $"

import sys
import os, os.path

from soclib_cc.config import config
from bblock import bblockize, BBlock, filenames
from action import Noop, ActionFailed, Action
import command
from soclib_utils.terminal import terminal_width

__all__ = ['Todo']

class ToDo:
    def __init__(self, *dests):
        self.has_blob = False
        self.dests = []
        self.add(*dests)
        self.prepared = False
        d = config.reposFile('')
        if not os.path.isdir(d):
            os.makedirs(d)
        self.max_actions = config.toolchain.max_processes
        self.__term_width = terminal_width()
        self.__todo = []

    def add(self, *dests):
        self.dests += bblockize(dests)
        for d in self.dests:
            self.has_blob |= d.is_blob

    def _getall_bbs(self, dests):
        dest_bb_list = set(dests)
        viewed_bb_list = set()
        todo_generator_list = set()

        while dest_bb_list:
            bb = dest_bb_list.pop()
            viewed_bb_list.add(bb)

            gen = bb.generator
            gen.prepare()
            dest_bb_list |= set(gen.todo_get_depends())
            todo_generator_list.add(gen)
#            print gen
            
            dest_bb_list -= viewed_bb_list


        def cr(x):
            return not isinstance(x, Noop)
        todo_generator_list = filter(cr, todo_generator_list)
        return todo_generator_list, viewed_bb_list
    
    def prepare(self):
        if self.prepared:
            return
        
        try:
            todo, bblocks = self._getall_bbs(self.dests)
        except ActionFailed, e:
            return self.handle_failed_action(e)

        for b in bblocks:
            b.generator.prepare()

        for b in bblocks:
            b.prepare()

        sorted_bblocks = []
        for b in bblocks:
            done = False
            for i, bb in reversed(list(enumerate(sorted_bblocks))):
                if b.needs(bb):
                    sorted_bblocks.insert(i+1, b)
                    done = True
                    break
            if not done:
                sorted_bblocks.insert(0, b)
        bblocks = sorted_bblocks

        self.__todo = []
        viewed = set()
        for b in bblocks:
            g = b.generator
            if isinstance(g, Noop) or g in viewed:
                continue
            self.__todo.append(g)
            viewed.add(g)

        if set(self.__todo) < set(todo):
            print 'Lost generators in battle:'
            for i in set(todo) - set(self.__todo):
                for d in i.dests:
                    print d in self.dests, d.generator.__class__, d
            raise RuntimeError()

#        self.__todo.reverse()
        self.prepared = True

#        for i, g in enumerate(self.todo):
#            print i, filenames(g.dests), filenames(g.sources)
        
    def clean(self):
        self.prepare()
        for i in self.__todo:
            i.todo_clean()

    def handle_failed_action(self, e):
        print "soclib-cc: *** Action failed with return value `%s'. Stop."%e.rval
        if self.has_blob:
            print '***********************************************'
            print '***********************************************'
            print '**** WARNING, YOU USED BINARY-ONLY MODULES ****'
            print '***********************************************'
            print '***********************************************'
            print 'If you compilation failed because of linkage, this is most'
            print 'likely a mismatch between expected libraries from a binary'
            print 'only module and your system libraries (libstdc++, SystemC, ...)'
            print
            print "\x1b[91mPlease don't report any error about binary modules"
            print "to SoCLib-CC maintainers, they'll ignore your requests.\x1b[m"
            print 
        if config.verbose:
            print "soclib-cc: Failed action: `%s'"%e.action
        else:
            import shlex
            act = []
            was = None
            cmdlist = e.action
            if not isinstance(cmdlist, (list, tuple)):
                cmdlist = shlex.split(cmdlist)
            for a in cmdlist:
                s = None
                if a.startswith('-I'):
                    s = '-I'
                if was != s and s:
                    act.append(s+'...')
                elif not s:
                    act.append(a)
                was = s
            print "soclib-cc: Failed action: `%s'" % ' '.join(act)
            print "soclib-cc: Command line shortened, rerun with -v for complete command line"
        if command.Command.pending_action_count():
            print "soclib-cc: Waiting for unfinished jobs"
            self.__wait_done()
            raise

    def wait(self):
        command.Command.wait()

    def __wait_done(self):
        while command.Command.pending_action_count():
            self.progressBar()
            command.Command.wait()

    def process(self):
        import sys
        self.prepare()
        
        if config.debug:
            print 'Would do:'
            print "="*80
            print "="*80
            for i, g in enumerate(self.__todo):
                print i, g.is_valid(), str(g)
            print "="*80
            print "="*80

#        self.infoBar()
        self.progressBar()

        while self.__build_as_much_as_possible() == False:
            self.progressBar()
        self.progressBar()
        if config.progress_bar:
            print

    def __build_as_much_as_possible(self):
        wont = filter(lambda x:x.todo_state() in [Action.FAILED, Action.WORKING],
                      self.__todo)
        left = filter(lambda x:x.todo_state() in [Action.TODO, Action.BLOCKED],
                      self.__todo)
        if not left:
            return True
        possible = filter(lambda x:x.todo_can_be_processed(),
                          left)
        if left and not possible and not wont:
            print 'Left:'
            for i in left:
                i.why_blocked()
            raise RuntimeError()

        try:
            self.__run(possible)
        except ActionFailed, e:
            return self.handle_failed_action(e)
        except OSError, e:
            if hasattr(e, 'child_traceback'):
                print e.child_traceback
            raise
        return False

    def __run(self, left):
        # pop() takes from the end...
        left.reverse()

#        print 'run'
        
        while left:
            todo = left.pop()
            if todo.todo_state() != Action.TODO:
                continue

            while command.Command.pending_action_count() >= self.max_actions:
                self.wait()

            if not todo.todo_can_be_processed():
                continue

            todo.todo_launch()
            self.progressBar()
        self.__wait_done()

    @staticmethod
    def __progress_bar_code(state):
        return {
            Action.TODO: ' ',
            Action.BLOCKED: '.',
            Action.DONE: '=',
            Action.WORKING: 'W',
            Action.FAILED: '!',
            }[state]

    def progressBar(self):
        if not config.progress_bar or not self.prepared:
            return
        if self.__term_width < len(self.__todo) + 12:
            self.__short_progress()
        else:
            self.__long_progress()

    def __long_progress(self):
        pb = ""
        left = 0
        for pi in self.__todo:
            s = pi.todo_state()
            if s == Action.TODO:
                left += 1
            pb += self.__progress_bar_code(s)
        sys.stdout.write('\r['+pb+']')
        sys.stdout.write(' %d left ' % left)
        sys.stdout.flush()

    def __short_progress(self):
        states = {
            Action.TODO : 'Todo',
            Action.BLOCKED : 'Blocked',
            Action.DONE : 'Done',
            Action.WORKING : 'Working',
            Action.FAILED : 'Failed',
            }
        counts = dict([(k, 0) for k in states.keys()])
        for i in self.__todo:
            counts[i.todo_state()] += 1
        pc = 100. * counts[Action.DONE] / len(self.__todo)
        sys.stdout.write('\r % 3d%% done' % int(pc))
        for k in sorted(states.keys()):
            sys.stdout.write(', %3d %s' % (counts[k], states[k]))
        sys.stdout.write('      \r')
        sys.stdout.flush()

    def infoBar(self):
        if not config.progress_bar:
            return
        pb = ""
        for pi in self.__todo:
            pb += pi.info_code
        sys.stdout.write('['+pb+']\n')
        sys.stdout.flush()
    
    def format(self, formatter_class_name, output):
        self.prepare()
        
        nodes = formatter_class_name.split('.')
        module = '.'.join(nodes[:-1])
        tmp = __import__(module, globals(), locals(), [nodes[-1]])
        formatter = getattr(tmp, nodes[-1])

        f = formatter(output)
        for a in self.__todo:
            f.format_action(a)

            
