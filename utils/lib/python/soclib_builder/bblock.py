
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
import time

from soclib_cc.config import config

__id__ = "$Id: bblock.py 1750 2010-05-26 09:49:35Z nipo $"
__version__ = "$Revision: 1750 $"

_global_bblock_registry = {}

class BBlock:
    def __init__(self, filename, generator = None):
        self.is_blob = False
        assert filename
        if generator is None:
            from action import Noop
            generator = Noop()
        self.__filename = filename
        assert filename not in _global_bblock_registry
        _global_bblock_registry[filename] = self
        
        self.generator = generator
        self.__i_need = None
        self.__needed_by = None
        self.__prepared = False
        self.__users = set()

        self.__rehash()

    @property
    def users(self):
        return self.__users

    def addUser(self, gen):
#        print 'addUser', repr(self), str(gen)[:40] + '...'
        self.__users.add(gen)

    def touch(self):
        mt = self.__mtime
        e = self.__exists
        self.__rehash()
        if mt != self.__mtime or e != self.__exists:
            for u in self.__users:
                u.source_changed()

    def is_dir(self):
        return os.path.isdir(self.__filename)

    def setIsBlob(self, val = True):
        self.is_blob = val

    def __rehash(self):
        if os.path.exists(self.__filename):
            self.__exists = True
            self.__mtime = os.stat(self.__filename).st_mtime
        else:
            self.__exists = False
            self.__mtime = time.time() + 4096

    def mtime(self):
        return self.__mtime

    def delete(self):
        if os.path.isfile(self.__filename):
            try:
                os.unlink(self.__filename)
            except OSError:
                pass
        self.__rehash()

    def generate(self):
        self.generator.process()

    def exists(self):
        return self.__exists

    def __str__(self):
        return self.__filename

    def __repr__(self):
        return '{%s}' % os.path.basename(self.__filename)

    def __hash__(self):
        return hash(self.__filename)

    def __eq__(self, other):
        return self.__filename == other.__filename

    def __walk_i_need(self):
        if self.__i_need is not None:
            return self.__i_need

        self.__i_need = set()
        for d in self.generator.todo_get_depends():
            if d in self.__i_need:
                continue
            self.__i_need.add(d)
            self.__i_need |= d.__walk_i_need()
        return self.__i_need

    def __walk_needed_by(self):
        if self.__needed_by is not None:
            return self.__needed_by

        self.__needed_by = set()
        for u in self.__users:
            for d in u.dests:
                if d is self.__needed_by:
                    continue
                self.__needed_by.add(d)
                self.__needed_by |= d.__walk_needed_by()
        return self.__needed_by

    def prepare(self):
        if self.__prepared:
            return

        self.__walk_needed_by()
        self.__walk_i_need()

        self.__prepared = True

    def needs(self, other):
        return (
            (other in self.__i_need or self in other.__needed_by) and not
            (self in other.__i_need or other in self.__needed_by)
            )

class AnonymousBBlock(BBlock):
    def __init__(self, builder):
        self.__done = False
        BBlock.__init__(self, '__anon_'+hex(id(builder)), builder)

    def touch(self):
        self.__done = True
        
    def exists(self):
        return self.__done

    def delete(self):
        self.__done = False

    def prepare(self):
        BBlock.prepare(self)
        self.__done = False

    def __repr__(self):
        return '{anon %03d}' % (hash(str(self)) % 1000)

def bblockize1(f, gen = None):
    if config.debug:
        print 'BBlockizing', f,
    if isinstance(f, BBlock):
        if config.debug:
            print 'already is'
        return f
    if f in _global_bblock_registry:
        r = _global_bblock_registry[f]
        if config.debug:
            print 'in global reg', r
        from action import Noop
        if gen is not None:
            if isinstance(r.generator, Noop):
                r.generator = gen
            elif r.generator.__class__ is not gen.__class__:
                raise ValueError('Generator in reg: %s, passed: %s'%(r.generator.__class__, gen.__class__))
        return r
    if config.debug:
        print 'needs BBlock'
    return BBlock(f, gen)

def bblockize(files, gen = None):
    return map(lambda x:bblockize1(x, gen), files)

def filenames(bblocks):
    ret = []
    for b in bblocks:
        assert isinstance(b, BBlock)
        if isinstance(b, AnonymousBBlock):
            continue
        ret.append(str(b))
    return ret
