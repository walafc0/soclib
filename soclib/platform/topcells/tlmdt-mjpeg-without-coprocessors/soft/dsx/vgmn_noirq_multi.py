
import soclib

def VgmnNoirqMulti(nproc, nram, icache_nline = 16, icache_nword = 8, dcache_nline = 16, dcache_nword = 8 ):
	pf = soclib.Architecture(cell_size = 4,
				 plen_size = 8,
				 addr_size = 32,
				 rerror_size = 1,
				 clen_size = 1,
				 rflag_size = 1,
				 srcid_size = 8,
				 pktid_size = 1,
				 trdid_size = 1,
				 wrplen_size = 1
				 )
	
	mt = pf.create('common:mapping_table',
		       'mapping_table',
		       addr_bits = [8],
		       srcid_bits = [8],
		       cacheability_mask = 0x200000)
	pf.create('common:loader', 'loader')

	vgmn = pf.create('caba:vci_vgmn', 'vgmn0', min_latency = 10, fifo_depth = 8, mt = mt)

	for i in range(nproc):
		xcache = pf.create('caba:vci_xcache_wrapper',
				   'mips%d'%i,
				   ident = i,
				   icache_ways = 1,
				   icache_sets = icache_nline,
				   icache_words = icache_nword,
				   dcache_ways = 1,
				   dcache_sets = dcache_nline,
				   dcache_words = dcache_nword,
				   iss_t='common:mips32el'
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

	tty = pf.create('caba:vci_multi_tty', 'tty', names = "tty0")
	tty.addSegment('tty', 0x90200000, 0x20, False)
	tty.vci // vgmn.to_target.new()

	return pf

if __name__ == '__main__':
	from dsx import *
	VgmnNoirqMulti().generate(soclib.PfDriver('hard'))
