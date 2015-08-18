
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

import parameter
import module
import description_files
import warnings
from error import *

__id__ = "$Id: specialization.py 1751 2010-05-27 07:28:23Z nipo $"
__version__ = "$Revision: 1751 $"

class SpecializationInterface:

    def builder(self):
        '''
        Retrieves the soclib_cc builder instance able to compile the
        specialization.
        '''
        raise NotImplementedError()

    def port_list(self):
        '''
        Retrieves the port list. Some ports may depend from instance
        parameters.
        '''
        raise NotImplementedError()

    def implementation_language(self):
        '''
        Retrieves the native language of the module. It should probably
        be vhdl, systemc, verilog, ...
        '''
        raise NotImplementedError()

    def get_header_files(self):
        '''
        Retrieves a collection of absolute header file paths to
        include when using this module.
        '''
        raise NotImplementedError()

    def get_interface_files(self):
        '''
        Retrieves a collection of absolute interface file paths to
        include from software using this module.
        '''
        raise NotImplementedError()

    def get_used_modules(self):
        '''
        Returns set of specializations used as dependancy of this
        one. This is the exhaustive list of needed modules, even if
        deep in the subtree.
        '''
        raise NotImplementedError()

    def __hash__(self):
        '''
        Returns an uid for the specialization
        '''
        raise NotImplementedError()

    def __eq__(self, other):
        '''
        Tests whether the two specializations are actually equal
        '''
        return False

    def get_extensions(self, namespace):
        '''
        Gets extensions defined for a given namespace. Extensions are
        each in a string.
        '''
        raise NotImplementedError()

    def get_entity_name(self):
        '''
        Retrieves a string corresponding to the whole entity
        name.

        * For SystemC modules, this is a class name with qualified
          namespace and template parameters.
        * For rtl modules, this is a simple entity name
        '''
        raise NotImplementedError()

    def get_instance_parameters(self):
        '''
        List of parameters to be passed at run time, these are the
        necessary parameters for getting a valid module instanciation.
        Depending on the module implementation language, this may be a
        string, a list of parameters, or a dictionary.
        '''
        raise NotImplementedError()

class SignalSpecializationInterface:

    def port_max_count(self, port):
        """
        Returns a maximal count of connectable port of type passed in
        'port'. 'port' is a specialization.
        """
        raise NotImplementedError()

    def can_meta_connect(self):
        '''
        Returns whether the signal can be connected at once
        '''
        return False

class PortSpecializationInterface:

    def can_connect(self, signal):
        '''
        Returns whether the port can be connected to the signal. This
        does not assert limitations on connection count.
        '''
        return False

    def get_signal(self):
        '''
        Returns the signal capable of connection to this port.
        '''
        raise NotImplementedError()

    def can_meta_connect(self):
        '''
        Returns whether the port can be connected at once
        '''
        return False

class Port:

    def __init__(self, spec, count = None, auto = None):
        self.__spec = spec
        self.__count = count
        self.__auto = auto

    def array_size(self):
        '''
        Returns the array size. Returns None if not part of an array.
        '''
        return self.__count

    @property
    def specialization(self):
        return self.__spec

    @property
    def auto(self):
        return self.__auto
