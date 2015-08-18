
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
#         Nicolas Pouillon <nipo@ssji.net>, 2010

__id__ = "$Id: error.py 1862 2010-08-30 15:44:15Z nipo $"
__version__ = "$Revision: 1862 $"

from soclib_cc import exceptions

class SpuriousDeclarationWarning(Warning):
    def __str__(self):
        return 'Spurious "%s" in %s declaration'%(self.args[0], self.args[1])

class BadInterfacePath(Warning):
    def __str__(self):
        return 'Interface file path "%s" %s'%(self.args[0], self.args[1])

class BadNameWarning(Warning):
    def __str__(self):
        return 'Bad component name: `%s\', %s'%(self.args[0], self.args[1])

class InvalidComponentWarning(Warning):
    def __str__(self):
        return 'Invalid component %s, it will be unavailable. Error: "%s"'%(self.args[0], self.args[1])

class ModuleDeprecationWarning(DeprecationWarning):
    def __str__(self):
        return 'Module %s deprecated: "%s"'%(self.args[0], self.args[1])



class InvalidComponent(exceptions.ExpectedException):
    pass

class ModuleExplicitFailure(exceptions.ExpectedException):
    pass

class ModuleSpecializationError(exceptions.ExpectedException):
    """
    Error raised on module specialization when an error occurs below.
    We dont let the exception propagate because we loose the
    information about the faulty component. This exception saves the
    nested traceback to show it in the end.
    """
    max_stack_levels = 16
    
    def __init__(self, module, error):
        """
        module: the module name where the error occured
        error:  the error that occured
        """

        # Try to avoid big clutter when printing huge nested errors
        # but let soclib-cc -v print the whole if needed
        from soclib_cc.config import config
        self.__short = not config.verbose

        self.__module = module
        self.__prev_error = error

        import sys
        import traceback

        sei = sys.exc_info()
        exc_frame = traceback.extract_tb(sei[2])

        lines = traceback.format_list(exc_frame)
        if len(lines) > self.max_stack_levels+1 and self.__short:
            lines = lines[:self.max_stack_levels/2] + ['\n', '[%d lines ommitted]\n'%(len(lines)-self.max_stack_levels), '\n'] + lines[-self.max_stack_levels/2:]
        self.__exc = ''.join(lines)

    @property
    def actual_error(self):
        """
        This is the inner-most error that must be displayed to the
        user.
        """
        error = self.__prev_error
        if isinstance(error, ModuleSpecializationError):
            return error.actual_error
        return self.__prev_error

    @property
    def path(self):
        """
        This is the specialization path to the inner-most error.
        """
        if isinstance(self.__prev_error, ModuleSpecializationError):
            return self.__module + " -> " + self.__prev_error.path
        return self.__module
        

    def __str__(self):
        r = '\n\n'+self.__format()
        r += '\n'
        r += '\nSpecialization path: '+self.path
        r += '\n'
        r += '\n'+self.__format_exc(self.actual_error)
        return r

    def __format_exc(self, err):
        import traceback
        return ''.join(traceback.format_exception_only(err.__class__, err))

    def __format_previous(self):
        error = self.__prev_error
        r = '\n'
        if isinstance(error, ModuleSpecializationError):
            if not self.__short:
                r += self.__exc
            r += error.__format()
        else:
            r += self.__exc
            r += self.__format_exc(error)
        return r.replace("\n", "\n| ")

    def __format(self):
        r = 'Error specializing module "%s", nested error:'%(self.__module)
        r += "\n" + self.__format_previous()
        return r
