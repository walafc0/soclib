#!/usr/bin/env python2

from op_decoder import decoder
from op_decoder import codegen

main = decoder.OpRegistry(
    16, 0xffc0,
    "XXXX XXXX XX",

    ("0100 0001 01", "adc"),
    ("0100 0000 00", "and"),
    ("0100 0011 10", "bic"),
    ("0100 0010 00", "tst"),
    ("0100 0001 00", "asr"),

    ("0100 0010 11", "cmn"),
    {"link": (7, "false", "true")},
    ("0100 0111 XX", "bx_r<[link]>"),
    ("0100 0010 10", "cmp"),
    ("0100 0101 XX", "cmp_hi"),
    ("0100 0110 XX", "cpy"),
#    ("0100 0110 XX", "mov_hi"),
    ("0100 0000 01", "eor"),
    ("0100 0000 10", "lsl"),
    ("0100 0000 11", "lsr"),
    ("0100 0100 XX", "add_hi"),
    ("0100 0011 01", "mul"),
    ("0100 0011 11", "mvn"),
    ("0100 0010 01", "neg"),
    ("0100 0011 00", "orr"),
    ("0100 0001 11", "ror"),
    ("0100 0001 10", "sbc"),

    {"lr": (11, "l", "r")},
    ("0000 XXXX XX", "ls[lr]_imm5"),

    ("0010 0XXX XX", "mov_imm8"),
    
    ("0001 0XXX XX", "asr_imm5"),

    {"sub_nadd": (9, "add", "sub")},
    ("0001 11XX XX", "[sub_nadd]_imm3"),
    ("0001 10XX XX", "[sub_nadd]_reg"),

    {"add_sub": (7, "add", "sub")},
    ("1011 0000 XX", "sp_[add_sub]"),

    {"sub_nadd": (11, "add", "sub")},
    ("0011 XXXX XX", "[sub_nadd]_imm8"),
    
    {"npc_sp": (11, "pc", "sp")},
    ("1010 XXXX XX", "add2[npc_sp]"),

    ("1011 0110 01", "cps_setend"),
#    ("1011 0110 01", "setend"),

    ("1011 1110 XX", "bkpt"),

    ("1101 XXXX XX", "bcond_swi"),

    ("1110 0XXX XX", "b"),
    ("1111 0XXX XX", "b_hi"),
    ("1111 1XXX XX", "bl"),
    ("1110 1XXX XX", "blx"),


    ("0010 1XXX XX", "cmp_imm"),

    {"load": (11, "false", "true")},
    {"ldst": (11, "st", "ld")},
    ("1100 XXXX XX", "ldstmia<[load]>", "[ldst]mia"),

    ("1011 110X XX", "pop"),
    ("1011 010X XX", "push"),

    {"byte": (6, "false", "true")},
    {"unsigned": (7, "false", "true")},
    {"bh": (6, "h", "b")},
    {"us": (7, "s", "u")},
    ("1011 0010 XX", "xt<[unsigned],[byte]>", "[us]xt[bh]"),

    ("1011 1010 00", "rev"),
    ("1011 1010 01", "rev16"),
    ("1011 1010 11", "revsh"),

#    ("0101 000X XX", "str_reg"),
#    ("0101 001X XX", "strh_reg"),
#    ("0101 010X XX", "strb_reg"),
#    ("0101 011X XX", "ldrsb_reg"),
#    ("0101 100X XX", "ldr_reg"),
#    ("0101 101X XX", "ldrh_reg"),
#    ("0101 110X XX", "ldrb_reg"),
#    ("0101 111X XX", "ldrsh_reg"),
    ("0101 XXXX XX", "ldst"),

    ("0100 1XXX XX", "ldr_pcrel"),

    {"load": (11, "false", "true")},
    {"byte": (12, "false", "true")},
    {"ldst": (11, "store", "load")},
    {" byte": (12, " word", " byte")},
    ("011X XXXX XX", "ldst_imm5<[load],[byte]>", "[ldst][ byte] imm5"),
    ("1001 XXXX XX", "ldst_sprel<[load]>", "[ldst] sp rel"),

    {"load": (11, "false", "true")},
    ("1000 XXXX XX", "ldsth_imm5", "[ldst] half"),
    
)

d = decoder.Decoder(main, False, 2, (15, 11))

d.prefix = 'thumb_'

#depth = d.tree()

codegen.putfile('../src/thumb_decoding_table.cpp',
                d.gen_tables('ArmIss', ['arm.h'], ['soclib', 'common']))
codegen.putfile('../include/thumb_ops.inc',
                d.gen_decls())
#codegen.putfile('../src/thumb_instructions.cpp',
#                d.gen_funcs('thumb_'))

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
