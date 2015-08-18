.. -*- rst-mode -*-

.. _soclib-cc:

=========================
The ``soclib-cc`` command
=========================

.. program:: soclib-cc

The main entry point in soclib is the ``soclib-cc`` command.

``soclib-cc`` can be used to serve different pusposes. Among them:

* build a platform, a component.

* get cflags needed to access compilation headers for software-centric
  view of the modules. This is used to build an embedded software.

* gather information about a module's :ref:`md`.

* gather information about the current soclib installation and
  configuration.

Built-in quick help
-------------------

.. option:: --help

Like other programs, ``soclib-cc`` embeds an in-line help option, to
quickly list all available options.

Configuration information
-------------------------

.. option:: --getpath

Retrieves the current absolute installation path of the SoCLib
library::

  $ soclib-cc --getpath
  /opt/soclib

.. option:: --dump-config

Dumps all the configuration, in a pretty-printed (yet verbose)
manner. This is mostly usedful to understand what soclib-cc actually
understand from the user's configuration file.

Gather information about models
-------------------------------

These options access the :ref:`md-index`.

.. option:: --list-descs=names

This lists all the known modules names in the index.

.. option:: -l, --list-descs=long

This lists all the known modules in the index. Output is the name of
the module (the index key), and various properties.

.. option:: -X

This cleans the metadata cache.

.. option:: --complete-name=COMPLETE_NAME

This completes a given module name with what is found in the index,
for names starting with ``COMPLETE_NAME``. This can serve as a base
for shell-completion mechanisms. Example::

  $ soclib-cc --complete-name=caba:vci_mw
  vci_mwmr_controller
  vci_mwmr_controller_lf
  vci_mwmr_stats

.. option:: --complete-separator=COMPLETE_SEPARATOR

Output words to complete start at separator
``COMPLETE_SEPARATOR``. This can help working around readline's
lexing. Example::

  $ soclib-cc --complete-name=caba:vci_mw --complete-separator=_
  mwmr_controller_lf
  mwmr_stats
  mwmr_controller
  $ soclib-cc --complete-name=caba:vci_mw --complete-separator=
  caba:vci_mwmr_controller_lf
  caba:vci_mwmr_controller
  caba:vci_mwmr_stats


Tampering with module definition
--------------------------------

Module index can also be accessed to change the defintion of modules.

.. option:: -I PATH

Add the specified ``PATH`` to the list of directories searched for
modules definitions. Paths listed with this option are searched first
when looking for a module.

.. option:: -b MODULE_NAME, --buggy=MODULE_NAME

Put the said module in :ref:`build-debug_mode`.

.. option:: --work=DIR

Use DIR as `work` directory for EDA tools requiring such a directory
(`modelsim`, etc.).


Platform or module compilation
------------------------------

.. option:: -o OUTPUT

Select the output file for compilation of the module or component. If
compiling a component, output should be an object file name (ends with
``.o``); if a platform, it is limited by the host OS rules (should end
with ``.exe`` on Windows).

.. option:: -p PLATFORM

Compiles a platform defined in the ``PLATFORM`` file. See
:ref:`pf-def` for more information about this file's format.

.. option:: -1 ONE_MODULE, --one-module=ONE_MODULE

Compiles just one module and generates one object file containing the
whole module code.  Use :option:`soclib-cc -a` to specify compile-time
parameters.

.. option:: -a NAME=VALUE, --arg=NAME=VALUE

Specifies the compile-time parameter ``NAME`` for building of module
in :option:`soclib-cc -1` mode.

.. option:: -c

Tells soclib-cc to only compile, and not link. This is only available
for module building (:option:`soclib-cc -1` mode).

.. option:: -x

Cleans the build, i.e. deletes the products (object and executable
files) that would have been produced by the build. This option must be
used together with :option:`soclib-cc -1` or :option:`soclib-cc -p`.

.. option:: --embedded-cflags

Only available with :option:`soclib-cc -p`. Outputs the cflags needed
to compile embedded software using the specified platform.

Compilation mode
----------------

.. option:: -m MODE, --mode=MODE

Sets the build mode. This can be ``debug``, ``prof`` or
``release``. This changes the used compilation/linkage flags. Default
is ``release``. Intent is:

``release``:
  Maximal optimization level, no debug symbols.
``debug``:
  All debug symbols included, no optimization.
``prof``:
  Maximal optimization with profiling (gprof) included.

.. option:: -t ENV, --type=ENV

Uses the :ref:`build environment <conf-build_env>` named ENV, defaults
to "default".

Building process pretty-printing
--------------------------------

During the build process, ``soclib-cc`` can output more or less
messages depending on the following parameters:

.. option:: -v, --verbose

Print currently-built module name, together with various information.

.. option:: -d, --debug

Print lots of information, even data specific to the core of
soclib-cc's implementation.

.. option:: -q, --quiet

Try not to emit messages, at all.

.. option:: -P, --progress-bar

Print a progress-bar of the build of a complete platform or
module. This gives user a little visual feedback.


Bug-reporting
-------------

Sometimes, user needs help from other people having more experience
with SoCLib. If this process happens through email, having all the
information peer needs to understand what happened can be a tedious
enumeration. These options try to automate this information gathering
process.

.. option:: --bug-report

Generates a log of the environment, command line, executed processes,
errors.

.. option:: --auto-bug-report=openbrowser

Automatically open a browser to the relevant soclib.fr's website
address where to report bugs.
