
from soclib_desc.metadata_file import MetadataFile

class CreatorProxy:
    """
    A proxy class which will get the calls to Module, PortDecl,
    ... when parsing a .sd file. This proxy will notify the
    callback the declaration happened.

    This is mostly a decorator, it could be beautifuled to handle
    docstrings et al correctly, but as it should not be inspected
    interactively, this should not be an issue.
    """
    def __init__(self, callback, klas):
        """
        callback will be notified of __call__s, calls will be
        proxied to klas.
        """
        self.__callback = callback
        self.__klas = klas
    def __call__(self, *args, **kwargs):
        """
        Do the hard work.
        """
        r = self.__klas(*args, **kwargs)
        self.__callback(r)
        return r

class SdFile(MetadataFile):

    extensions = ['.sd']

    def __mkParserGlobals(self, module_list):
        """
        Creates the globals dictionary for exec()ing a .sd. This
        creates all the necessary CreatorProxies.
        """
        from sd_parser import module
        from soclib_desc import parameter
        glbl = {}

        for n in module.__all__:
            widget = getattr(module, n)
            try:
                wrap = issubclass(widget, module.Module)
            except:
                wrap = False
            if wrap:
                widget = CreatorProxy(module_list.append, widget)
            glbl[n] = widget
        glbl['Uses'] = module.Uses
        glbl['parameter'] = parameter
        glbl['__name__'] = self.path
        return glbl

    def get_modules(self):
        modules = []
        glbl = self.__mkParserGlobals(modules)
        locl = {}
        exec file(self.path) in glbl, locl
        return modules
