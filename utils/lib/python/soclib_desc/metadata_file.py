
import os
import re
import time

class MetadataFile:
    _known_parsers = set()
    
    def init_parsers(cls, parsers):
        """
        Gets all the parser classes and append them to medata
        providers.

        :param parsers: a list of strings containing names of Python
                        subclasses of this one.
        """
        for mn in parsers:
            tokens = mn.split('.')
            mod = '.'.join(tokens[:-1])
            tmp = __import__(mod, globals(), {}, [tokens[-1]])
            p = getattr(tmp, tokens[-1])
            cls._known_parsers.add(p)
    init_parsers = classmethod(init_parsers)

    def handle(cls, filename):
        """
        Try to find the metadata provider matching the given filename,
        if found, initialize it. If not found, raise a ``ValueError``
        exception.

        :param filename: file to handle
        :returns: a :py:class:`soclib_desc.metadata_file.MetadataFile`
                  subclass object
        :raises: ValueError if file is unhandled
        """
        assert cls._known_parsers, RuntimeError("No known parser")
        for p in cls._known_parsers:
            for ext in p.extensions:
                if filename.endswith(ext):
                    return p(filename)
        raise ValueError('Unhandled file "%s"' % filename)
    handle = classmethod(handle)

    def filename_regexp(cls):
        """
        Retrieves a regexp matching filenames of all currently handled
        files.

        :returns: a Python ``re`` object
        """
        assert cls._known_parsers, RuntimeError("No known parser")
        gre = []
        for p in cls._known_parsers:
            for ext in p.extensions:
                gre.append(re.escape(ext))
        gre = '|'.join(gre)
        return re.compile('^[^.][a-zA-Z0-9_-]+('+gre+')$')
    filename_regexp = classmethod(filename_regexp)



    def __file_time(self):
        try:
            return os.path.getmtime(self.__path)
        except OSError:
            # If file does not exists, make its time somewhere in the future
            return time.time() + 1024




    def __init__(self, path):
        self.__path = path
        self.__date_loaded = 0
        self.__modules = []

    def path(self):
        return self.__path
    path = property(path)

    def doForModules(self, callback):
        """
        Calls the given callback for each module
        """
        for m in self.__modules:
            callback(m)

    def cleanup(self):
        """
        Call :py:func:`~soclib_desc.module.ModuleCommon.cleanup` for each
        module in this file.
        """
        for m in self.__modules:
            m.cleanup()

    def isOutdated(self):
        """
        Returns whether the in-memory cache of this file is
        outdated. This is based of file modification time.

        :returns: Whether memory cache is stale
        """
        return self.__date_loaded < self.__file_time()

    def exists(self):
        """
        Tells whether the metadata file still exists in FS.

        :returns: Whether the file exists
        """
        return os.path.isfile(self.__path)

    def __reloaded(self):
        """
        Tells the module it was just loaded. Must be called when the
        file is parsed.
        """
        self.__date_loaded = self.__file_time()

    def rehashIfNecessary(self):
        """
        This may rehash the cached file, if necessary (modification).
        """
        if self.isOutdated():
            self.rehash()

    def rehash(self):
        """
        Unconditionnaly reloads a description.
        """
        try:
            self.__modules = self.get_modules()
        except NotImplementedError, e:
            raise
#        except Exception, e:
#            import description_files
#            raise description_files.FileParsingError(
#                'in %s: %r'%(self.path, e))
        if not isinstance(self.__modules, (list, tuple)):
            raise ValueError("Parsing of %s by %s did not return a valid result"%
                             (self.path, self))
        self.__reloaded()

    def get_modules(self):
        """
        Parses the metadata file and returns the modules found in it.

        :returns: a list of modules defined in the handled file.
        """
        raise NotImplementedError()        
