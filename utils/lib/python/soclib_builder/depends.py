
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

import os, os.path, time
import pickle

from soclib_cc.config import config

import bblock

__id__ = "$Id: depends.py 2016 2011-01-21 10:45:27Z nipo $"
__version__ = "$Revision: 2016 $"

__all__ = ['load', 'dump']

class MustRehash(Exception):
    pass

class DepPickler(pickle.Pickler):
    def __init__(self, filename):
        fd = open(filename, 'w')
        pickle.Pickler.__init__(self, fd, pickle.HIGHEST_PROTOCOL)
    def persistent_id(self, obj):
        if isinstance(obj, bblock.BBlock):
            return 'BBlock:'+str(obj)

def dump(name, deps):
    p = DepPickler(config.reposFile(name))
    p.dump(deps)

class DepUnpickler(pickle.Unpickler):
    def __init__(self, filename):
        try:
            fd = open(filename, 'r')
#           print 'loading back depends %s'%filename
        except IOError:
#           print '%s not loadable'%filename
            raise MustRehash()
        pickle.Unpickler.__init__(self, fd)
        self.last_mod = os.stat(filename).st_mtime
    def persistent_load(self, ident):
        mode,ide = ident.split(':',1)
        if mode == 'BBlock':
            r = bblock.bblockize1(ide)
            if r.exists() and r.mtime() > self.last_mod:
                raise MustRehash()
            return r

def load(name):
    p = DepUnpickler(config.reposFile(name))
    try:
        return p.load()
    except:
        raise MustRehash
