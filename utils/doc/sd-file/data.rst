
Provided data
=============

.. _sd-syntax:

Syntax
------

.. index::
   pair: .sd file; syntax

Fundamental datatype are:

Strings:
  They can be enclosed in simple or double quotes.
Lists:
  They are enclosed in brackets (``[]``).
Dictionaries:
  They are a mapping from a string to something else. Syntax is ``{
  "key": value, "key2": value2, ... }``.


Files contain declarations of different modules. Basic module types
are:

``Module``:
  A new module, most of the time an entity.
``PortDecl``:
  A new port type definition. This is actually a superset of a module
``Signal``:
  A new signal type definition. This is actually a superset of a
  module

These declarations can use fundamental datatypes and statements, among
them:

``Port``:
  Usage of a port, see :ref:`sd-ports`
``Uses``:
  Usage of another module, see :ref:`sd-uses`.
``parameter``:
  Usage of a parameter, see :ref:`sd-file-parameters`.

Root statements
---------------

.. index::
   triple: module; definition; metadata
   triple: port; definition; metadata
   triple: signal; definition; metadata

``Module``, ``PortDecl`` and ``Signal`` take the declared module name
as first argument. All other arguments are named and specify attached
metadata.

.. function:: Module(name, classname, tmpl_parameters = [], header_files = [], implementation_files = [], ports = [], instance_parameters = [])

   Defines a new module. Module can be either actual entities, or
   simple helper modules, like pure C++ classes.

   :param name: Name of module in :ref:`md-index`

   :param classname: Entity name (class with namespace for C++, entity
                     name for VHDL)

   :param tmpl_parameters: Compile-time parameters, a list of
                           :ref:`parameters <sd-file-parameters>`.

   :param instance_parameters: Run-time parameters, a list of
                               :ref:`parameters <sd-file-parameters>`.

   :param implementation_files: Source files to compile

   :param header_files: Source files to include in parent modules

   :param ports: A list of :ref:`sd-ports`.


Example declaring a component named "caba:my_module" in the
:ref:`md-index`, implemented as the ``MyModule`` class, with code in
the ``my_module.h`` and ``my_module.cc`` files::

  Module("caba:my_module",
         classname = "MyModule",
         header_files = ["my_module.h"],
         implementation_files = ["my_module.cc"],
     )


.. function:: Signal(name, classname, tmpl_parameters = [], header_files = [], implementation_files = [], accepts = {})

   Defines a new signal type

   :param name: Like in :py:func:`Module`
   :param classname: Like in :py:func:`Module`
   :param tmpl_parameters: Like in :py:func:`Module`
   :param header_files: Like in :py:func:`Module`
   :param implementation_files: Like in :py:func:`Module`
   :param accepts: A dictionnary of port names associated to maximum
                   connection count. If there is no limit, put ``True``.

.. function:: PortDecl(name, classname, tmpl_parameters = [], header_files = [], implementation_files = [], signal = None)

   Defines a new port type.

   :param name: Like in :py:func:`Module`
   :param classname: Like in :py:func:`Module`
   :param tmpl_parameters: Like in :py:func:`Module`
   :param header_files: Like in :py:func:`Module`
   :param implementation_files: Like in :py:func:`Module`
   :param signal: The name in :ref:`md-index` of a correponding signal

Example defining the bit type::

  # A bit signal only supports one driver, but any number of bit input
  # ports may be connected on it.
 
  # No specific header files are necessary for these declarations as
  # they use builtin SystemC types.

  Signal('caba:bit',
         classname = 'sc_core::sc_signal<bool>',
         accepts = {'caba:bit_in'  : True,
                    'caba:bit_out' : 1,
                   },
         )

  PortDecl('caba:bit_in',
           signal = 'caba:bit',
           classname = 'sc_core::sc_in<bool>',
           )
  
  PortDecl('caba:bit_out',
           signal = 'caba:bit',
           classname = 'sc_core::sc_out<bool>',
           )


.. _sd-uses:

Uses
----

If the module uses another module internally, we may want to declare
this also. ``Uses()`` statement serves this goal.

.. function:: Uses(module_name, **parameters)

   :param module_name: Module name in :ref:`md-index`

   :param parameters: A key/value mapping of parameters

Example with "caba:my_super_module" using "caba:my_module"::

  Module("caba:my_super_module",
         classname = "MySuperModule",
         header_files = ["my_super_module.h"],
         implementation_files = ["my_super_module.cc"],
         uses = [
            Uses("caba:my_module"),
            ],
     )

.. _sd-ports:

Ports
-----

``Port`` statements declare a port with a given type and name.

.. function:: Port(type, name, array_size = None, **parameters)

   :param type: Module name in :ref:`md-index`

   :param name: Name of the port.

   :param array_size: A numeric value. Only relevant if this is
                      actually an array of ports.

   :param parameters: A key/value mapping of needed arguments of port
                      module.

Supported ``types`` are either:

* Built-in port types, depending on language:

  * SystemC types: ``caba:bit_in``, ``caba:bit_out``,
    ``caba:clock_in``, ``caba:clock_out``, ``caba:word_in``,
    ``caba:word_out``.

  * HDL types: ``rtl:bit_in``, ``rtl:bit_out``, ``rtl:word_in``,
    ``rtl:word_out``.

  Example::

    Port("rtl:bit_in", "p_in0")

  Word-based ports need a ``W`` parameter to set their width::

    Port("rtl:word_out", "p_result", W = 5)

* If a port is connected to a specific net, this can be hinted through
  the ``auto`` keyword::

    Port('caba:bit_in', 'p_resetn', auto = 'resetn')

* Other composite port types declared with a :py:func:`PortDecl` statement.

Usage in a module with one output IRQ port, and 10 input IRQ ports::

  Module("caba:my_super_module",
         classname = "MySuperModule",
         ports = [
            Port('caba:bit_out','p_irq'),
            Port('caba:bit_in','p_irq_in', 10),
            ],
     )
