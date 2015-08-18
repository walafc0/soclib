
def mask_to_bitlist(mask):
	bits = []
	i = 0
	while mask:
		if mask & 1:
			bits.append(i)
		i += 1
		mask >>= 1
	return bits

def bitlist_to_slices(bitlist):
	slices = []
	bitlist = list(sorted(bitlist))
	while bitlist:
		b = bitlist.pop()
		l, r = b, b
		while bitlist and bitlist[-1] == r-1:
			r = bitlist.pop()
		slices.append((l,r))
	return slices

def slices_to_bitlist(slices):
	bits = []
	for l, r in slices:
		for i in range(r, l+1):
			bits.append(i)
	return list(sorted(bits))

def slices_diff(sll, slr):
	bitsl = set(slices_to_bitlist(sll))
	bitsr = set(slices_to_bitlist(slr))
	return bitlist_to_slices(bitsl-bitsr)

class Packer:
	def __init__(self, base_slices, mask):
		self.__base_slices = base_slices
		self.__mask = mask
		self.__base_slices_bitlist = slices_to_bitlist(base_slices)
		self.__dest_slices_bitlist = []
		for i in mask_to_bitlist(mask):
			self.__dest_slices_bitlist.append(self.__base_slices_bitlist[i])
		self.__dest_slices = bitlist_to_slices(self.__dest_slices_bitlist)
		self.__size = len(self.__dest_slices_bitlist)

#		print "New packer:", self.__base_slices_bitlist, self.__base_slices, mask, "->", self.__dest_slices_bitlist, self.__dest_slices

	def mask(self):
		return self.__mask

	def slices(self):
		return self.__dest_slices

	def bits(self):
		return self.__dest_slices_bitlist
	
	def size(self):
		return self.__size
		
	def parse_mask(self, form):
		n = 0
		r = filter(lambda x:x in '01X', reversed(form))
		for bit, c in zip(self.__dest_slices_bitlist, r):
			n |= int(c == 'X')<<bit
		assert len(r) == self.__size,\
			   ValueError(form, r, self.__size)
		return n

	def parse_bits(self, form):
		n = 0
		r = filter(lambda x:x in '01X', reversed(form))
		for bit, c in zip(self.__dest_slices_bitlist, r):
			n |= int(c == '1')<<bit
		assert len(r) == self.__size,\
			   ValueError(form, r)
		return n

	def mops(self):
		b = self.__dest_slices_bitlist[0]
		b = [0, 1<<b]
		for bit in self.__dest_slices_bitlist[1:]:
			b = b+map(lambda x:x|(1<<bit), b)
		return b

	def mask_bits(self, form):
		n = []
		r = filter(lambda x:x in '01X', reversed(form))
		for bit, c in zip(self.__dest_slices_bitlist, r):
			if c == 'X':
				n.append(bit)
		return n

	def pprint_mask(self, slices, base_mask):
		mbits = slices_to_bitlist(slices)
		r = filter(lambda x:x in '01X', base_mask.replace("_", "X"))
		ret = ''
#		print 'pprint_mask', base_mask, slices
		for bbit, bit, c in zip(self.__base_slices_bitlist,
						  self.__dest_slices_bitlist,
						  reversed(r)):
			if bit in mbits:
				ret = 'X'+ret
			else:
				ret = c.replace('X', '_')+ret
#			print ''.join(r), mbits, bbit, bit, c, ret
#		print ret
#		print 
		return ret

	def ids_for_op_mask(self, mask):
		ret = [0]
		for n, val in enumerate(self.__base_slices_bitlist):
			if mask & 1:
				ret = ret + map(lambda x:x|(1<<n), ret)
			mask >>= 1
		return sorted(ret)

	def mop_to_plain(self, mop):
#		r = 0
#		for dest_bit, orig_bit in enumerate(reversed(self.__base_slices_bitlist)):
#			r |= ((mop >> orig_bit) & 1) << dest_bit
		w = 0
		ret = 0
		for l, r in sorted(self.__dest_slices):
			rr = ((mop >> (r-w)) & (((1<<(l-r+1))-1)<<w))
#			print ret, l, r, w, (mop >> (r-w)), (((1<<(l-r+1))-1)<<w), rr
			ret |= rr
			w += (l-r+1)
#		print self.__base_slices, self.__dest_slices
#		print '%x mop_to_plain(%x) = %x'%(self.__mask, mop, ret)
#		assert ret < (1<<self.__size)
		return ret
