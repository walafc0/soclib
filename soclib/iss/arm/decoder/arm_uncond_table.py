#!/usr/bin/env python2

from op_decoder import decoder
from op_decoder import codegen

main = decoder.OpRegistry(
    32, 0x0ff000f0,
     "XXXXXXXX XXXX",

    ("00010000 XX0X", "cps_setend"),
    ("001XXXXX XXXX", "avsimd_data"),
    ("0100XXX0 XXXX", "avsimd_ldst"),
    # MP
    ("0100X001 XXXX", "nop"),  # unallocated memory hint
    ("0110X001 XXX0", "nop"),  # unallocated memory hint
    # v7
    ("0100X101 XXXX", "pli"),
    ("0110X101 XXX0", "pli"),
    # MP
    ("0101X001 XXXX", "pld"),
    ("0111X001 XXX0", "pld"),
    # v5te
    ("0101X101 XXXX", "pld"),
    ("0111X101 XXX0", "pld"),
    # v6k
    ("01010111 0001", "clrex"),
    # v6t2
    ("01010111 0100", "dsb"),
    # v7
    ("01010111 0101", "dmb"),
    # v6t2
    ("01010111 0110", "isb"),

    ("100XX1X0 XXXX", "srs"),
    ("100XX0X1 XXXX", "rfe"),
    ("101XXXXX XXXX", "blx"),
    ("11000X11 XXXX", "ldci"),
    ("11001XX1 XXXX", "ldci"),
    ("1101XXX1 XXXX", "ldci"),
    ("11000X10 XXXX", "stci"),
    ("11001XX0 XXXX", "stci"),
    ("1101XXX0 XXXX", "stci"),
    ("11000100 XXXX", "mcrr"),
    ("11000101 XXXX", "mrrc"),
#    v5
#    ("1110XXXX XXX0", "cpd"),
#    ("1110XXX0 XXX1", "mcr"),
#    ("1110XXX1 XXX1", "mrc"),
)

d = decoder.Decoder(main, False, 2, (27, 24), (20, 20), (7, 7), (4, 4))
d.prefix = "arm_uncond_"

codegen.putfile('../src/arm_uncond_decoding_table.cpp',
                d.gen_tables('ArmIss', ['arm.h'], ['soclib', 'common']))
codegen.putfile('../include/arm_uncond_ops.inc',
                d.gen_decls())

depth = d.tree()

d.self_test()

print "Max depth in tables:", depth
print "%d bytes lost in tables on a 32-bits machine" % d.stats(4)
print "%d bytes lost in tables on a 64-bits machine" % d.stats(8)

#funcs = main.funcs()
## print len(funcs), 'functions'

## for i in "000", "001", "010", "011", "100", "101", "110", "111":
##     for j in "01":
##         fmt = i+"X XXXX XXX"+j
##         table = main.table(fmt)
##         print i
##         table.printall()

#
# Local Variables:
# tab-width: 4
# py-indent-offset: 4
# indent-tabs-mode: nil
# End:
