#!/usr/bin/env python
#
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
# Copyright (c) UPMC, Lip6
#         Nicolas Pouillon <nipo@ssji.net>, 2007
# 
# Maintainers: nipo
# 
# $Id: create_ppc_tables.py 917 2009-03-12 10:10:06Z nipo $
# 

import sys
import re

__id__ = "$Id: create_ppc_tables.py 917 2009-03-12 10:10:06Z nipo $"
__version__ = "$Revision: 917 $"

class Func:
	_proto_re = re.compile('^(?P<retval>.+)\s+(?P<name>[a-zA-Z0-9_:]+)\s*\((?P<proto>[^)]*)\)$')
	def __init__(self, name, retval, proto, disabled = False, code = ''):
		self.name = name
		self.retval = retval
		self.proto = proto
		self.code = code
		self.disabled = disabled
	@classmethod
	def parse(cls, fd):
		line = fd.readline().strip()
		while not line:
			line = fd.readline().strip()
		disabled = False
		if line == '/* disabled':
			disabled = True
			line = fd.readline().strip()
		rv = cls._proto_re.match(line)
		if not rv:
			raise ValueError, line
		code = ''
		line = fd.readline().strip()
		while not line:
			line = fd.readline().strip()
		assert line == '{'
		line = fd.readline()
		while line.rstrip() != "}":
			code += line
			line = fd.readline()
		if disabled:
			line = fd.readline()
			while line.strip() != "*/":
				line = fd.readline()
		return cls(rv.group('name'), rv.group('retval'), rv.group('proto'), disabled, code)
	def __cmp__(self, other):
		return cmp(self.name, other.name)
	def expand(self):
		dis = self.disabled and ('/* disabled\n', '\n*/') or ('', '')
		return '''%s%s %s(%s)
{
%s}%s'''%(dis[0], self.retval, self.name, self.proto, self.code, dis[1])
	def expand_proto(self):
		#dis = self.disabled and ('/* disabled\n', '\n*/') or ('', '')
		dis = ('', '')
		return '''%s%s %s(%s);%s'''%(dis[0], self.retval, self.name.split('::',1)[1], self.proto, dis[1])

class FuncFile:
	def __init__(self, filename, header = '', footer = '', *funcs):
		self.filename = filename
		self.funcs = {}
		self.header = header
		self.footer = footer
		self.funcs = {}
		for f in funcs:
			self.funcs[f.name] = f
	@classmethod
	def parse(cls, filename):
		fd = open(filename, 'r')
		header = ''
		line = fd.readline()
		while '**Start**' not in line:
			header += line
			line = fd.readline()
		funcs = ()
		try:
			while True:
				funcs += Func.parse(fd),
		except ValueError, e:
			line = e.args[0]
			assert '**End**' in line
		footer = fd.read()
		fd.close()
		return cls(filename, header, footer, *funcs)
	def expand_protos(self):
		return '\n'.join(map(Func.expand_proto, sorted(self.funcs.itervalues())))
	def expand(self):
		return (
			self.header +
			'''// **Start**

''' +
			'\n\n'.join(map(Func.expand, sorted(self.funcs.itervalues()))) +
			'''

// **End**
''' +
			self.footer
			)
	def __contains__(self, name):
		return name in self.funcs
	def __getitem__(self, name):
		return self.funcs[name]
	def __delitem__(self, name):
		del self.funcs[name]
	def __setitem__(self, name, val):
		self.funcs[name] = val
	def __iter__(self):
		return iter(self.funcs)

def primes(n):
	if n==2: return [2]
	elif n<2: return []
	s=range(3,n+1,2)
	mroot = n ** 0.5
	half=(n+1)/2-1
	i=0
	m=3
	while m <= mroot:
		if s[i]:
			j=(m*m-3)/2
			s[j]=0
			while j<half:
				s[j]=0
				j+=m
		i=i+1
		m=2*i+3
	return [2]+[x for x in s if x]

