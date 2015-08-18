
.. _build:

=================
The build process
=================

.. index::
   pair: build; process

The C++ template compilation problem
====================================

The usual way of using templated code is to put all code in .h, having
template code emitted when used in main C++ file. 

This is good for utility libraries (like STL), but SystemC modules may
be more than 1000 lines long, and more that 40 of them may be used in
one topcell. This may yield a single translation unit with more than
50000 lines of code, heavily templated.

This implies some usability issues:

* compiler getting out of memory,
* unreasonable compile times.

Therefore we need two features in the build-system:

* Support for separate implementation and declaration:

  * Put template class definition (.h) and implementation (.cpp) in
    two separate files. Compile the implementation separately.

  * This implies that the C++ templates must be explicitly
    instanciated with some ``template class ns::Foo<parameters>;``
    code.

    As we can't predict all the template parameters that will be
    used for all modules, template instanciation has to be done
    automatically.

* Object reuse: Once modules are built separately, we can put objects
  in a global repository and use the in a cached way. This can shorted
  compile times across different topcells using the same modules with
  the same parameters.


.. _tmp-spool:

Temporary objects reuse
=======================

.. index::
   pair: build; temporary objects

When modules are compiled, object files resulting from their
compilation are put in a separate temporary `repos` directory where
file names are indexed depending on:

* compilation mode (`release`, `debug`, `prof`),

* module,

* source file,

* template parameters.

This way, when the same module is used in the same context, its object
files may be already present, and reused without recompilation of big
C++ code.

.. figure:: /_static/soclib-cc-flow.*


.. _build-debug_mode:

Per-module debug mode
=====================

.. index::
   pair: debug; module

User sometimes needs to debug just one module. Soclib-cc includes
support for such an usage with its `debug mode`. This permits to have
just one module compiled with no optimization and debug symbols, with
the rest of simulator running optimized code.

This can be either done with:

* the :option:`soclib-cc -b` on command line,

* adding ``debug = True`` in :py:func:`Module` metadata statement.

When `debug mode` is activated, module is compiled with debug flags
turned on, and also has the preprocessor macro
:c:macro:`SOCLIB_MODULE_DEBUG` defined.
