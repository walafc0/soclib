
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

__all__ = ['checker']

__id__ = "$Id: abstraction_levels.py 1009 2009-04-10 11:54:37Z alinevieiramello@hotmail.com $"
__version__ = "$Revision: 1009 $"

class AbstractionLevelChecker:
	def __init__(self, level):
		self.__level = level
	
	def validClassName(self, name):
		return False

	def __str__(self):
		return '<%s>'%self.__level

class CppChecker(AbstractionLevelChecker):
	def validClassName(self, name):
		for c in name:
			if c.isalnum() or c in '_:<> ':
				continue
			return False
		return True
		
class RtlChecker(AbstractionLevelChecker):
	def validClassName(self, name):
		for c in name:
			if c.isalnum() or c in '_ ':
				continue
			return False
		return True

class Checker:
	
	def __init__(self, **handled_levels):
		self.__handled = {}
		for level, clas in handled_levels.iteritems():
			self.__handled[level] = clas(level)
		self.__unknown = AbstractionLevelChecker('unknown')
			
	def __getitem__(self, k):
		if k in self.__handled:
			return self.__handled[k]
		return self.__unknown

checker = Checker(
	sc = CppChecker,
	common = CppChecker,
	caba = CppChecker,
	tlmt = CppChecker,
	tlmdt = CppChecker,
	rtl = RtlChecker,
)