files = {
	"run":(("m_ins.d.op", -64, 0),
		   ((0, 'ill',     0),
			(3, 'twi',     0),
			(4, 'op4',     0),
			(7, 'mulli',   0),
			(8, 'subfic',  0),
			(10, 'cmpli',  0),
			(11, 'cmpi',   0),
			(12, 'addic',  0),
			(13, 'addic_', 0),
			(14, 'addi',   0),
			(15, 'addis',  0),
			(16, 'bc',     0),
			(17, 'sc',     0),
			(18, 'b',      0),
			(19, 'op19',   0),
			(20, 'rlwimi', 0),
			(21, 'rlwinm', 0),
			(23, 'rlwnm',  0),
			(24, 'ori',    0),
			(25, 'oris',   0),
			(26, 'xori',   0),
			(27, 'xoris',  0),
			(28, 'andi',   0),
			(29, 'andis',  0),
			(31, 'op31',   0),
			(32, 'lwz',    0),
			(33, 'lwzu',   0),
			(34, 'lbz',    0),
			(35, 'lbzu',   0),
			(36, 'stw',    0),
			(37, 'stwu',   0),
			(38, 'stb',    0),
			(39, 'stbu',   0),
			(40, 'lhz',    0),
			(41, 'lhzu',   0),
			(42, 'lha',    0),
			(43, 'lhau',   0),
			(44, 'sth',    0),
			(45, 'sthu',   0),
			(46, 'lmw',    0),
			(47, 'stmw',   0),
			)), 
	"op_op4":(("m_ins.x.func", None, -18, -8, 1, 8),
			   ((8,    "mulhhwu", 0),
				(12,   "machhwu", True),
				(40,    "mulhhw", 0),
				(44,    "machhw", True),
				(46,   "nmachhw", True),
				(76,  "machhwsu", True),
				(108,  "machhws", True),
				(110, "nmachhws", True),
				(136,  "mulchwu", 0),
				(140,  "macchwu", True),
				(168,   "mulchw", 0),
				(172,   "macchw", True),
				(174,  "nmacchw", True),
				(204, "macchwsu", True),
				(236,  "macchws", True),
				(238, "nmacchws", True),
				(392,  "mullhwu", 0),
				(396,  "maclhwu", True),
				(424,   "mullhw", 0),
				(428,   "maclhw", True),
				(430,  "nmaclhw", True),
				(460, "maclhwsu", True),
				(492,  "maclhws", True),
				(494, "nmaclhws", True),
				)),
	"op_op31":(("m_ins.x.func", 343, -7, 1, 7, 9),
				((0,       "cmp", 0),
				 (4,        "tw", 0),
				 (8,     "subfc", True),
				 (10,     "addc", True),
				 (11,   "mulhwu", 0),
				 (19,     "mfcr", 0),
				 (20,    "lwarx", 0),
				 (23,     "lwzx", 0),
				 (24,      "slw", 0),
				 (26,   "cntlzw", 0),
				 (28,      "and", 0),
				 (32,     "cmpl", 0),
				 (40,     "subf", True),
				 (54,    "dcbst", 0),
				 (55,    "lwzux", 0),
				 (60,     "andc", 0),
				 (75,    "mulhw", 0),
				 (83,    "mfmsr", 0),
				 (86,     "dcbf", 0),
				 (87,     "lbzx", 0),
				 (104,     "neg", True),
				 (119,   "lbzux", 0),
				 (124,     "nor", 0),
				 (131,   "wrtee", 0),
				 (136,   "subfe", True),
				 (138,    "adde", True),
				 (144,   "mtcrf", 0),
				 (146,   "mtmsr", 0),
				 (150,   "stwcx", 0),
				 (151,    "stwx", 0),
				 (163,  "wrteei", 0),
				 (183,   "stwux", 0),
				 (200,  "subfze", True),
				 (202,   "addze", True),
				 (215,    "stbx", 0),
				 (232,  "subfme", True),
				 (234,   "addme", True),
				 (235,   "mullw", True),
				 (246,  "dcbtst", 0),
				 (247,   "stbux", 0),
				 (262,    "icbt", 0),
				 (266,     "add", True),
				 (278,    "dcbt", 0),
				 (279,    "lhzx", 0),
				 (284,     "eqv", 0),
				 (311,   "lhzux", 0),
				 (316,     "xor", 0),
				 (323,   "mfdcr", 0),
				 (339,   "mfspr", 0),
				 (343,    "lhax", 0),
#				 (370,   "tlbia", 0),
				 (371,    "mftb", 0),
				 (375,   "lhaux", 0),
				 (407,    "sthx", 0),
				 (412,     "orc", 0),
				 (439,   "sthux", 0),
				 (444,      "or", 0),
				 (451,   "mtdcr", 0),
				 (454,   "dccci", 0),
				 (459,   "divwu", True),
				 (467,   "mtspr", 0),
				 (470,    "dcbi", 0),
				 (476,    "nand", 0),
				 (486,  "dcread", 0),
				 (491,    "divw", True),
				 (512,   "mcrxr", 0),
				 (533,    "lswx", 0),
				 (534,   "lwbrx", 0),
				 (536,     "srw", 0),
#				 (566, "tlbsync", 0),
				 (597,    "lswi", 0),
				 (598,    "sync", 0),
				 (661,   "stswx", 0),
				 (662,  "stwbrx", 0),
				 (725,   "stswi", 0),
				 (758,    "dcba", 0),
				 (790,   "lhbrx", 0),
				 (792,    "sraw", 0),
				 (824,   "srawi", 0),
				 (854,   "eieio", 0),
#				 (914,   "tlbsx", 0),
				 (918,  "sthbrx", 0),
				 (922,   "extsh", 0),
#				 (946,   "tlbre", 0),
				 (954,   "extsb", 0),
				 (966,   "iccci", 0),
#				 (978,   "tlbwe", 0),
				 (982,    "icbi", 0),
				 (998,  "icread", 0),
				 (1014,   "dcbz", 0),
				 )),
	"op_op19":(("m_ins.x.func", None, -21, -14, 1, 7),
				((0,     "mcrf", 0),
				 (16,    "bclr", 0),
				 (33,   "crnor", 0),
				 (50,     "rfi", 0),
				 (51,    "rfci", 0),
				 (129, "crandc", 0),
				 (150,  "isync", 0),
				 (193,  "crxor", 0),
				 (225, "crnand", 0),
				 (257,  "crand", 0),
				 (289,  "creqv", 0),
				 (417,  "crorc", 0),
				 (449,   "cror", 0),
				 (528,  "bcctr", 0),
				 )),
	}

