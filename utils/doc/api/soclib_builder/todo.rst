.. -*- rst -*-

.. module:: soclib_builder.todo

=================
Builder todo list
=================


.. class:: ToDo

   A to-do list. From a list of
   :py:class:`soclib_builder.bblock.BBlock` objects, retrieve all
   involved :py:class:`soclib_builder.action.Action` objects, and
   permit to:

   * run them, possibly in parallel, with a limit of parallel
     processes,

   * clean products.

   :py:class:`~soclib_builder.action.Action` objects are automatically
   retrieved from their associated
   :py:class:`soclib_builder.bblock.BBlock` objects.

   .. method:: __init__(*dests)

      Creates a new to-do list with an arbitrary list of build blocks
      to do.

      :param dests: arbitrary list of
                    :py:class:`~soclib_builder.bblock.BBlock` that are
                    the products to do

   .. method:: add(*dests)

      Adds an arbitraty list of build blocks to the todo list.

      :param dests: arbitrary list of
                    :py:class:`~soclib_builder.bblock.BBlock` that are
                    the products to do

   .. method:: prepare()

      Prepare the list of actions to perform. Compute the
      dependencies, order the building blocks by dependencies, order
      the actions.

   .. method:: clean()

      Delete all products of all actions.

   .. method:: process()

      Build all dests associated to this to-do list. This method
      either returns with all products done and no error, or raises an
      exception of a failed action.

      :returns: None
      :raises: Build failure
