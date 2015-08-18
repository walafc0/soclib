
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
__copyright__ = 'UPMC, Lip6, SoC, 2007-2011'
__license__ = 'GPL-v2'
__id__ = "$Id: module.py 1805 2010-06-09 14:33:51Z nipo $"
__version__ = "$Revision: 1805 $"

__all__ = ['ComponentBuilder']

class ComponentBuilderInterface:
    
    def results(self):
        """
        Returns a list of :py:class:`soclib_builder.bblock.BBlock`
        objects corresponding to products of module compilation.
        """
        return []
