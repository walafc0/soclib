#!/usr/bin/env python

from dsx import *

import sys

mjpeg_file = "plan.jpg"
mjpeg_size = 48,48

# Declaration of all MWMR fifos
tg_demux    = Mwmr('tg_demux', 32, 2)
demux_vld   = Mwmr('demux_vld', 32, 2)
vld_iqzz    = Mwmr('vld_iqzz', 64, 2)
iqzz_idct   = Mwmr('iqzz_idct', 64*4, 2)
idct_libu   = Mwmr('idct_libu', 64, 2)
libu_ramdac = Mwmr('libu_ramdac', 48*8, 2)
huffman     = Mwmr('huffman', 32, 6)
quanti      = Mwmr('quanti', 64, 4)

tcg = Tcg(
	Task( 'tg', "tg",
	      {'output':tg_demux },
	      defines = {'FILE_NAME':'"%s"'%mjpeg_file}),
	Task( 'demux', "demux",
	      { 'input':tg_demux,
		'output':demux_vld,
		'huffman':huffman,
		'quanti':quanti },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	Task( 'vld', "vld",
	      { 'input':demux_vld,
		'output':vld_iqzz,
		'huffman':huffman },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	Task( 'iqzz', "iqzz",
	      { 'input':vld_iqzz,
		'output':iqzz_idct,
		'quanti':quanti },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	Task( 'idct', "idct",
	      { 'input':iqzz_idct,
		'output':idct_libu },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	Task( 'libu', "libu",
	      { 'input': idct_libu,
		'output': libu_ramdac },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	Task( 'ramdac', "ramdac",
	      { 'input': libu_ramdac },
	      defines = {'WIDTH':str(mjpeg_size[0]),
			 'HEIGHT':str(mjpeg_size[1])}),
	)

if __name__ == '__main__':
	p = Posix()
	tcg.generate(p)
	
