
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
import tempfile

from soclib_cc.config import config

__author__ = 'Nicolas Pouillon, <nipo@ssji.net>'
__copyright__ = 'UPMC, Lip6, SoC, 2007-2010'
__license__ = 'GPL-v2'
__id__ = "$Id: command.py 1787 2010-06-07 21:50:25Z nipo $"
__version__ = "$Revision: 1787 $"

__all__ = ['Command']

class Command:

    __jobs = {}

    def __init__(self, cmd, cwd = None, on_done = None):
        self.__cmd = cmd
        self.__cwd = cwd
        self.__on_done = on_done
        self.__done = False
        self.__stdout = ''
        self.__stderr = ''

    @property
    def stdout(self):
        return self.__stdout

    @property
    def stderr(self):
        return self.__stderr

    @property
    def command(self):
        return ' '.join(map('"%s"'.__mod__, self.__cmd))

    def run(self, synchronous = False):
        import subprocess
        #print "---- run", cmd

        if config.verbose:
            print 'Running', ["async", "sync"][synchronous], self.command

        self.__out = tempfile.TemporaryFile("w+b", bufsize=128)
        self.__err = tempfile.TemporaryFile("w+b", bufsize=128)

        try:
            self.__handle = subprocess.Popen(
                self.__cmd,
                shell = False,
                cwd = self.__cwd,
                bufsize = 128*1024,
                close_fds = True,
                stdin = None,
                stdout = self.__out,
                stderr = self.__err,
                )
        except OSError, e:
            from action import ActionFailed
            raise ActionFailed(-1, self.command)

        if synchronous:
            self.__handle.wait()
            self.__out.seek(0)
            self.__err.seek(0)
            self.__stdout = self.__out.read()
            self.__stderr = self.__err.read()
            ret = self.__handle.returncode
            del self.__handle
            del self.__out
            del self.__err
            return ret
        
        self.__class__.__jobs[self.__handle.pid] = self

    @classmethod
    def pending_action_count(cls):
        return len(cls.__jobs)

    @classmethod
    def wait(cls):
        try:
            (pid, st) = os.wait()
            killed = st & 0x80
            sig = st & 0x7f
            ret = st >> 8
            cls.__jobs[pid].__handle.returncode = killed and -sig or ret
        except Exception, e:
            pass
        for job in cls.__jobs.values()[:]:
            p = job.__handle.poll()
#            print "---- poll", job, p
            if job.__handle.returncode is not None:
                job.__set_done()

    def __set_done(self):
        try:
            self.__handle.communicate()
        except:
            pass
        self.__out.seek(0)
        self.__err.seek(0)
        self.__stdout = self.__out.read()
        self.__stderr = self.__err.read()
        del self.__out
        del self.__err

        #print '--'
        del self.__class__.__jobs[self.__handle.pid]

        rc = self.__handle.returncode
        del self.__handle

        if rc == 0:
            self.__done = True

        if self.__on_done:
            self.__on_done(self, rc, self.__stdout, self.__stderr)

    def is_background(self):
        try:
            return self.__handle.poll() is None
        except:
            return False

class CreateFile:
    def __init__(self, filename, contents, on_done = None):
        self.__filename = filename
        self.__contents = contents
        self.__on_done = on_done

    def run(self, synchronous = False):
        if config.verbose:
            print 'Creating file', self.__filename
        if os.path.exists(self.__filename):
            fd = open(self.__filename, 'r+')
            old = fd.read()
            fd.seek(0)
            if old != self.__contents:
                fd.truncate()
                fd.write(self.__contents)
        else:
            fd = open(self.__filename, 'w')
            fd.write(self.__contents)
        fd.close()

        if self.__on_done:
            self.__on_done(self, 0, '', '')

    def is_background(self):
        return False

class CreateDir:
    def __init__(self, filename, on_done = None):
        self.__filename = filename
        self.__on_done = on_done

    def run(self, synchronous = False):
        if config.verbose:
            print 'Creating directory', self.__filename
        if not os.path.exists(self.__filename):
            os.makedirs(self.__filename)

        if self.__on_done:
            self.__on_done(self, 0, '', '')

    def is_background(self):
        return False
