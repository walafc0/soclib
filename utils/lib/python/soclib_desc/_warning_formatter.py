
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

import warnings
import sys
import os

__id__ = "$Id: _warning_formatter.py 952 2009-03-16 17:55:51Z nipo $"
__version__ = "$Revision: 952 $"

_green = '\x1b[32m'
_red = '\x1b[31m'
_normal = '\x1b[m'

def formatwarning(message, category, filename, lineno, line = None):
	return "%s at %s%s%s:%d: %s%s%s\n"%(
		category.__name__,
		_green, filename, _normal, lineno,
		_red, message, _normal)

if sys.stderr.isatty() and not os.getenv('EMACS'):
	warnings.formatwarning = formatwarning

