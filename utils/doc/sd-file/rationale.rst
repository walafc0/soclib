
Rationale
=========

.. index:: .sd file

Design
------

Module metadata format is, like most other formats, subject to a
debate about the "right" representation. At the time SoCLib started to
grow, we needed:

* something not subject to external erratic changes in specification,

* a simple yet powerful format:

  * easy to parse,
  * easy to learn,
  * easy to manually edit.

``.sd`` files were created to satisfy these goals. They are:

* Easy to parse, the syntax is a subset of the Python syntax,
  therefore, python interpreter can parse the files directly.

* Easy to read, this is mostly a key-value system.

* Easy to learn, no overhead in syntax.

* Easy to edit, a complete module fits less than 25 lines, you can see
  it all while editing, and understand what you do.

* SystemC source-code oriented.  There is limited support for
  VHDL/Verilog source-code.

.. _sd-limitations:

Limitations
-----------

* The ``.sd`` metadata format cannot express all of the
  :py:class:`~soclib_desc.module.ModuleInterface` and
  :py:class:`~soclib_desc.specialization.SpecializationInterface`
  protocols. Only a subset is supported.
  
  For instance, ``.sd`` files can't contain declarations of a module
  where dependent submodules list changes depending on a parameter
  (``X`` is dependent on ``Y`` only if parameter ``z > 21``).  This is
  a feature for sake of simplicity. Other metadata providers could
  implement such a feature.

* This is a custom file format, nobody else uses it.
