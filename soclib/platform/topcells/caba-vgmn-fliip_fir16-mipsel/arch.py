
import sys,os
sys.path.insert(0, os.getenv('DSX') + '/lib/python')

import soclib

# import definitions from "segmentation.h"
import re
f = file('segmentation.h')
for line in f:
	res = re.match('#define\W+([^\W]+)\W+(.*)$', line)
	if res:
		exec(res.group(1) +' = ' + res.group(2))
f.close()

# create architecture

arch = soclib.Architecture(
	cell_size = 4,
	plen_size = 9,
	addr_size = 32,
	rerror_size = 1,
	clen_size = 1,
	rflag_size = 1,
	srcid_size = 8,
	pktid_size = 1,
	trdid_size = 1,
	wrplen_size = 1)

arch.create('common:mapping_table', 'mapping_table',
	addr_bits  = [8],
	srcid_bits = [8],
	cacheability_mask=0x00300000)
arch.create('common:loader', 'loader', binary='soft/bin.soft')
vgmn = arch.create('caba:vci_vgmn', 'vgmn0', min_latency = 2, fifo_depth = 8)

xcache = arch.create('caba:vci_xcache_wrapper', 'mips0',
	ident = 0,
	icache_ways  = 1,
	icache_sets  = 8,
	icache_words = 4,
	dcache_ways  = 1,
	dcache_sets  = 8,
	dcache_words = 4,
	iss_t = 'common:mips32el',
)

vgmn.to_initiator.new() // xcache.vci

ram = arch.create('caba:vci_ram', 'ram0')
ram.addSegment('reset', RESET_BASE, RESET_SIZE, True)
ram.addSegment('excep', EXCEP_BASE, EXCEP_SIZE, True)
ram.addSegment('text',  TEXT_BASE,  TEXT_SIZE,  True)
ram.addSegment('data',  DATA_BASE,  DATA_SIZE,  True)
ram.vci // vgmn.to_target.new()

tty = arch.create('caba:vci_multi_tty', 'tty0', names=['tty0'])
tty.addSegment('tty0', TTY_BASE, TTY_SIZE, False)
tty.vci // vgmn.to_target.new()

sim = arch.create('caba:vci_simhelper', 'sim0')
sim.addSegment('sim0', SIMHELPER_BASE, SIMHELPER_SIZE, False)
sim.vci // vgmn.to_target.new()

fb0 = arch.create('caba:vci_framebuffer', 'fb0',
	width=FB_WIDTH, height=FB_HEIGHT)
fb0.addSegment('fb0', FB_BASE, FB_SIZE, False)
fb0.vci // vgmn.to_target.new()

NTOIP_FIFO     = 1
NFRIP_FIFO     = 1
FIFO_BITWIDTH  = 32
CTRL_ADDR_SIZE = 12
NUSER_REGS     = 0
FIFO_SIZE      = 60
IP_BITWIDTH    = 16
useComposite   = True

if useComposite:
	ip0 = arch.create('caba:vci_fir16', 'ip0', burst_size=20)
	ip0.addSegment('ip0', IP_BASE, IP_SIZE, False)
	ip0.vci_target // vgmn.to_target.new()
	vgmn.to_initiator.new() // ip0.vci_initiator
else:
	dma0 = arch.create('caba:vci_acc_dma', 'dma0',
			NTOIP_FIFO=NTOIP_FIFO, NFRIP_FIFO=NFRIP_FIFO,
			FIFO_BITWIDTH=FIFO_BITWIDTH, CTRL_ADDR_SIZE=CTRL_ADDR_SIZE,
			NUSER_REGS=NUSER_REGS, FIFO_SIZE=FIFO_SIZE,
			burst_size=20)
	dma0.addSegment('dma0', IP_BASE, IP_SIZE, False)
	dma0.vci_target // vgmn.to_target.new()
	vgmn.to_initiator.new() // dma0.vci_initiator
	
	lw0 = arch.create('caba:load_word', 'lw0',
			INPUT_BITWIDTH=FIFO_BITWIDTH, OUTPUT_BITWIDTH=IP_BITWIDTH,
			SWAP_IN='true', SWAP_OUT='true')
	dma0.toip_data[0] // lw0.input
	
	lw1 = arch.create('caba:load_word', 'lw1',
			INPUT_BITWIDTH=IP_BITWIDTH, OUTPUT_BITWIDTH=FIFO_BITWIDTH,
			SWAP_IN='true', SWAP_OUT='true')
	lw1.output // dma0.frip_data[0]
	
	stub = arch.create('caba:firstub', 'stub0',
			FIFO_BITWIDTH=FIFO_BITWIDTH, CTRL_ADDR_SIZE=CTRL_ADDR_SIZE)
	lw0.output          // stub.input
	lw1.input           // stub.output
	
	coeffs = [
		 0,  0,  0,  0,
		 1,  1,  1,  1,
		 1,  1,  1,  1,
		 0,  0,  0,  0 ]
	ip0 = arch.create('caba:Mwmr_fir16', 'ip0', coeffs = coeffs)
	ip0.from_ctrl // stub.inputIP
	ip0.to_ctrl   // stub.outputIP

arch.generate(soclib.PfDriver())

