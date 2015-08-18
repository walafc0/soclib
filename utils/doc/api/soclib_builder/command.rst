.. -*- rst -*-

.. module:: soclib_builder.command

========
Commands
========


.. class:: Command

   A build step for an :py:class:`~soclib_builder.action.Action`. This
   step may be performed by direct Python code or an external command
   execution.

   .. method:: __init__(self, cmd, cwd = None, on_done = None)

      Creates a new command, with an optional callback on completion.

      :param cmd: Command to run. This is a list of strings used as
                  ``argv``, containing ``argv[0]``.

      :param cwd: Current working directory for the command, if
                  relevant. If left to the ``None`` default value, the
                  directory in which the command is run is not
                  predictible.

      :param on_done: A callable called on completion of the command.
                      This will be called with the following
                      arguments:

                      ``on_done(command, return_code, stdout, stderr)``

   .. attribute:: command

      A pretty-printed string corresponding to the command.

   .. method:: run(synchronous = False)

      Runs the command.

      :param synchronous: Whether to wait for the completion of the
                          command to return from the method.

   .. classmethod:: pending_action_count()

      :returns: the count of currently-running background commands

   .. classmethod:: wait()

      Waits for at least one background command to complete

   .. method:: is_background()

      :returns: whether this command is performed in background

.. class:: CreateFile(soclib_builder.command.Command)

   A simple command able to create a file

   .. method:: __init__(filename, contents, on_done = None)

      :param filename: File to create
      :param contents: Future contents of the file
      :param on_done: Callable to call back on completion

.. class:: CreateDir(soclib_builder.command.Command)

   A simple command able to create a directory

   .. method:: __init__(filename, on_done = None)

      :param filename: Directory name to create
      :param on_done: Callable to call back on completion