p1024 = primes(1024)

class NoMod(Exception): pass
class NoTab(Exception): pass

def findmod(tab, func):
	prims = range(len(tab.keys()), 1024)
	#prims = [x for x in p1024 if x>len(tab.keys())]
	for prime in prims:
		used = [False]*prime
		res = {}
		for n in tab.keys():
			m = func(n,prime)
			if used[m] and res[m] != tab[n]:
#				print prime, ":", res[m], "collision avec", n
				break
			res[m] = tab[n]
			used[m] = True
		else:
#			if prime == 17:
#				print used, res, prime, sorted([x%prime for x in res.keys()]), func.im_self.l
			return prime
#	print len(tab), "not in", prims
	raise NoMod

def mktab_old(ops, func, pf = "ppc405_", ill = "", prime = None):
	out = ""
	res = []
	if prime == None:
		try:
			prime = findmod(ops)
		except:
			raise NoTab
	corres = {}
	for k in ops.iterkeys():
		corres[func(k,prime)] = k
	class_pf = 'op_'
	out += "// mod = %d\n"%(prime)
	out += "#define %s %sill\n"%((class_pf,)*2)
	out += "#define op4(au, a, bu, b, cu, c, du, d) %s##a, %s##b, %s##c, %s##d\n"%((class_pf,)*4)
	out += "Ppc405Iss::func_t const Ppc405Iss::%stable[] = {\n"%(pf)
	for i in range(prime):
		c = ''
		op = ill
		if i in corres:
			c = str(corres[i])
			op = ops[corres[i]]
		if i%4 == 0:
			out += "   op4("
		out += ("%4s, %s"%(c, op)).ljust(16)
		if i%4 != 3:
			out += ", "
		else:
			out += "),\n"
	while i%4 < 2:
		out += ",,"
		i+=1
	if i%4 == 2:
		out += ",)\n"
		i+=1
	out += """};
#undef op4
#undef %s
"""%class_pf
	return out, prime

def mktab(ops, hash_func, class_pfx, cpp_func_name = "func0", ill = "ill", prime = None):
	out = ""
	res = []
	if prime == None:
		try:
			prime = findmod(ops)
		except:
			raise NoTab
	corres = {}
	for k in ops.iterkeys():
		corres[hash_func(k,prime)] = k
	op_pf = 'op_'
	out += "// mod = %d\n"%(prime)
	out += "%sfunc_t const %s%s_table[%d] = {\n"%(class_pfx, class_pfx, cpp_func_name, prime)
	expl = '//  '
	line = '    '
	cwidth = 18
	for i in range(prime):
		c = ''
		op = '_'
		if i in corres:
			c = str(corres[i])
			op = "&"+class_pfx+op_pf+ops[corres[i]]
			expl += ("%s (%d)"%(c, i)).rjust(cwidth)
		else:
			expl += "".rjust(cwidth)
		line += op.rjust(cwidth)
		line += ", "
		expl += "  "
		if i%4 == 3:
			out +=  expl+'\n'+line+'\n'
			expl = "//  "
			line = "    "
	out +=  expl+'\n'+line+'\n'
	out += """};
"""
	return out, prime

