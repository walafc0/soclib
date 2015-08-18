.. -*- rst -*-

.. module:: soclib_cc.actions

===================================
The soclib-specific builder actions
===================================

.. module:: soclib_cc.actions.hdl

HDL compiler actions
====================

.. class:: HdlCompile(soclib_builder.action.Action)

   An HDL (VHDL, Verilog) compiler
   :py:class:`~soclib_builder.action.Action` baseclass for
   :py:mod:`soclib_builder`.

   This class handles the correct way of calling modelsim's ``vlog``
   tool. You must not directly use this class, but use one of its
   inheriters.

.. class:: VhdlCompile(soclib_cc.actions.hdl.HdlCompile)

   A ``.vhd`` file compiler able to run ``vlog`` command lines.

.. class:: VerilogCompile(soclib_cc.actions.hdl.HdlCompile)

   A ``.v`` file compiler able to run ``vlog`` command lines.


.. module:: soclib_cc.actions.cxx

C++ compiler actions
====================

.. class:: CCompile(soclib_builder.action.Action)

   A GCC compiler :py:class:`~soclib_builder.action.Action` baseclass
   for :py:mod:`soclib_builder`.

   This class handles the correct way to call ``gcc/g++/ld`` for
   compiling or linking C/C++ code.

.. class:: CxxCompile(soclib_cc.actions.hdl.CCompile)

   A C++ file compiler able to run ``g++`` like command lines.

.. class:: CLink(soclib_cc.actions.hdl.CCompile)

   A C object linker able to run ``gcc`` like command lines.

.. class:: CxxLink(soclib_cc.actions.hdl.CLink)

   A C++ object linker able to run ``g++`` like command lines.

.. class:: CMkObj(soclib_cc.actions.hdl.CLink)

   A C object merger able to run ``ld -r`` like command lines.

.. class:: CxxMkObj(soclib_cc.actions.hdl.CLink)

   A C++ object merger able to run ``ld -r`` like command lines.
