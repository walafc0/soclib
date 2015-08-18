
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
__id__ = "$Id: spool.py 2022 2011-01-29 00:54:08Z nipo $"
__version__ = "$Revision: 2022 $"

import os
import os.path

__all__ = ['ConfigSpool']

from soclib_cc.config import objects

def _old_config(base, **kwargs):
    '''
    Emulation of the old Config() statement in configuration files.
    '''
    #warnings.warn("Config() statements are deprecated")
    if isinstance(base, objects.Toolchain):
        return objects.Toolchain(base, **kwargs)
    elif isinstance(base, objects.Library):
        return objects.Library(base, **kwargs)
    elif isinstance(base, objects.BuildEnv):
        if 'systemc' in kwargs:
            kwargs['libraries'] = [kwargs['systemc']]
            del kwargs['systemc']
        return objects.BuildEnv(base, **kwargs)
    raise ValueError("No valid base found")


class ConfigSpool(object):
    '''
    Root configuration class, the object in the config variable.

    This object has a special feature: it implicitely dereferences the
    defaut configuration when getting an attribute on it. This way,
    config.toolchain.something is in fact
    config.<default>.toolchain.something.
    '''

    # Global parameters, not tied to a specific configuration
    verbose = False
    quiet = False
    debug = False
    progress_bar = False
    max_name_length = 0
    mode = 'release'
    desc_parsers = ['sd_parser.sd.SdFile']
    output = 'system.x'
    sd_ignore_regexp = []
    workpath = 'work'

    def __init__(self, soclib_dir):
        '''
        Takes the root of the current soclib installation. Parses
        "built-in.conf" in the current directory as a starting point.
        '''
        self.__configs = {}
        self.path = soclib_dir
        self.__default_config = 'default'
        self.desc_paths = []

        config_file = os.path.join(
            os.path.dirname(__file__),
            'built-in.conf')
        objects.Config.unlock()
        self.__locked = False
        self.__include(config_file, False)
        objects.Config.lock()
        self.__locked = True

    def __include(self, filename, ignore_if_absent = False):
        '''
        Parses a file as a configuration file.
        '''
        if not os.path.exists(filename):
            if not ignore_if_absent:
                raise ValueError("File not found: '%s'"%filename)
            return
        globs = dict(
            config = self,
            Config = _old_config,
            BuildEnv = objects.BuildEnv,
            Library = objects.Library,
            Toolchain = objects.Toolchain,
            include = self.__include,
            )
        locs = {}
        exec file(filename) in globs, locs

    def set_default(self, name):
        """
        Sets the configuration used sor subsequent attribute gettings.
        """
        v = self.__configs[name]
        if not isinstance(v, objects.BuildEnv):
            raise ValueError("You must select a build environment (BuildEnv) as default, not a %s"%type(v).__name__)
        self.__default_config = name

    def available(self):
        """
        Lists the available configuration types
        """
        r = []
        for k, v in self.__configs.items():
            if not isinstance(v, objects.BuildEnv):
                continue
            r.append(k)
        r.remove(self.__default_config)
        r.insert(0, self.__default_config)
        return list(set(r))

    def __str__(self):
        r = objects._pformat(self.__configs, "",
                             dict_delim = ["configurations:", '']) + '\n'
        r += "default:" + self.__default_config + '\n'
        other = {}
        for k in dir(self):
            v = object.__getattribute__(self, k)
            if hasattr(v, '__call__') and not k.startswith('_'):
                continue
            if k.startswith('_'):
                continue
            if isinstance(v, objects.Config):
                continue
            other[k] = v
        return r + objects._pformat(other, "",
                                    dict_delim = ["other:", ''])

    def addDescPath(self, path):
        """
        Needed by the configuration file API, adds a description path
        to the default list. Accepts paths relative the soclib's root
        or absolute.

        :param path: Root path to add to soclib index path
        """
        if not os.path.isabs(path):
            path = os.path.join(self.path, path)
        self.desc_paths.append(path)

    def add_desc_parser(self, parser):
        '''
        Configuration file API, adds a class name to be imported,
        capable of parsing metadata files

        :param parser: A :ref:`metadata provider class
                       <soclib_desc-metadata_providers>` name string.
        '''
        self.desc_parsers.append(parser)


    def __getattr__(self, key):
        '''
        Attribute getter hack, returns a
        :py:class`soclib_cc.config.objects.Config` present in the
        spool by its name, or equivalent attribute in the defaut
        config.

        Special key "type" returns the current default config name.
        '''
        if key.startswith('_'):
            raise AttributeError(key)
        try:
            r = getattr(self.__configs[self.__default_config], key)
#            print "Getting %s from default" % key
            return r
        except AttributeError:
            pass
        except KeyError:
            pass
        if key == 'type':
            return self.__default_config
        if not self.__locked:
            if key == 'default':
                return self.__configs[self.__default_config]
            if key in self.__configs:
    #            print "Getting %s from configs" % key
                return self.__configs[key]
        raise AttributeError(key)

    def __setattr__(self, key, value):
        """
        Attribute setter.

        Handled cases:
         * Setting default (must be a Config present in __configs)
            -> changes __default_config
         * Setting a config with any key -> to __configs
         * Setting normal attribute
        """
        if key == 'default':
            for k, v in self.__configs.items():
                if v is value:
                    self.set_default(k)
                    return
            else:
                raise ValueError("Configuration is not present in config yet, cant set it default !")
        if isinstance(value, objects.Config):
            self.__configs[key] = value
            return
#        print "Setting attr %s to %s" % (key, value)
        object.__setattr__(self, key, value)

    def set(self, key, value):
        '''
        Sets an attribute, only supports setting attributes already
        present in class
        '''
        assert hasattr(self.__class__, key)
        object.__setattr__(self, key, value)
