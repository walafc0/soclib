
# SOCLIB_GPL_HEADER_BEGIN
# 
# This file is part of SoCLib, GNU GPLv2.
# 
# SoCLib is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# SoCLib is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SoCLib; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
# 
# SOCLIB_GPL_HEADER_END
# 
# Copyright (c) UPMC, Lip6, SoC
#         Nicolas Pouillon <nipo@ssji.net>, 2009
# 
# Maintainers: group:toolmakers

import sys
import os
from datetime import datetime
import atexit
import traceback

__id__ = "$Id: bugreport.py 1063 2009-05-05 15:27:25Z nipo $"
__version__ = "$Revision: 1063 $"

__all__ = ['bootstrap']

class Outter:
	def __init__(self, fd, term, prefix):
		self.__fd = fd
		self.__term = term
		self.__prefix = prefix
		self.__class__.__last = self
	def write(self, s):
		if self.__class__.__last != self:
			self.__class__.__last = self
			self.__fd.write("\n==== %s ====\n"%self.__prefix)
		self.__fd.write(s)
		self.__term.write(s)
	def flush(self):
		self.__fd.flush()
		self.__term.flush()
	def isatty(self):
		return False

class ExceptHandler:
	def __init__(self):
		self.had = False
	def __call__(self, typ, value, traceback_):
		traceback.print_exception(typ, value, traceback_)
		from soclib_cc import exceptions
		print 
		if issubclass(typ, exceptions.ExpectedException):
			print "SoCLib-cc failed expectedly."
		else:
			print "SoCLib-cc failed unexpectedly."
		self.had = True

class AdvertizeForBugreporter:
	def __init__(self, prev):
		self.__prev = prev
	def __call__(self, typ, value, traceback_):
		self.__prev(typ, value, traceback_)
		from soclib_cc import exceptions
		print
		if issubclass(typ, exceptions.ExpectedException):
			print "SoCLib-cc failed expectedly. If you think this is not the case,"
		else:
			print "SoCLib-cc failed unexpectedly. To submit a bug report,"
		print " please re-run soclib-cc with --bug-report"
		print " (i.e run: 'soclib-cc %s --bug-report')"%' '.join(sys.argv[1:])

def end_handler(logname, exc, fd, report_action):
	from soclib_desc.description_files import get_all_used_modules
	from soclib_utils.repos_file import revision
	from soclib_cc.config import config
	import os
	import os.path
	print >> fd
	print >> fd, "Config:"
	print >> fd, str(config)
	print >> fd, "Used modules:"
	for m in get_all_used_modules():
		print >> fd, '  ', m
		for f in m.files():
			print >> fd, '    ', os.path.basename(f), revision(f)
	print >> fd
	url = 'https://www.soclib.fr/soclib-cc-bugreport'
	print "Now, to report the bug:"
	print " * open %s"%url,
	if report_action == "openbrowser":
		print '(should open automagically)'
		try:
			import webbrowser
			webbrowser.open(url)
		except:
			pass
	else:
		print
	print " * submit a bug report attaching '%s'"%logname

def bootstrap(create_log, report_action):
	if create_log:
#		filename = datetime.now().strftime("soclib-cc-%Y%m%d-%H%M%S.log")
		filename = "soclib-cc-debug.log"
		fd = open(filename, "w")
		sys.stdout = Outter(fd, sys.stdout, "stdout")
		sys.stderr = Outter(fd, sys.stderr, "stderr")
		print "Soclib-cc is in bug-report mode. Every output will go to '%s'"%filename

		sys.excepthook = ExceptHandler()
		atexit.register(end_handler, filename, sys.excepthook, fd, report_action)

		print >> fd, "Command: %s"%sys.argv
		print >> fd, "Environment:"
		for k in sorted(os.environ.iterkeys()):
			print >> fd, "%s: %r"%(k, os.environ[k])
		print >> fd
	else:
		sys.excepthook = AdvertizeForBugreporter(sys.excepthook)

