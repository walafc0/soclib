

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
__copyright__ = 'UPMC, Lip6, SoC, 2007-2010'
__license__ = 'GPL-v2'
__id__ = "$Id: objects.py 2025 2011-01-29 11:39:44Z nipo $"
__version__ = "$Revision: 2025 $"

import os
import os.path

__all__ = ['BuildEnv', 'Library', 'Toolchain', 'Config', '_pformat']

def _pformat(data, indent, dict_delim = ['dict(', ')']):
    '''
    Pretty printer capable of handling:
     * dicts
     * lists and tuples (printed as lists)
     * Config (uses pprint())
     * other datatypes (using repr)
    '''
    r = ''
    if isinstance(data, dict):
        r += dict_delim[0]
        for k in sorted(data.keys()):
            v = data[k]
            r += '\n'
            r += indent + '  ' + k + ' = '
            r += _pformat(v, indent+'    ')
            r += ','
        r += '\n'
        r += indent + dict_delim[1]
    elif isinstance(data, (list, tuple)):
        r += '['
        for i in data:
            r += '\n'
            r += indent + '  '
            r += _pformat(i, indent+'  ')
            r += ','
        r += '\n'
        r += indent + ']'
    elif isinstance(data, Config):
        r += data.pformat(indent)
    else:
        r += repr(data)
    return r

class Config(object):
    __locked = True

    @classmethod
    def lock(cls):
        cls.__locked = True

    @classmethod
    def unlock(cls):
        cls.__locked = False

    '''
    Base configuration object, handles all the attribute
    dereferencing.

    Also handles the %()s expansion when attributes are referenced
    '''
    def __init__(self, parent = None, **kwargs):
        '''
        parent is an optional parent Config.
        kwargs are set values
        '''
        self.__parent = parent
        self.__args = kwargs

    def __getitem__(self, key):
        """
        This gets a value from the __args, if not found, recurses down
        to parents.

        Using __getitem__ permits writing 'str % self'
        """
        try:
            return self.__args[key]
        except:
            pass
        if self.__parent:
            return self.__parent[key]
        raise KeyError(key)

    def __remap(self, value):
        '''
        Does the %()s expansion on the value given. It silently
        ignores anything other than a string.
        '''
        if not isinstance(value, basestring):
            return value
        if not self.__locked:
            return str(value)
        old = ''
        while old != value:
            old, value = value, os.path.expandvars(value % self)
        return value

    def __getattr__(self, key):
        '''
        Does the magical attribute getting hackery.
         * Supports getting all variables present in __args
         * Passes strings, dicts, lists and tuples through the
           remapping feature
        '''
        if key.startswith('__'):
            raise AttributeError(key)
        try:
            value = self[key]
        except KeyError:
            raise AttributeError(key)
        if isinstance(value, (list, tuple)):
            return map(self.__remap, value)
        if isinstance(value, dict):
            return dict([(k, self.__remap(v)) for
                         (k, v) in value.items()])
        if isinstance(value, str):
            value = self.__remap(value)
        return value

    def set(self, key, val):
        self.__args[key] = val

    def __setattr__(self, key, val):
        if key.startswith('_'):
            object.__setattr__(self, key, val)
            return
        found = False
        try:
            getattr(self, key)
        except:
            raise AttributeError("You cant add a new attribute afterwards")
        self.__args[key] = val

    @property
    def parent(self):
        '''
        Parent of current Config, None if no parent.
        '''
        return self.__parent

    def pformat(self, indent = ''):
        '''
        Pretty printer. Basically a str() with an optional indent
        argument.
        '''
        ks = set()
        p = self
        while p:
            ks |= set(p.__args.keys())
            p = p.parent
        data = dict((k, getattr(self, k)) for k in ks)
        return _pformat(data, indent,
                        dict_delim = [self.__class__.__name__+'(', ')'])

    def get_flags(self, *flags):
        '''
        A getter helper for flags, where we need a concatenation of
        <something>_cflags and cflags. Concatenes all lists found
        named after *flags. Silently ignored if not found.
        '''
        flag_list = []
        for f in flags:
            try:
                flag_list += getattr(self, f)
            except:
                pass
        return flag_list

class Toolchain(Config):
    '''
    Implements toolchain specificities
    '''

    def __get_tool(self, name):
        try:
            return getattr(self, 'tool_'+name)
        except AttributeError:
            pass
        try:
            tm = self.tool_map
        except:
            tm = {}
        if name in tm:
            #warnings.warn(
            #    "Please migrate tool_map[%s] entry to tool_%s" % (name, name),
            #    DeprecationWarning)
            return tm[name]
        if self.parent:
            return self.parent.__get_tool(name)
        raise KeyError(name)

    def get_tool(self, name, mode):
        '''
        Retrieves a tool in the tool_map field. Recurses down to
        parent until found.
        '''
        tn = mode+'_'+name
        try:
            tool = self.__get_tool(tn)
        except KeyError:
            try:
                tool = self.__get_tool(name)
            except KeyError:
                tool = None
        if tool is not None:
            if isinstance(tool, str):
                return tool.split(' ')
            else:
                return tool
        raise KeyError(name)

class Library(Config):
    pass

class BuildEnv(Config):

    # Private API

    def __get_flags(self, *flags):
        flag_list = []
        for l in self.libraries:
            flag_list += l.get_flags(*flags)
        flag_list += self.toolchain.get_flags(*flags)
        return flag_list

    def __get_tool(self, name, mode):
        """
        Retrieves a tool in the toolchain's tool_map field. Recurses
        down to parent until found. Beware of double parent recursion:
         * <me>
           * <me.toolchain>
             * <me.toolchain.parent>
               * ...
         * <me.parent.toolchain>
           * <me.parent.toolchain.parent>
             * ...
        """
        try:
            return self.toolchain.get_tool(name, mode)
        except KeyError:
            pass
        if self.parent:
            return self.parent.__get_tool(name, mode)
        return [self.toolchain.prefix + name]

    # User API

    def get_library(self, name):
        for l in self.libraries:
            if l.name == name:
                return l
        raise ValueError("No library %s found" % name)

    def getTool(self, name, mode = ''):
        '''
        Use API helper, see __get_tool
        '''
        return self.__get_tool(name, mode)

    def getCflags(self, mode=None):
        '''
        Use API helper, see __get_flags and Config.get_flags
        '''
        if mode is None:
            from soclib_cc.config import config
            mode = config.mode
        return self.__get_flags(mode+'_cflags', 'cflags')

    def getLibs(self, mode=None):
        '''
        Use API helper, see __get_flags and Config.get_flags
        '''
        if mode is None:
            from soclib_cc.config import config
            mode = config.mode
        return self.__get_flags(mode+'_libs', 'libs')

    def reposFile(self, name, mode=None):
        '''
        Retrieves an unique filename based on the name and the
        "repos" location.
        '''
        from soclib_cc.config import config
        if mode is None:
            mode = config.mode
        if config.max_name_length:
            if len(name) > config.max_name_length:
                name_ = name
                name, ext = os.path.splitext(name_)
                name = '_'+hex(hash(name))+ext
                name = name_[:config.max_name_length-1-len(name)]+name
        r = os.path.join(self.repos, mode, name)
        if config.max_name_length:
            r = r.replace(':','_')
        return r
