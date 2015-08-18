#!/usr/bin/env python

from dsx.util.objdumper import *
import sys

__id__ = "$Id: iss_profiler2profile.py 917 2009-03-12 10:10:06Z nipo $"
__version__ = "$Revision: 917 $"

class SymLooker:
	def __init__(self, arch, obj):
		self.__syms = {}
		dumper = ObjDumper(arch, obj)
		for section in dumper:
			for sym in section:
				self.__syms[sym.addr] = sym.name
		self.__addrs = self.__syms.keys()
		self.__addrs.sort()
		self.__addr2sym = {}
	def is_entry(self, addr):
		return addr in self.__syms
	def lookup_sym(self, addr):
		last_addr = None
		for sym_addr in self.__addrs:
			if sym_addr > addr:
				break
			last_addr = sym_addr
		if last_addr is None:
			print hex(addr), "not found in", self.__addrs
		return self.__syms[last_addr]
	def find_sym(self, addr):
		try:
			return self.__addr2sym[addr]
		except KeyError:
			sym = self.lookup_sym(addr)
			self.__addr2sym[addr] = sym
			return sym
	def per_sym(self, ctor):
		ret = {}
		for k in self.syms():
			ret[k] = ctor(k)
		return ret
	def syms(self):
		return self.__syms.values()

arch = sys.argv[1]
obj = sys.argv[2]

sl = SymLooker(arch, obj)

class Counter:
	def __init__(self, sym):
		self.sym = sym
		self.total = 0
		self.frozen = 0
		self.running = 0
		self.runs = 0
	def inc(self, running, entering):
		if entering:
			self.runs += 1
		if running:
			self.running += 1
		else:
			self.frozen += 1
		self.total += 1
	def cmp_total(self, other):
		return cmp(self.total, other.total)
	def cmp_running(self, other):
		return cmp(self.running, other.running)
	def missing(self):
		if self.total:
			return float(self.frozen)/float(self.total)
		else:
			return 0
	def cmp_missing(self, other):
		return cmp(self.missing(), other.missing())
	def cmp_runs(self, other):
		return cmp(self.runs, other.runs)
	def cpr(self):
		if self.runs:
			return float(self.total)/float(self.runs)
		else:
			return 0
	def cmp_cpr(self, other):
		return cmp(self.cpr(), other.cpr())
	def __repr__(self):
		return "%s runs %04d total %06d, cpr: %06d, running time %06d, frz %06d, miss %f"%(
			self.sym.ljust(30), self.runs, self.total, self.cpr(), self.running, self.frozen, self.missing())

if sys.argv[3:]:
	for xaddr in sys.argv[3:]:
		addr = int(xaddr, 16)
		print hex(addr), sl.find_sym(addr)
else:
	count = sl.per_sym(Counter)
	total = 0
	last_func = ''
	for line in sys.stdin.readlines():
		line = line.strip()
		running, asked, xaddr = line.split(' ')
		if asked == '+':
			total += 1
			running = running == 'R'
			addr = int(xaddr, 16)
			sym = sl.find_sym(addr)
			entry = sl.is_entry(addr)
			count[sym].inc(running, last_func != sym and entry)
			last_func = sym
	v = count.values()

	v = filter(lambda x:x.runs > 15, v)
	
	v.sort(Counter.cmp_runs)
	v.reverse()
	print "Most runs"
	for i in v:
		print i

	v.sort(Counter.cmp_running)
	v.reverse()
	print "Most on CPU"
	for i in v:
		print i

	v.sort(Counter.cmp_missing)
	v.reverse()
	print "Most missing"
	for i in v:
		print i
