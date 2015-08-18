#!/usr/bin/env python

from dsx import *
import sys
from soclib import *

#############################################################
# VARIABLES
#############################################################
NP = int(sys.argv[1])

#############################################################
# CONSTANTS
#############################################################
N_CRUNCH = NP
N_A = 6
N_B = 2

icache_lines = 1024
dcache_lines = 1024

#############################################################
# APPLICATION
#############################################################
#from sw_app import tcg

prod_model = TaskModel('prod',
                       ports = { 'input' : MwmrInput(4), 'output' : MwmrOutput(4) },
                       impls = [ SwTask('prod_func',
                                        stack_size = 1024,
                                        sources = ['src/prod/prod.c'],
                                        defines = ['initial_a', 'final_a', 'initial_b', 'final_b' ] ) ] )

crunch_model = TaskModel('crunch',
                         ports = { 'input' : MwmrInput(4), 'output' : MwmrOutput(6) },
                         impls = [ SwTask('crunch_func',
                                          stack_size = 200000,
                                          sources = ['src/crunch/crunch.c'] ) ] )

maxi_model = TaskModel('maxi',
                       ports = { 'input' : MwmrInput(6) },
                       impls = [ SwTask('maxi_func',
                                        stack_size = 200000,
                                        sources = ['src/maxi/maxi.c'],
                                        defines = ['A','B'] ) ] )


fifo_null = Mwmr('fifo_null', 4, 3)
fifo_prod = Mwmr('fifo_prod', 4, 3)
fifo_maxi = Mwmr('fifo_maxi', 6, 3)

tasks = ()

tasks = ()

tasks += (
    Task('prod', 'prod',
         portmap = {'input':fifo_null, 'output':fifo_prod},
         defines = {'initial_a': str(0), 'final_a':str(N_A),'initial_b': str(0), 'final_b':str(N_B)} ),
    Task('maxi', 'maxi',
         portmap = {'input':fifo_maxi},
         defines = {'A': str(N_A), 'B':str(N_B)} ),
    )

for i in range(N_CRUNCH):
    tasks += (
        Task('crunch%d'%i, 'crunch',
             portmap = {'input':fifo_prod, 'output' :fifo_maxi} ),
        )

tcg = Tcg(*tasks)
    
#############################################################
# HARDWARE
#############################################################
from vgmn_noirq_multi import VgmnNoirqMulti

hard = VgmnNoirqMulti(nproc = NP, nram = 2, icache_nline = icache_lines, icache_nword = 8, dcache_nline = dcache_lines, dcache_nword = 8 )

#############################################################
# MAPPING
#############################################################
m = Mapper(hard, tcg)

m.map(tcg['fifo_null'],
      buffer = 'cram0',
      status = 'cram0',
      desc   = 'cram0')

m.map(tcg['fifo_prod'],
      buffer = 'cram0',
      status = 'cram0',
      desc   = 'cram0')

m.map(tcg['fifo_maxi'],
      buffer = 'cram0',
      status = 'cram0',
      desc   = 'cram0')

m.map(tcg['prod'],
      desc   = 'cram0',
      run    = 'mips0',
      stack  = 'cram0',
      tty    = 'tty',
      tty_no = 0)

m.map(tcg['maxi'],
      desc   = 'cram0',
      run    = 'mips0',
      stack  = 'cram0',
      tty    = 'tty',
      tty_no = 0)


for i in range(N_CRUNCH):
    m.map(tcg['crunch%d'%i],
          desc   = 'cram%d'%(i%2),
          run    = 'mips%d'%i,
          stack  = 'cram%d'%(i%2),
          tty    = 'tty',
          tty_no = 0)

for i in range(NP):
    m.map(m.hard['mips%d'%i],
          private = 'cram%d'%(i%2),
          shared  = 'uram%d'%(i%2))

m.map(tcg,
      private = 'cram0',
      shared  = 'uram0',
      code    = 'cram0',
      tty     = 'tty',
      tty_no  = 0)

#############################################################
# GENERATION
#############################################################
posix = Posix()
tcg.generate(posix)
m.generate(MutekS(with_stream = True))
# m.generate(MutekH())
