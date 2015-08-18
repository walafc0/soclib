.. -*- rst-mode -*-

=========================
The soclib Python modules
=========================

.. index:: API

Soclib comes with a Python library. This library is split in three
parts:

* A metadata indexer and cache in :py:mod:`soclib_desc`.

* A :py:mod:`builder <soclib_builder>`, responsible for compiling platforms.

* A configuration module, used by the metadata module, the builder and
  :ref:`soclib-cc`.

* A glue code module, used to assemble metadata and builder:
  :py:mod:`soclib_cc`.

* A built-in ``.sd`` metadata file parser plugin in :py:mod:`sd_parser`.

.. figure:: /_static/soclib-cc.*
   :width: 18cm  

.. toctree::
   :maxdepth: 1

   soclib_desc/index
   soclib_builder/index
   soclib_cc/index

