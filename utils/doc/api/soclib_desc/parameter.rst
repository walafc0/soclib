
.. _soclib_desc-parameter:

==========
Parameters
==========

.. index::
   pair: parameters; API

Purpose
=======

Parameters can be used for 2 different purposes:

* compile-time specialization. In soclib-cc, they are called
  ``template`` parameters.

* run-time parameters. In soclib-cc, they are called ``instance``
  parameters.

Most HDL languages only support numeric constant as parameters.

Language support
================

+---------------+------------------------+---------------------+
| Language      | Template parameter     |  Instance parameter |
+===============+========================+=====================+
| VHDL          | Unsupported by         |                     |
|               | language.              |                     |
|               |                        |                     |
|               | Sometimes, you'll find |  `generic map`      |
| (only numeric | VHDL preprocessors     |                     |
| parameters    | which generate VHDL    |                     |
| supported)    | files, preprocessor    |                     |
|               | parameters can be seen |                     |
|               | as ``tempalte``        |                     |
|               | parameters.            |                     |
+---------------+------------------------+---------------------+
| SystemC (C++) | Any pod (integers,     |  Any pod or         |
|               | pointers, etc.) or     |  object instance    |
|               | types, classes, etc.   |                     |
+---------------+------------------------+---------------------+

Soclib-cc library does not check for validity of parameters as such,
this is up to the :ref:`soclib_desc-metadata_providers` to ensure
parameters and types are valid for their own handled module types.

Parameter types
===============

Available parameter types:

====================== ====================
Parameter type name    Associated data
====================== ====================
Bool                   A binary value (true/false)
Int                    An integer value
Float                  A floating point value
String                 A character string (file path, etc.)
StringArray            An array of character strings, of arbitrary length
IntArray               An array of integer values, of arbitrary length
IntTab                 A table of integers, this is used by most soclib components for routing configuration
Type   (C++-specific)  A data type, only usable as template parameter
Module (C++-specific)  A module type or instance
====================== ====================
