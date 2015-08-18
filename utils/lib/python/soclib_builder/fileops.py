
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

import os
import os.path
import action

__id__ = "$Id: fileops.py 1750 2010-05-26 09:49:35Z nipo $"
__version__ = "$Revision: 1750 $"

class CreateDir(action.Action):
    info_code = 'D'

    def __init__(self, directory):
        action.Action.__init__(self, [directory], [])

    def is_valid(self):
        return os.path.isdir(str(self.dests[0]))

    def prepare(self):
        for b in self.dests:
            self.run_command(['mkdir', '-p', str(b)])
        action.Action.prepare(self)
