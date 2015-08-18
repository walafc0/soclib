#!/usr/bin/env python

from dsx import *
import sys

from soclib import *

from app import tcg

from vgmn_noirq_multi import VgmnNoirqMulti

dcache_lines = int(sys.argv[1])
icache_lines = int(sys.argv[2])

NP = 1

hard = VgmnNoirqMulti(nproc = NP, nram = 2,icache_nline = icache_lines, icache_nword = 8,dcache_nline = dcache_lines, dcache_nword = 8 )

m = Mapper(hard, tcg)

r = 0
for mwmr in 'tg_demux','demux_vld','vld_iqzz','iqzz_idct','idct_libu','libu_ramdac','huffman','quanti':
	r = 1-r
	m.map(tcg[mwmr],
		  buffer = 'cram%d'%r,
		  status = 'cram%d'%r,
		  desc = 'cram%d'%r)

m.map(tcg['demux'],
      desc = 'cram0',
      run = 'mips0',
      stack = 'cram0',
      tty = 'tty',
      tty_no = 0)

m.map(tcg['vld'],
      desc = 'cram1',
      run = 'mips0',
      stack = 'cram1',
      tty = 'tty',
      tty_no = 0)

m.map(tcg['iqzz'],
      desc = 'cram0',
      run = 'mips0',
      stack = 'cram0',
      tty = 'tty',
      tty_no = 0)

m.map(tcg['idct'],
      desc = 'cram1',
      run = 'mips0',
      stack = 'cram1',
      tty = 'tty',
      tty_no = 0)

m.map(tcg['libu'],
      desc = 'cram0',
      run = 'mips0',
      stack = 'cram0',
      tty = 'tty',
      tty_no = 0)

m.map(tcg['tg'],
      desc = 'cram0',
      run = 'mips0',
      stack = 'cram0',
      tty = 'tty',
      tty_no = 0)

## m.map(tcg['tg'],
##       vci = 'vgmn0',
##       address = 0x70200000)

m.map('ramdac',
      desc = 'cram0',
      run = 'mips0',
      stack = 'cram0',
      tty = 'tty',
      tty_no = 0)

## m.map(tcg['ramdac'],
##       vci = 'vgmn0',
##       address = 0x71200000)

r = 0
for i in range(NP):
	r = 1-r
	m.map(m.hard['mips%d'%i],
	      private = 'cram%d'%r,
	      shared = 'uram%d'%r,
	      tty = 'tty',
	      tty_no = 0)

m.map(tcg,
      private = 'cram0',
      shared = 'uram0',
      code = 'cram1',
      tty = 'tty',
      tty_no = 0)

posix = Posix()
tcg.generate(posix)

m.generate(MutekS())
#m.generate(MutekH())
