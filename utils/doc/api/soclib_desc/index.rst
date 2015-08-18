.. -*- rst -*-

.. module:: soclib_desc

======================================
The soclib metadata description module
======================================

.. index::
   pair: API; metadata

Overview
========

The soclib-cc internal representation of metadata is abstract to their
actual format and parsing. The module descriptions are pluggable and
can be external to soclib core library.

In order to inject module metadata in soclib-cc,
:ref:`md-providers` have to provide a Python module which
define a limited set of entry points.

These providers all return :py:class:`Module objects
<soclib_desc.module.ModuleInterface>`, which contain abstract
representation of metadata of a component. Objects must abide a
limited set of methods described in :ref:`soclib_desc-proto`.

When creating modules, parameters are sometimes needed. They are
described in :ref:`soclib_desc-parameter`.

Topics
======

.. toctree::
   :maxdepth: 2

   protocols
   parameter
   providers
