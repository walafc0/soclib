
import soclib

def VgmnNoirqMulti(nproc = 1, nram = 2 ):

	pf = soclib.Architecture(cell_size = 4,
				 plen_size = 9,
				 addr_size = 32,
				 rerror_size = 1,
				 clen_size = 1,
				 rflag_size = 1,
				 srcid_size = 8,
				 pktid_size = 8,
				 trdid_size = 8,
				 wrplen_size = 1
				 )
	
	mt = pf.create('common:mapping_table',
		       'mapping_table',
		       addr_bits = [8],
		       srcid_bits = [8],
		       cacheability_mask = 0x200000)
	
	pf.create('common:loader', 'loader')

	vgmn = pf.create('caba:vci_vgmn', 'vgmn0', min_latency = 2, fifo_depth = 8, mt = mt )

	for i in range(nproc):
		xcache = pf.create('caba:vci_xcache_wrapper',
				   'mips%d'%i,
				   ident = i,
				   icache_ways = 1,
				   icache_sets = 8,
				   icache_words = 4,
				   dcache_ways = 1,
				   dcache_sets = 8,
				   dcache_words = 4,
				   #iss_t='common:mips32el'
				   iss_t = 'common:iss2_simhelper',
				   simhelper_iss_t = 'common:mips32el'
				   )
		
		
		vgmn.to_initiator.new() // xcache.vci
		
	for i in range(nram):
		ram = pf.create('caba:vci_ram', 'ram%d'%i)
		base = 0x10000000*i+0x10000000
		ram.addSegment('cram%d'%i, base, 0x100000, True)
		ram.addSegment('uram%d'%i, base + 0x200000, 0x100000, False)
		ram.vci // vgmn.to_target.new()
	ram.addSegment('boot', 0xbfc00000, 0x800,True)
	ram.addSegment('excep', 0x80000080, 0x800,True)
	#ram.addSegment('text', 0x00400000, 0x50000,True)

	tty_names = []
	for i in range(nproc):
		tty_names.append('tty%d'%i)

	
	tty = pf.create('caba:vci_multi_tty', 'tty', names = tty_names)
	tty.addSegment('tty', 0x90200000, nproc * 0x20, False)
	tty.vci // vgmn.to_target.new()

	return pf

if __name__ == '__main__':
	from dsx import *
	VgmnNoirqMulti().generate(soclib.PfDriver('hard'))
