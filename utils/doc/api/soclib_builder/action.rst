.. -*- rst -*-

=======
Actions
=======

.. module:: soclib_builder.action

.. class:: Action

   An action. This is a step in a build process, corresponding to a
   set of steps to perform to produce results. An action can be
   performed directly by Python code, or by running
   :py:class:`soclib_builder.command.Command` objects.

   * Initialization

     .. method:: __init__(dests, sources, **options)
  
        Creates a new :py:class:`~soclib_builder.action.Action`
        object. It must declare its sources and destinations, and can
        contain an arbitrary list of optional parameters.
  
        :param dests: A list of paths strings or
                      :py:class:`soclib_builder.bblock.BBlock` objects
                      considered as products of this action.
  
        :param sources: A list of paths strings or
                        :py:class:`soclib_builder.bblock.BBlock` objects
                        considered as inputs of this action.
  
        :param options: An arbitrary set of ``key = value``
                        specifications.
  
   * Working with the action

     .. method:: prepare()
  
        Prepare this action:
  
        * Create directories needed to host produced files
  
        * Rehash the state of the action: whether it is already done or
          yet to do.
  
     .. method:: is_valid()
  
       :returns: whether all the output files exist, and are newer than
                 any of the inputs
  
     .. method:: add_depends(*users)
  
        Add a given list of :py:class:`soclib_builder.bblock.BBlock`
        objects as users of this action. This is not necessarily a list
        of input objects, but can be bblocks needing completion of this
        action some time before their creation.
  
        :param users: a list of :py:class:`soclib_builder.bblock.BBlock`
  
   * Managing build steps

     Build steps must be added in order in the action. This is up to
     :py:class:`~soclib_builder.action.Action` inheriting class to
     populate the build steps with the following methods.

     .. method:: run_command(cmd, cwd = None)
  
        Add a command to the list of commands associated to this action.
  
        :param cmd: a :py:class:`soclib_builder.command.Command` object
        :param cwd: current-working-directory where the command must be
                    run, if relevant
  
     .. method:: create_file(bblock, contents)
  
        Add a file to create in the build steps.

        :param bblock: A bblock corresponding to a file

        :param contents: A string containing the future contents of
                         the file

   * Running the action

     .. method:: process()

        Synchronously run all the steps involved in the action.

     .. method:: source_changed()

        Notifies this action that at least one source file changed,
        and the action must recompute its completion state.

   * API used by the :py:class:`soclib_builder.todo.ToDo`

     .. method:: todo_launch(synchronous = False)

        Run all the associated steps of the action

        :param synchronous: Whether to run the steps in background

     .. method:: todo_state()

        :returns: one of `Action.BLOCKED`, `Action.DONE` or
                  `Action.TODO` corresponding to the current
                  action's state.

     .. method:: todo_can_be_processed()

        :returns: whether the action is ready to be performed

     .. method:: why_blocked()

        Prints on ``sys.stdout`` why this action is blocked.

     .. method:: todo_get_depends()

        Retrieve all the :py:class:`~soclib_builder.bblock.BBlock`
        objects that are prerequisites of this action.

     .. method:: todo_clean()

        Delete all products

   * Pretty printing helpers

     These methods can be reimplemented by subclassers in order to
     provide uniform output format among tools

     .. method:: stdout_reformat(msg)

        Reformat the stdout of tools

        :param msg: String containing a line of a stdout message
        :returns: A string reformated containing the same data

     .. method:: stderr_reformat(msg)

        Reformat the stderr of tools

        :param msg: String containing a line of a stderr message
        :returns: A string reformated containing the same data

.. module:: soclib_builder.textfile
       
.. class:: TextFile(soclib_builder.action.Action)

   A simple action that creates a file.

   If file already exists, it will be overwritten only if its contents
   differs from the desired contents.

   .. method:: __init__(output, contents)

      Creates a new textfile creation action that will contain the
      given contents.

      :param output: filename of the created file

      :param contents: future contents of the file

.. class:: CxxSource(soclib_builder.textfile.TextFile)

   All methods are inherited from
   :py:class:`soclib_builder.textfile.TextFile`.
