
.. module:: soclib_cc.config.objects

Configuration objects
=====================

The ``Config`` base object
--------------------------

.. class:: Config

   This is the base configuration object. It handles all the attribute
   dereferencing features of the soclib configuration tool. Example
   session::

     >>> from soclib_cc.config.objects import *
     >>> c = Config(foo = "foo", bar = ["%(foo)s_value", "other_value"], baz = 42)
     >>> c.foo
     'foo'
     >>> c.baz
     42
     >>> c.bar
     ['foo_value', 'other_value']
     >>> c2 = Config(parent = c, foo = "zip")
     >>> c2.foo
     'zip'
     >>> c2.bar
     ['zip_value', 'other_value']
     >>> c2.baz
     42
     >>> c2["bar"]
     ['%(foo)s_value', 'other_value']

   .. method:: __init__(parent = None, **kwargs)

      Create a new Config object with an optional parent to retrieve
      default values from. All other parameters are fed to the
      configuration entry values.

      :param parent: Parent
                     :py:class:`~soclib_cc.config.objects.Config`
                     object to inherit from, if any
      
      :param kwargs: Key/value list of parameters to set in this
                     config entry


   .. method:: __getitem__(key)

      Returns the corresponding configuration value, without expanding
      ``%(..)s`` hacks.

      :param key: Key to retrieve value for. If not found in current
                  config, walk through parents.

      :raises: KeyError if not found

   .. method:: set(key, value)

      Set corresponding configuration value

   .. method:: __getattr__(key)

      Returns the corresponding configuration value, expanding
      ``%(..)s`` hacks.

      :param key: Key to retrieve value for. If not found in current
                  config, walk through parents.

      :raises: AttributeError if not found

   .. method:: __setattr__(key, value)

      Set corresponding configuration value, only possible if the key
      was already defined in object::

        >>> c = Config(foo = 1)
        >>> c.foo = 2
        >>> c.bar = 3
        Traceback (most recent call last):
          File "<stdin>", line 1, in <module>
          File "soclib_cc/config/objects.py", line 151, in __setattr__
            raise AttributeError("You cant add a new attribute afterwards")
        AttributeError: You cant add a new attribute afterwards
     
     :param key: Name of config entry to add
     :param value: Value of config entry to add

     :raises: AttributeError if setting this key is not permitted.

   .. attribute:: parent

      Pointer to the parent
      :py:class:`~soclib_cc.config.objects.Config` object.

   .. method:: pformat(indent = '')

      A recursive method for pretty-printing of the configuration
      object value
     
      :param indent: Indentation prefix of output

      :returns: a string containing the configuration values

   .. method:: get_flags(*flags)

      A getter helper for flags, where we need a concatenation of
      <something>_cflags and cflags. Concatenes all lists found named
      after ``flags``. Silently ignored if not found.
      
      :param flags: list of keys to retrieve

      :returns: concatenation of all the found lists

      Example::

        >>> c = Config(foo = ['a', 'b'], bar = ['c', 'd'])
        >>> c.get_flags('foo', 'bar', 'baz')
        ['a', 'b', 'c', 'd']

   .. classmethod:: lock()

      Make all the :py:class:`~soclib_cc.config.objects.Config`
      objects read-only.

   .. classmethod:: unlock()

      Make all the :py:class:`~soclib_cc.config.objects.Config`
      objects read-write.

The inherited objects
---------------------

.. class:: Toolchain(soclib_cc.config.objects.Config)

   A configuration object with specific features for toolchain
   support.

   .. method:: get_tool(name, mode)

      Retrieves a tool in the tool map. Tools are referenced with a
      special key format: ``tool_<mode>_<name>``. ``mode`` is
      optional.

      Example::

        >>> t = Toolchain(tool_foo = "abc", tool_debug_foo = "def")
        >>> t.get_tool("foo", "debug")
        ['def']
        >>> t.get_tool("foo", "other")
        ['abc']

.. class:: Library(soclib_cc.config.objects.Config)

   A configuration object with specific features for a Library.

   Libraries should always contain a ``name`` key.

The Build environment
---------------------

.. class:: BuildEnv(soclib_cc.config.objects.Config)

   A configuration object with specific features for a build
   environment. A build environment contains a set of libraries, a
   toolchain, and other flags.

   .. method:: get_library(name)

      Retrieves library by library name. i.e. get the
      :py:class:`~soclib_cc.config.objects.Library` object in the
      ``libraries`` list matching name.

      :param name: Name to match

      :returns: a matching
                :py:class:`~soclib_cc.config.objects.Library` object.

      Example::

        >>> l = Library(name = "foo", val = 42)
        >>> l2 = Library(name = "bar", val = 4096)
        >>> b = BuildEnv(libraries = [l, l2])
        >>> b.get_library("foo").val
        42
        
   .. method:: getTool(name, mode = "")

      Retrieves a tool through the ``toolchain`` object's
      :py:func:`soclib_cc.config.objects.Toolchain.get_tool` method
      for given `name` and `mode`.

   .. method:: getCflags(mode)

      Retrieves the concatenation of the ``cflags`` field from all
      libraries, toolchain, and build environment attributes. Also
      concatenates the ``<mode>_cflags`` attribute value if
      available.

      Example::

        >>> l = Library(name = "foo", cflags = ["-Ifoo"])
        >>> l2 = Library(name = "bar", cflags = ["-Ibar"], debug_cflags = ["-DBAR_DEBUG"])
        >>> t = Toolchain(cflags = ["-Itoolchain"])
        >>> b = BuildEnv(libraries = [l, l2], toolchain = t)
        >>> b.getCflags("release")
        ['-Ifoo', '-Ibar', '-Itoolchain']
        >>> b.getCflags("debug")
        ['-Ifoo', '-DBAR_DEBUG', '-Ibar', '-Itoolchain']

   .. method:: getLibs(mode)

      Retrieves the concatenation of the ``libs`` field from all
      libraries, toolchain, and build environment attributes. Also
      concatenates the ``<mode>_libs`` attribute value if
      available.

      This works exactly like
      :py:func:`soclib_cc.config.objects.BuildEnv.getCflags` method.

   .. method:: reposFile(name, mode = None)

      Create an unique absolute filename based on a passed relative
      name and mode.

      :param name: a relative file name
      :param mode: a mode prefix
      :returns: An absolute filename in the spool.

      Example::

        >>> b = BuildEnv(repos = "/tmp")
        >>> b.reposFile("foo.o")
        '/tmp/release/foo.o'
        >>> b.reposFile("foo.o", "debug")
        '/tmp/debug/foo.o'
