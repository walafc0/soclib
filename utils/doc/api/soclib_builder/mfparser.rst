.. -*- rst -*-

====================
Makefile rule parser
====================

.. module:: soclib_builder.mfparser

Some dependency trackers can generate Makefile portions to create a
rule dependency list, like `GCC` does. In order to be able to import
the generated ``Makefile`` stanza in ``soclib_builder``, here is a
parser.

.. class:: MfRule

   A Makefile rule parser.

   .. method:: __init__(text)

      Create the new parser with the given ``Makefile`` stanza. 

      :param text: a ``Makefile`` stanza

      Stanza is of the form::

        dest0 dest1 destn : prerequisite0 prerequisite1 prerequisiten

      Additionnaly, this parser supports correct handling of line
      continuation with backslashes, and escaping of colon ``:`` in
      file names.

   .. attribute:: dests

      A list of filenames which are destinations of the rule

   .. attribute:: prerequisites

      A list of filenames which are the prerequisites of the rule.
