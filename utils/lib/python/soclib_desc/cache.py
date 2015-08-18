
import os
import os.path
import time
import re
import warnings
import pickle

class GlobalDescCache:
    """
    A cache for all definitions. They are indexed by .sd path.
    """
    def __init__(self, ignore_regexp = None, config_parsers = []):
        self.__ignore_regexp = None
        if ignore_regexp:
            self.__ignore_regexp = re.compile(ignore_regexp)
        self.__registry = {}
        self.__once_seen = set()

        from soclib_desc.metadata_file import MetadataFile
        MetadataFile.init_parsers(config_parsers)
        self._sdfile = MetadataFile.filename_regexp()

    def visitSubtree(self, path):
        """
        Recurse in a subtree looking for .sd files, or assume it is cached
        """
        if path not in self.__once_seen:
            self.parseSubtree(path)
        self.__once_seen.add(path)

    def parseSubtree(self, path):
        """
        Inconditionally recurse in a subtree looking for .sd files.
        """
        path = os.path.abspath(path)
        self.__once_seen.add(path)
        for root, dirs, files in os.walk(path):
            if ".svn" in dirs:
                dirs.remove(".svn")
            files = filter(self._sdfile.match, files)
            if self.__ignore_regexp:
                files = filter(lambda x:not self.__ignore_regexp.match(x), files)
            for f in files:
                self.checkFile(os.path.join(root,f))

    def checkFile(self, path):
        """
        Rehashes a file, add it to cache if non-existent
        """
        from soclib_desc.metadata_file import MetadataFile

        if not path in self.__registry:
            self.__registry[path] = MetadataFile.handle(path)
        self.__registry[path].rehashIfNecessary()

    def checkFiles(self):
        """
        Rehashes all the known files if necessary.
        Remove deleted files.
        """
        todel = []
        for key, c in self.__registry.iteritems():
            if c.exists():
                c.rehashIfNecessary()
            else:
                todel.append(key)
        for d in todel:
            del self.__registry[d]

    def getCachedDescFilesIn(self, path):
        """
        Returns cached descriptions which files are under path
        """
        path = os.path.abspath(path)
        if not path.endswith('/'):
            path += '/'
        r = []
        for p, d in self.__registry.iteritems():
            if p.startswith(path):
                r.append(d)
        return r

    def save(self, path):
        """
        Saves the cache to path.
        """
        fd = open(path, 'w')
        pickle.dump(self, fd, pickle.HIGHEST_PROTOCOL)

    def cleanup(self):
        self.__once_seen = set(self.__once_seen)
        for p, d in self.__registry.iteritems():
            d.cleanup()

    def load(cls, path):
        """
        Loads the cache from path. If unable to load a valid cache,
        this call returns None.
        """
        try:
            fd = open(path, 'r')
            r = pickle.load(fd)
        except:
            return None
        
        if isinstance(r, cls):
            r.cleanup()
            return r
        return None
    load = classmethod(load)
