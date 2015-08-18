#!/usr/bin/env python2

from op_decoder import decoder
from op_decoder import codegen

main = decoder.OpRegistry(
    32, 0x0ff000f0,
     "XXXX XXXX XXXX",

    # v4, deprecated in v6
    {'b':(22, '', 'b')},
    ("0001 0X00 1001", "swp", "swp[b]"),

    {'set':(20, '', 's')},
    # v4
    ("0000 101X XXX0", "adc[set]"),
    ("0000 101X 0XX1", "adc[set]"),
    ("0010 101X XXXX", "adc[set]"),
    ("0000 100X XXX0", "add[set]"),
    ("0000 100X 0XX1", "add[set]"),
    ("0010 100X XXXX", "add[set]"),
    ("0000 000X XXX0", "and[set]"),
    ("0000 000X 0XX1", "and[set]"),
    ("0010 000X XXXX", "and[set]"),
    ("0001 110X XXX0", "bic[set]"),
    ("0001 110X 0XX1", "bic[set]"),
    ("0011 110X XXXX", "bic[set]"),
    ("0000 001X XXX0", "eor[set]"),
    ("0000 001X 0XX1", "eor[set]"),
    ("0010 001X XXXX", "eor[set]"),
    ("0001 101X XXX0", "mov[set]"),
    ("0001 101X 0XX1", "mov[set]"),
    ("0011 101X XXXX", "mov[set]"),
    ("0001 111X XXX0", "mvn[set]"),
    ("0001 111X 0XX1", "mvn[set]"),
    ("0011 111X XXXX", "mvn[set]"),
	("0001 100X XXX0", "orr[set]"),
	("0001 100X 0XX1", "orr[set]"),
	("0011 100X XXXX", "orr[set]"),
    ("0000 011X XXX0", "rsb[set]"),
    ("0000 011X 0XX1", "rsb[set]"),
    ("0010 011X XXXX", "rsb[set]"),
    ("0000 111X XXX0", "rsc[set]"),
    ("0000 111X 0XX1", "rsc[set]"),
    ("0010 111X XXXX", "rsc[set]"),
    ("0000 110X XXX0", "sbc[set]"),
    ("0000 110X 0XX1", "sbc[set]"),
    ("0010 110X XXXX", "sbc[set]"),
    ("0000 010X XXX0", "sub[set]"),
    ("0000 010X 0XX1", "sub[set]"),
    ("0010 010X XXXX", "sub[set]"),

    ("0001 0111 XXX0", "cmns", "cmn"),
    ("0001 0111 0XX1", "cmns", "cmn"),
    ("0011 0111 XXXX", "cmns", "cmn"),
    ("0001 0101 XXX0", "cmps", "cmp"),
    ("0001 0101 0XX1", "cmps", "cmp"),
    ("0011 0101 XXXX", "cmps", "cmp"),
    ("0001 0001 XXX0", "tsts", "tst"),
    ("0001 0001 0XX1", "tsts", "tst"),
    ("0011 0001 XXXX", "tsts", "tst"),
    ("0001 0011 XXX0", "teqs", "teq"),
    ("0001 0011 0XX1", "teqs", "teq"),
    ("0011 0011 XXXX", "teqs", "teq"),

    {'l':(24, '', 'l')},
    ("101X XXXX XXXX", "b[l]"),

	("1110 XXXX XXX0", "cdp"),
	("110X XXX1 XXXX", "ldc"),

    {'hbh':(5, "b", "h")},
    {'h12':(5, "1", "2")},
    {'ldst':(20, "st", "ld")},
    {'load':(20, "false", "true")},
    {'pre':(24, "false", "true")},
    {' pre':(24, "", " pre")},
    {'signed':(6, "false", "true")},
    {'reg':(25, "false", "true")},
    {'u':(6, "u", "")},
    #    +----------- pre = 1
    #    |    +------ load = 1
    #    |    |  ++-- 01 unsigned, half
    #    |    |  ||   10 signed, byte
    #    |    |  ||   11 signed, half
    ("000X XXXX 1011", "ldstrh<[h12],[pre],[load],[signed]>", "[ldst]r[hbh][u][ pre]"),
    ("000X XXXX 1101", "ldstrh<[h12],[pre],[load],[signed]>", "[ldst]r[hbh][u][ pre]"),
    ("000X XXXX 1111", "ldstrh<[h12],[pre],[load],[signed]>", "[ldst]r[hbh][u][ pre]"),
    #   +------- reg = 1
    #   |+------ pre = 1
    #   ||    +- load = 1
    #   ||    |
    ("010X XXXX XXXX", "ldstr<[reg],[pre],[load]>", "[ldst]r imm[ pre]"),
    ("011X XXXX XXX0", "ldstr<[reg],[pre],[load]>", "[ldst]r reg[ pre]"),

    # TODO: LDRBT
    # TODO: LDRT
    ("100X XXXX XXXX", "ldstm", '[ldst]m'),
    
	("1110 XXX0 XXX1", "mcr"),


    ("0000 001X 1001", "mla", "mla[set]"),
    ("1110 XXX1 XXX1", "mrc"),
    ("0001 0X00 0000", "mrs"),
    ("0001 0X10 0000", "msr"),
    ("0011 0X10 XXXX", "msr", "msr (imm)"),
    ("0000 000X 1001", "mul", "mul[set]"),

    ("0000 111X 1001", "smlal", "smlal[set]"),
	("0000 110X 1001", "smull", "smull[set]"),
	("110X XXX0 XXXX", "stc"),
    # STRBT
    ("1111 XXXX XXXX", "swi"),
    ("0000 100X 1001", "umull", "umull[set]"),
    ("0000 101X 1001", "umlal", "umlal[set]"),

    # v4-T
    ("0001 0010 0001", "bx"),

    # v5-T
    ("0001 0010 0011", "blx"),
    ("0001 0010 0111", "bkpt"),
#   ("              ", "cdp2"),
    ("0001 0110 0001", "clz"),
#   ("              ", "ldc2"),
#   ("              ", "mcr2"),
#   ("              ", "mrc2"),
#   ("              ", "stc2"),

    # v5-TExP, v5-TEJ, v5-TE
#   ("0001 0000 0101", "qadd"),
#   ("0001 0100 0101", "qdadd"),
#   ("0001 0010 0101", "qsub"),
#   ("0001 0110 0101", "qdsub"),

    {'btx':(5, 'b', 't')},
    {'bty':(6, 'b', 't')},
    ("0001 0000 1XX0", "smla_xy", "smla[btx][bty]"),
    ("0001 0010 1X00", "smlaw_y", "smlaw[bty]"),
    ("0001 0010 1X10", "smulw_y", "smulw[bty]"),
    ("0001 0110 1XX0", "smul_xy", "smul[btx][bty]"),

    # v5-TEJ, v5-TE
#   ("000X XXX1 1101", "ldrd"),
#	("1100 0100 XXXX", "mcrr"),
#	("1100 0101 XXXX", "mrrc"),
#   ("000X XXX0 1101", "strd"),

    # v5-TEJ
#   BXJ

    # v6
    ("0001 100X 1001", "[ldst]rex"),
    ("0110 1011 0011", "rev"),
    ("0110 1011 1011", "rev16"),
    ("0110 1111 1011", "revsh"),

#	("              ", "mcrr2"),
#	("              ", "mrrc2"),
#   ("0110 1000 X001", "pkhbt"),
#   ("0110 1000 X101", "pkhtb"),
#   ("0110 0X0X 0001", "p_add16"),
#   ("0110 0X0X 0101", "p_add"),
#   ("0110 0X0X 1001", "p_add8"),
#   ("0110 0X0X 0011", "p_addsubx"),
#   ("0110 0X1X 0111", "p_sub16"),
#   ("0110 0X1X 0101", "p_sub"),
#   ("0110 0X1X 1111", "p_sub8"),
#   ("0110 0X1X 0101", "p_subaddx"),
#   ("0110 1000 1011", "sel"),
    {'su':(22, 's', 'u')},

#   ("0110 1X1X XX01", "[su]sat"),
#   ("0110 1X10 0011", "[su]sat16"),

    ("0110 1X00 0111", "[su]xtb16"),
    ("0110 1X10 0111", "[su]xtb"),
    ("0110 1X11 0111", "[su]xth"),
#   ("0111 1000 0001", "usad8"),
#   ("0111 1000 0001", "usada8"),
    ("0000 0100 1001", "umaal"),

    # future
#    ("0111 XXX1 1111", "ill"),
)

#pld v5-TE, v5-TEJ

# all v6
#blx
#cps
#rfe
#setend
#srs


# 2058
#d = decoder.Decoder(main, True, 3, (27, 25), (4, 4))
# 4414
#d = decoder.Decoder(main, True, 6, (27, 27), (25, 24), (22, 20), (6, 4))
# 1908
#d = decoder.Decoder(main, True, 2, (27, 24), (20, 20), (4, 4))
# 1902
d = decoder.Decoder(main, False, 2, (27, 24), (20, 20), (7, 7), (4, 4))
# 2240
#d = decoder.Decoder(main, True, 2, (27, 24), (20, 20), (7, 4))
# 2526
#d = decoder.Decoder(main, True, 2, (27, 25), (20, 20))
# 5484
#d = decoder.Decoder(main, True, (27, 25), (21, 20))

#print len(d._Decoder__tables)
#for n, t in enumerate(d.tables()):
#    print n, t

#depth = d.tree()

codegen.putfile('../src/arm_decoding_table.cpp',
                d.gen_tables('ArmIss', ['arm.h'], ['soclib', 'common']))
codegen.putfile('../include/arm_ops.inc',
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
