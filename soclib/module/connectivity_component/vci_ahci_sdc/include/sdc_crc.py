#!/usr/bin/env python

from mapping import *

######################################################################################
#   file   : crc.py  
#   date   : may 2015
#   author : Alain Greiner
#######################################################################################

############
def crc7( ):

    current = 0
    nbits   = 40   

    CMD0    = 0b0100000000000000000000000000000000000000
    CMD17   = 0b0101000100000000000000000000000000000000
    RSP     = 0x0800000155

    stream  = RSP 

    crc_0 = 0
    crc_1 = 0
    crc_2 = 0
    crc_3 = 0
    crc_4 = 0
    crc_5 = 0
    crc_6 = 0

    for x in xrange( nbits ):
        bit = (stream >> (nbits - 1 - x)) & 0x1

        nxt_0 = crc_6 ^ bit
        nxt_1 = crc_0
        nxt_2 = crc_1
        nxt_3 = crc_2 ^ nxt_0
        nxt_4 = crc_3
        nxt_5 = crc_4
        nxt_6 = crc_5

        crc_0 = nxt_0
        crc_1 = nxt_1
        crc_2 = nxt_2
        crc_3 = nxt_3
        crc_4 = nxt_4
        crc_5 = nxt_5
        crc_6 = nxt_6

        val = (crc_6 << 6) + \
              (crc_5 << 5) + \
              (crc_4 << 4) + \
              (crc_3 << 3) + \
              (crc_2 << 2) + \
              (crc_1 << 1) + \
              (crc_0 << 0)

        print ' - x = %d / val = %x' % ( x , val ) 

    return val
            
#############
def crc16( ):

    nbits   = 4096   

    # stream = 4096 bits / all = 1

    crc_0  = 0
    crc_1  = 0
    crc_2  = 0
    crc_3  = 0
    crc_4  = 0
    crc_5  = 0
    crc_6  = 0
    crc_7  = 0
    crc_8  = 0
    crc_9  = 0
    crc_10 = 0
    crc_11 = 0
    crc_12 = 0
    crc_13 = 0
    crc_14 = 0
    crc_15 = 0

    for x in xrange( nbits ):

        nxt_0  = crc_15 ^ True
        nxt_1  = crc_0
        nxt_2  = crc_1
        nxt_3  = crc_2
        nxt_4  = crc_3
        nxt_5  = crc_4 ^ nxt_0
        nxt_6  = crc_5
        nxt_7  = crc_6
        nxt_8  = crc_7
        nxt_9  = crc_8
        nxt_10 = crc_9
        nxt_11 = crc_10
        nxt_12 = crc_11 ^ nxt_0
        nxt_13 = crc_12
        nxt_14 = crc_13
        nxt_15 = crc_14

        crc_0  = nxt_0
        crc_1  = nxt_1
        crc_2  = nxt_2
        crc_3  = nxt_3
        crc_4  = nxt_4
        crc_5  = nxt_5
        crc_6  = nxt_6
        crc_7  = nxt_7
        crc_8  = nxt_8
        crc_9  = nxt_9
        crc_10 = nxt_10
        crc_11 = nxt_11
        crc_12 = nxt_12
        crc_13 = nxt_13
        crc_14 = nxt_14
        crc_15 = nxt_15

    val = (crc_15 << 15) + \
          (crc_14 << 14) + \
          (crc_13 << 13) + \
          (crc_12 << 12) + \
          (crc_11 << 11) + \
          (crc_10 << 10) + \
          (crc_9  <<  9) + \
          (crc_8  <<  8) + \
          (crc_7  <<  7) + \
          (crc_6  <<  6) + \
          (crc_5  <<  5) + \
          (crc_4  <<  4) + \
          (crc_3  <<  3) + \
          (crc_2  <<  2) + \
          (crc_1  <<  1) + \
          (crc_0  <<  0)

    return val

################################ test ######################################################

if __name__ == '__main__':

#   print 'crc16 = %x' %  crc16() 
    print 'crc7 = %x' %  crc7() 


# Local Variables:
# tab-width: 4;
# c-basic-offset: 4;
# c-file-offsets:((innamespace . 0)(inline-open . 0));
# indent-tabs-mode: nil;
# End:
#
# vim: filetype=python:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

