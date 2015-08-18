
import struct

# This is a naive jpeg parser just to extract image size

def jpeg_size(fn):
	fd = open(fn, 'r')
	data = fd.read(512) # should be enough
	fd.close()
	first_header = data.split('\xff\xc0')[1]
	sz = struct.unpack(">HH", first_header[3:7])
	w = sz[1]
	w = int((int(w)+7)/8)*8
	h = sz[0]
	h = int((int(h)+7)/8)*8
	return w, h

if __name__ == '__main__':
	import sys
	print jpeg_size(sys.argv[1])
