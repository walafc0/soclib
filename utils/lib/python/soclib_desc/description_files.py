
import os
import sys
import cache
import warnings
import atexit
from soclib_cc import exceptions

class NoSuchComponent(exceptions.ExpectedException):
	def __str__(self):
		return 'NoSuchComponent "%s"'%self.args[0]

class FileParsingError(exceptions.ExpectedException):
	pass

class DoubleRegistrationWarning(Warning):
	def __str__(self):
		return 'Module %s registered twice: in "%s" and "%s"'%(self.args[0], self.args[1], self.args[2])

class DescCache:
	def __init__(self, cache_file = None, ignore_regexp = None, parsers = []):
		"""
		Create a new DescCache
		"""
		self.__cache_file = cache_file
		self.__ignore_regexp = ignore_regexp
		self.__global_cache = cache.GlobalDescCache(self.__ignore_regexp, parsers)
		self.__enabled_paths = []
		self.__modules = {}
		self.__fresh = False
		self.reload()

	def reload(self):
		"""
		Loads the cache from cache path. If no cache path is set in
		config, no cache is loaded. Dates of files in cache are
		compared to corresponding timestamps, and individual files are
		reloaded if necessary.
		"""

		newcache = cache.GlobalDescCache.load(self.__cache_file)
		if newcache is None:
			self.__fresh = True
			return
		self.__global_cache = newcache
		self.__global_cache.checkFiles()
		self.__fresh = False
		
	def save(self):
		"""
		Saves back the cache. If no cache path is present in the
		configuration, this call is a noop.
		"""
		if not self.__fresh:
			return
		try:
			self.__global_cache.save(self.__cache_file)
#			print >> sys.stderr, "Saved cache"
		except:
			pass
		
	def delete(self):
		"""
		Trash the cache.
		"""
		try:
			os.unlink(self.__cache_file)
#			print >> sys.stderr, "Deleted cache"
		except:
			pass
		
	def addPath(self, path, cacheable = True):
		"""
		Add a path, and get its definitions from cache by default.
		"""
		self.__enabled_paths.append(path)
		if cacheable:
			self.__global_cache.visitSubtree(path)
		else:
			self.__global_cache.parseSubtree(path)

		def __add(module):
			if module.name in self.__modules and \
				   hash(self.__modules[module.name]) != hash(module):
				warnings.warn(DoubleRegistrationWarning(module.name, self.__modules[module.name], module), stacklevel = 3)
			self.__modules[module.name] = module
			
		for cdf in self.__global_cache.getCachedDescFilesIn(path):
			cdf.doForModules(__add)

	def removePath(self, path):
		"""
		Disable definitions from path
		"""
		def __remove(module):
			if self.__modules[module.name] == module:
				self.__modules[module.name]
		for cdf in self.__global_cache.getCachedDescFilesIn(path):
			cdf.doForModules(__remove)
		self.__enabled_paths.remove(path)

	def __getitem__(self, key):
		"""
		Get a module from its fqmn
		"""
		if key not in self.__modules:
			self.rehashCaches()
		if key in self.__modules:
			return self.__modules[key]
		else:
			raise NoSuchComponent(key)

	def all(self):
		return self.__modules.values()

	def rehashCaches(self):
		"""
		Rehashes cache for all currently-enabled paths.
		"""
		if self.__fresh:
			return
		self.__fresh = True

		paths = self.__enabled_paths[:]
		for path in paths:
			self.removePath(path)
			
		for path in paths:
			self.__global_cache.parseSubtree(path)

		for path in paths:
			self.addPath(path)

class _on_exit_save_cache:
	def __init__(self):
		self.__value = True
	def failed(self):
		self.__value = False
	def __call__(self):
		global soclib_desc_registry
		try:
			soclib_desc_registry
		except:
			return
		if self.__value:
			soclib_desc_registry.save()
		else:
			soclib_desc_registry.delete()

class _clear_cache_on_exception:
	def __init__(self, prev, onex):
		self.__prev = prev
		self.__onex = onex
	def __call__(self, typ, value, traceback_):
		global soclib_desc_registry
		self.__onex.failed()
		try:
			soclib_desc_registry.delete()
		except NameError:
			pass
		self.__prev(typ, value, traceback_)

def init():
	"""
	Initialize description_files cache, register some hooks to delete
	the cache if things fail.
	"""
	global soclib_desc_registry_initialized
	try:
		soclib_desc_registry_initialized
		return
	except:
		soclib_desc_registry_initialized = True
	
	onex = _on_exit_save_cache()
	atexit.register(onex)
	sys.excepthook = _clear_cache_on_exception(sys.excepthook, onex)

	global soclib_desc_registry

	from soclib_cc.config import config
	soclib_desc_registry = DescCache(config.cache_file, config.sd_ignore_regexp, config.desc_parsers)
	import os
	import os.path
	soclib_desc_registry.addPath(
		os.path.join(os.path.dirname(__file__),
					 '../../communication_base'))
	for path in config.desc_paths:
		soclib_desc_registry.addPath(path)

def cleanup():
	init()
	global soclib_desc_registry
	try:
		soclib_desc_registry.delete()
	except:
		pass

def add_path(path, cacheable = True):
	init()
	global soclib_desc_registry

	soclib_desc_registry.addPath(path, cacheable)

def get_module(name):
	init()
	global soclib_desc_registry

	return soclib_desc_registry[name]

def get_all_modules():
	init()
	global soclib_desc_registry

	return soclib_desc_registry.all()

def get_all_used_modules():
	init()
	global soclib_desc_registry

	return filter(lambda x:x.is_used(),
				  soclib_desc_registry.all())
