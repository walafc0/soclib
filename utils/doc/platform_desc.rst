
.. _pf-def:

==========================
Platform description files
==========================

Intent
======

For SystemC, platform are composed of a custom topcell, i.e. a
``sc_main()`` function in a source file. This netlist source has to be
linked with template C++ code of each component, using the right
template parameters for all needed modules.

A platform description file contain such things:

* a SystemC (C++) source file name,

* a list of used modules and their parameters.

This can be used by :ref:`soclib-cc`, with the :option:`soclib-cc -p`
option to build a complete simulator.

Format
======

.. index::
   pair: platform description; syntax

The global syntax is the same as :ref:`.sd file syntax
<sd-syntax>`.

The root statement is an assignation of a ``Platform`` object to the
``todo`` variable.

Here is an example with a self-contained topcell (not using external
modules)::

  todo = Platform('caba', 'top.cpp')

.. function:: Platform(abstraction_level, source, uses = [], **params)

   :param abstraction_level: The abstraction level of the topcell.

   :param source: Source file name, relative to the platform
                  description parent directory.

   :param uses: A list of :ref:`Uses statements <sd-uses>`.

   :param params: Default implicit parameters for all ``Uses``
                  statements.

A complete example::

  todo = Platform('caba', 'top.cpp',
                  uses = [
                      Uses('caba:vci_xcache_wrapper',
                           iss_t = 'common:mips32el'),
                      Uses('caba:vci_ram'),
                      Uses('caba:vci_vgmn'),
                      Uses('common:elf_file_loader'),
                      ],
                  cell_size = 4,
                  plen_size = 6,
                  addr_size = 32,
                  rerror_size = 1,
                  clen_size = 1,
                  rflag_size = 1,
                  srcid_size = 8,
                  pktid_size = 1,
                  trdid_size = 1,
                  wrplen_size = 1
                  )

Semantically equivalent to its verbose counterpart::

  todo = Platform('caba', 'top.cpp',
                  uses = [
                      Uses('caba:vci_xcache_wrapper',
                           iss_t = 'common:mips32el',
                           cell_size = 4,
                           plen_size = 6,
                           addr_size = 32,
                           rerror_size = 1,
                           clen_size = 1,
                           rflag_size = 1,
                           srcid_size = 8,
                           pktid_size = 1,
                           trdid_size = 1,
                           wrplen_size = 1),
                      Uses('caba:vci_ram',
                           cell_size = 4,
                           plen_size = 6,
                           addr_size = 32,
                           rerror_size = 1,
                           clen_size = 1,
                           rflag_size = 1,
                           srcid_size = 8,
                           pktid_size = 1,
                           trdid_size = 1,
                           wrplen_size = 1),
                      Uses('caba:vci_vgmn',
                           cell_size = 4,
                           plen_size = 6,
                           addr_size = 32,
                           rerror_size = 1,
                           clen_size = 1,
                           rflag_size = 1,
                           srcid_size = 8,
                           pktid_size = 1,
                           trdid_size = 1,
                           wrplen_size = 1),
                      Uses('common:elf_file_loader'),
                      ],
                  )
