#!/usr/bin/env python

from dsx import *
import sys

from soclib import *

from app import tcg

from vgmn_noirq_mono import VgmnNoirqMono

cpu_count = int(sys.argv[1])
ram_count = 2

hard = VgmnNoirqMono(nbcpu = cpu_count,
					 nbram = ram_count,
					 with_gdb = False,
					 with_memchecker = False,
					 with_profiler = False,
					 )

m = Mapper(hard, tcg)

r = 0
for r, mwmr in enumerate(
	('tg_demux','demux_vld','vld_iqzz','iqzz_idct','idct_libu',
	 'libu_ramdac','huffman','quanti')):
	m.map(mwmr,
		  buffer = 'cram%d'%( (r % ram_count) ),
		  status = 'cram%d'%( (r % ram_count) ),
		  desc = 'cram%d'%( (r % ram_count) ))

for no, task in enumerate([ 'demux', 'vld', 'iqzz', 'idct', 'libu']):
	m.map(task,
		  desc = 'cram%d'%( (no % ram_count) ),
		  run = 'mips%d'%( (no % cpu_count) ),
		  stack = 'cram%d'%( (no % ram_count) ),
		  tty = 'tty0',
		  tty_no = no )

for task, coproc, ctrl in [
	('tg', 'tg0', 'tg0_ctrl'),
	('ramdac', 'ramdac0', 'ramdac0_ctrl'),
#	('idct', 'idct0', 'idct0_ctrl'),
	]:
	m.map(task, coprocessor = coproc, controller = ctrl)

for i in range(cpu_count):
	m.map("mips%d"%i,
		  private = 'cram0',
		  shared = 'uram0')

m.map(tcg,
	  private = 'cram0',
	  shared = 'uram0',
	  code = 'cram%d'%( (1 % ram_count) ),
	  tty = 'tty0',
	  tty_no = 0)

m.generate(MutekS())
