
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

__all__ = ["MfRule"]

__id__ = "$Id: mfparser.py 1750 2010-05-26 09:49:35Z nipo $"
__version__ = "$Revision: 1750 $"

def bsfilter(spacer, l):
	r = []
	next_follows = False
	for i in l:
		nf = next_follows
		if i.endswith('\\'):
			next_follows = True
			i = i[:-1]+spacer
		else:
			next_follows = False
		if nf:
			r[-1] += i
		else:
			r.append(i)
	return r

class MfRule:
	def __init__(self, text):
		lines = filter(lambda x:not x.startswith('#'),
			bsfilter("", text.split('\n')))
		try:
			dest, prereq = lines[0].split(":",1)
		except ValueError:
			print lines[0]
			raise
		self.rules = lines[1:]
		self.dest = bsfilter(" ", dest.strip().split())
		self.prerequisites = bsfilter(" ", filter(None, map(
			lambda x:x.strip(), prereq.split())))
