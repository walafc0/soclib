.. -*- rst -*-

.. module:: soclib_builder.bblock

==============
Building block
==============

.. class:: BBlock

   A build block. This is either a input or a product of a
   :py:class:`soclib_builder.action.Action`. They may be associated to
   a file or not, depending on
   :py:class:`~soclib_builder.action.Action` needs.

   Build blocks associated to files are indexed in a global registry,
   and are merged when used from different
   :py:class:`~soclib_builder.action.Action` objects. A BBlock can tell
   what actions are considered user or producer of itself.

   Build blocks which are products have to be passed to
   :py:class:`soclib_builder.todo.ToDo`.

   .. method:: __init__(filename, generator = None)

      Creates a new bblock, associated to a filename, and a generator.

      Generator is a :py:class:`soclib_builder.action.Action`
      responsible for creating the file on demand.

      :param filename: a file path
      :param generator: a :py:class:`soclib_builder.action.Action`
                        object

   .. method:: touch()

      Marks the associated file as changed.

   .. attribute:: users

      List of actions which are dependant from this bblock

   .. method:: addUser(user)

      Add a given :py:class:`soclib_builder.action.Action` object as
      user.

      :param user: a :py:class:`soclib_builder.action.Action`

   .. method:: is_dir()

      :returns: whether the associated path is a directory.

   .. method:: mtime()

      :returns: the modification time of associated file

   .. method:: delete()

      Delete the underlying file in the filesystem.

   .. method:: generate()

      Calls the corresponding
      :py:class:`~soclib_builder.action.Action` for building this
      bblock.

   .. method:: prepare()

      Prepare the dependency list of the bblock. Must be called
      between the registration of users and the evaluation of
      dependencies.

   .. method:: needs(other)

      Tells whether this bblock needs another one as dependency.

      :param other: another :py:class:`soclib_builder.bblock.BBlock`
                    object
      :returns: True or False

.. class:: AnonymousBBlock(BBlock)

   A bblock that is anonymous.

   .. method:: __init__(generator)

      Creates a new anonymous bblock associated to a generator. An
      anonymous bblock with no generator is irrelevant.

      :param generator: a :py:class:`soclib_builder.action.Action`
                        object.