class Calc:
	def __init__(self, *l):
		self.l = l
	def func(self, n, prime):
		v = 0
		for i in self.l:
			if i>0:
				v = v^(n>>i)
			elif i<0:
				v = v^(n<<-i)
			else:
				v = v^n
		return v%prime
	def txt(self, prime, base, nomod = False):
		text = '0'
		for i in self.l:
			if i>0:
				text += "^(%s>>%d)"%(base, i)
			elif i<0:
				text += "^(%s<<%d)"%(base, -i)
			else:
				text += "^%s"%base
		if nomod:
			return text
		else:
			return '(%s)%%%d'%(text, prime)

## class Calc:
## 	def __init__(self, *l):
## 		self.l = l
## 	def func(self, n, prime):
## 		return int((n*2654435761)&0x3fffffff)%prime
## 	def txt(self, prime):
## 		return 'ins.op.func*2654435761%%%d'%prime

def incc(ceil, li):
	for i in range(len(li)-1, -1, -True):
		li[i] += 1
		if li[i] < ceil:
			break
		else:
			li[i] = li[i-1]+2
			if not i:
				break

def mkf(fd, fd_proto, pf, ops, reg, la = None, *forval):
	if la and la < 0:
		minli = forval
		minprime = -la
	else:
		if la is None:
			la = len(ops.keys())*30/10+1
		minprime = 1024
		base = -22
		ceil = 10
		if not forval:
			li = range(base, base+2)
		else:
			li = list(forval)
		del forval
		while len(li) < 7:
			if minprime < la:
				break
			while li[0] < ceil:
				if minprime < la:
					break
				calc = Calc(*li)
				try:
					prime = findmod(ops, calc.func)
					sys.stdout.write("\r%4d %4d %30s %4d        "%(la, minprime, li, prime))
					sys.stdout.flush()
					if prime < minprime:
						minprime = prime
						minli = li[:]
						print minprime, minli
					incc(ceil, li)
				except NoMod:
					incc(ceil, li)
					sys.stdout.write(".")
					sys.stdout.flush()
			li = range(base, base+len(li)+1)
	calc = Calc(*minli)
	txt = calc.txt(minprime, reg, la and la < 0)
	class_pfx = "Ppc405Iss::"
	tab, prime = mktab(ops, calc.func, class_pfx, pf, ill="ill", prime = minprime)
	print
	print minli, len(ops.keys()), pf, prime, txt
	for name in ops.itervalues():
		fname = "%sop_%s"%(class_pfx, name)
		if not fname in fd_proto:
			fd_proto[fname] = Func(fname, 'void', '', name.startswith('op'), '')
	vals = {
		"tab":tab,
		"pf":pf,
		"prime":prime,
		"txt":txt,
		"class_pfx":class_pfx
		}
	fd.write(("""

%(tab)s

void %(class_pfx)s%(pf)s()
{
     func_t func = %(pf)s_table[%(txt)s];
     (this->*func)();
}

""")%vals)

def remap_data(data):
	out = {}
	for opcod, name, oe in data:
		out[opcod] = name
		if oe:
			out[opcod+512] = name
	return out

if __name__ == '__main__':
	import sys
	try:
		import psyco
		psyco.full()
	except ImportError:
		pass
	solo = len(sys.argv)>1
	if len(sys.argv)>1:
		if sys.argv[1] == '*':
			ks = files.keys()
			ks.sort()
		else:
			ks = [sys.argv[1]]
	else:
		ks = files.keys()
		ks.sort()
	fd = open('ppc405_jump_tables.cpp', "w")
	fd_proto = FuncFile.parse('ppc405_instructions.cpp')
	fd.write("""
#include "common/iss/ppc405.h"

namespace soclib { namespace common {

#define _ &Ppc405Iss::op_ill

""")
	for k in ks:
		hint, data = files[k]
		data = remap_data(data)
		if solo:
			hint = map(int, sys.argv[2:])
		mkf(fd, fd_proto, k, data, *hint)
	fd.write("""
#undef _

}}
""")
	fd.close()
	fd = open("ppc405_instructions.cpp", "w")
	fd.write(fd_proto.expand())
	fd.close()
	fd = open("../../../include/common/iss/ppc405_ops.inc", "w")
	fd.write(fd_proto.expand_protos()+'\n')
	fd.close()
