
.. _soclib_desc-metadata_providers:

Metadata providers
==================

.. index:: metadata provider modules

Entry point of metadata providers is a class inheriting from
:py:class:`soclib_desc.metadata_file.MetadataFile`.
This class will be instanciated once per found metadata file, and must
return modules when asked for.

.. module:: soclib_desc.metadata_file

.. autoclass:: MetadataFile

   The API to implement to create a new metadata provider is
   "extensions" and "get_modules":

   See :ref:`sd-file` for an example of such class.

   .. attribute:: extensions

      A class attribute containing a list of handled
      extensions.

   .. method:: get_modules()

      This parses and retrives all modules defined in the file.

      :returns: a list of modules defined in the handled file.

   Here is complete API for this class, for reference. These methods
   are already implemented, no need to define them in metadata
   providers.

   .. method:: __init__(path)

      Create a new parser for the given file path.

      :param path: path to a file

   .. attribute:: path

      The full path of file.

   Handling of contained data:

   .. automethod:: doForModules(callback)

   Information about pertinence of data:

   .. automethod:: isOutdated()

   Loading of data:

   .. automethod:: rehashIfNecessary()

   .. automethod:: rehash()

   .. automethod:: cleanup()


   Here are the class methods of
   :py:class:`soclib_desc.metadata_file.MetadataFile`. They are able
   to find the correct subclass of
   :py:class:`soclib_desc.metadata_file.MetadataFile` able to handle a
   given file.

   .. automethod:: init_parsers(parsers)

   .. automethod:: handle(filename)

   .. automethod:: filename_regexp()
