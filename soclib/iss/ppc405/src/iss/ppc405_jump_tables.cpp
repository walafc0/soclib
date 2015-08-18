/*
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */

#include "ppc405.h"

namespace soclib { namespace common {

#define _ &Ppc405Iss::op_ill



// mod = 14
Ppc405Iss::func_t const Ppc405Iss::op_op19_table[14] = {
//               0 (0)             129 (1)             289 (2)             193 (3)  
    &Ppc405Iss::op_mcrf, &Ppc405Iss::op_crandc, &Ppc405Iss::op_creqv, &Ppc405Iss::op_crxor, 
//              16 (4)             417 (5)              33 (6)              51 (7)  
    &Ppc405Iss::op_bclr, &Ppc405Iss::op_crorc, &Ppc405Iss::op_crnor, &Ppc405Iss::op_rfci, 
//             257 (8)              50 (9)            528 (10)            225 (11)  
    &Ppc405Iss::op_crand, &Ppc405Iss::op_rfi, &Ppc405Iss::op_bcctr, &Ppc405Iss::op_crnand, 
//            150 (12)            449 (13)  
    &Ppc405Iss::op_isync, &Ppc405Iss::op_cror, 
};


void Ppc405Iss::op_op19()
{
     func_t func = op_op19_table[(0^(m_ins.x.func<<21)^(m_ins.x.func<<14)^(m_ins.x.func>>1)^(m_ins.x.func>>7))%14];
     (this->*func)();
}



// mod = 342
Ppc405Iss::func_t const Ppc405Iss::op_op31_table[342] = {
//               0 (0)                                   8 (2)                      
    &Ppc405Iss::op_cmp,                  _, &Ppc405Iss::op_subfc,                  _, 
//                                                      24 (6)                      
                     _,                  _, &Ppc405Iss::op_slw,                  _, 
//              32 (8)                                 40 (10)                      
    &Ppc405Iss::op_cmpl,                  _, &Ppc405Iss::op_subf,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                279 (25)            104 (26)                      
                     _, &Ppc405Iss::op_lhzx, &Ppc405Iss::op_neg,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                311 (33)                                136 (35)  
                     _, &Ppc405Iss::op_lhzux,                  _, &Ppc405Iss::op_subfe, 
//                                144 (37)                                          
                     _, &Ppc405Iss::op_mtcrf,                  _,                  _, 
//                                343 (41)                                          
                     _, &Ppc405Iss::op_lhax,                  _,                  _, 
//                                 11 (45)                                 19 (47)  
                     _, &Ppc405Iss::op_mulhwu,                  _, &Ppc405Iss::op_mfcr, 
//                                375 (49)                                200 (51)  
                     _, &Ppc405Iss::op_lhaux,                  _, &Ppc405Iss::op_subfze, 
//            534 (52)                                                    747 (55)  
    &Ppc405Iss::op_lwbrx,                  _,                  _, &Ppc405Iss::op_mullw, 
//            407 (56)                                                    232 (59)  
    &Ppc405Iss::op_sthx,                  _,                  _, &Ppc405Iss::op_subfme, 
//                                 75 (61)                                 83 (63)  
                     _, &Ppc405Iss::op_mulhw,                  _, &Ppc405Iss::op_mfmsr, 
//            439 (64)                                                              
    &Ppc405Iss::op_sthux,                  _,                  _,                  _, 
//            598 (68)                                                              
    &Ppc405Iss::op_sync,                  _,                  _,                  _, 
//                                                    131 (74)                      
                     _,                  _, &Ppc405Iss::op_wrtee,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                    163 (82)                      
                     _,                  _, &Ppc405Iss::op_wrteei,                  _, 
//                                662 (85)                                          
                     _, &Ppc405Iss::op_stwbrx,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                        533 (95)  
                     _,                  _,                  _, &Ppc405Iss::op_lswx, 
//                                                                         54 (99)  
                     _,                  _,                  _, &Ppc405Iss::op_dcbst, 
//           235 (100)                                                              
    &Ppc405Iss::op_mullw,                  _,                  _,                  _, 
//                                                                        86 (107)  
                     _,                  _,                  _, &Ppc405Iss::op_dcbf, 
//                               758 (109)                               597 (111)  
                     _, &Ppc405Iss::op_dcba,                  _, &Ppc405Iss::op_lswi, 
//                                                   790 (114)                      
                     _,                  _, &Ppc405Iss::op_lhbrx,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                   150 (122)                      
                     _,                  _, &Ppc405Iss::op_stwcx,                  _, 
//                                                   661 (126)           520 (127)  
                     _,                  _, &Ppc405Iss::op_stswx, &Ppc405Iss::op_subfc, 
//                                                   854 (130)           536 (131)  
                     _,                  _, &Ppc405Iss::op_eieio, &Ppc405Iss::op_srw, 
//                               512 (133)                               552 (135)  
                     _, &Ppc405Iss::op_mcrxr,                  _, &Ppc405Iss::op_subf, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                   725 (142)                      
                     _,                  _, &Ppc405Iss::op_stswi,                  _, 
//                                                   246 (146)           918 (147)  
                     _,                  _, &Ppc405Iss::op_dcbtst, &Ppc405Iss::op_sthbrx, 
//                               262 (149)                               616 (151)  
                     _, &Ppc405Iss::op_icbt,                  _, &Ppc405Iss::op_neg, 
//                               278 (153)                                          
                     _, &Ppc405Iss::op_dcbt,                  _,                  _, 
//                                                   648 (158)           966 (159)  
                     _,                  _, &Ppc405Iss::op_subfe, &Ppc405Iss::op_iccci, 
//                                                                       982 (163)  
                     _,                  _,                  _, &Ppc405Iss::op_icbi, 
//                                                                       998 (167)  
                     _,                  _,                  _, &Ppc405Iss::op_icread, 
//                                                                      1014 (171)  
                     _,                  _,                  _, &Ppc405Iss::op_dcbz, 
//             4 (172)                               712 (174)                      
     &Ppc405Iss::op_tw,                  _, &Ppc405Iss::op_subfze,                  _, 
//            20 (176)                                28 (178)                      
    &Ppc405Iss::op_lwarx,                  _, &Ppc405Iss::op_and,                  _, 
//                                                   744 (182)                      
                     _,                  _, &Ppc405Iss::op_subfme,                  _, 
//                                                    60 (186)                      
                     _,                  _, &Ppc405Iss::op_andc,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//           454 (196)           792 (197)                               971 (199)  
    &Ppc405Iss::op_dccci, &Ppc405Iss::op_sraw,                  _, &Ppc405Iss::op_divwu, 
//           470 (200)                               124 (202)                      
    &Ppc405Iss::op_dcbi,                  _, &Ppc405Iss::op_nor,                  _, 
//           486 (204)           824 (205)                              1003 (207)  
    &Ppc405Iss::op_dcread, &Ppc405Iss::op_srawi,                  _, &Ppc405Iss::op_divw, 
//                                                                       323 (211)  
                     _,                  _,                  _, &Ppc405Iss::op_mfdcr, 
//           522 (212)                                                   339 (215)  
    &Ppc405Iss::op_addc,                  _,                  _, &Ppc405Iss::op_mfspr, 
//                                                                        23 (219)  
                     _,                  _,                  _, &Ppc405Iss::op_lwzx, 
//                                                                       371 (223)  
                     _,                  _,                  _, &Ppc405Iss::op_mftb, 
//                                                                        55 (227)  
                     _,                  _,                  _, &Ppc405Iss::op_lwzux, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                        87 (235)  
                     _,                  _,                  _, &Ppc405Iss::op_lbzx, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//           284 (240)                               451 (242)           119 (243)  
    &Ppc405Iss::op_eqv,                  _, &Ppc405Iss::op_mtdcr, &Ppc405Iss::op_lbzux, 
//           459 (244)           650 (245)           467 (246)                      
    &Ppc405Iss::op_divwu, &Ppc405Iss::op_adde, &Ppc405Iss::op_mtspr,                  _, 
//           316 (248)                               151 (250)                      
    &Ppc405Iss::op_xor,                  _, &Ppc405Iss::op_stwx,                  _, 
//           491 (252)                                                              
    &Ppc405Iss::op_divw,                  _,                  _,                  _, 
//                                                   183 (258)            10 (259)  
                     _,                  _, &Ppc405Iss::op_stwux, &Ppc405Iss::op_addc, 
//                               714 (261)                                26 (263)  
                     _, &Ppc405Iss::op_addze,                  _, &Ppc405Iss::op_cntlzw, 
//                                                   215 (266)                      
                     _,                  _, &Ppc405Iss::op_stbx,                  _, 
//                               746 (269)                                          
                     _, &Ppc405Iss::op_addme,                  _,                  _, 
//                               412 (273)           247 (274)                      
                     _, &Ppc405Iss::op_orc, &Ppc405Iss::op_stbux,                  _, 
//                                                   778 (278)                      
                     _,                  _, &Ppc405Iss::op_add,                  _, 
//                               444 (281)                                          
                     _,  &Ppc405Iss::op_or,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                               476 (289)           138 (290)                      
                     _, &Ppc405Iss::op_nand, &Ppc405Iss::op_adde,                  _, 
//           146 (292)                                                              
    &Ppc405Iss::op_mtmsr,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                   202 (306)                      
                     _,                  _, &Ppc405Iss::op_addze,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                   234 (314)           922 (315)  
                     _,                  _, &Ppc405Iss::op_addme, &Ppc405Iss::op_extsh, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                       954 (323)  
                     _,                  _,                  _, &Ppc405Iss::op_extsb, 
//                               266 (325)                                          
                     _, &Ppc405Iss::op_add,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                          
                     _,                  _, 
};


void Ppc405Iss::op_op31()
{
     func_t func = op_op31_table[(0^(m_ins.x.func<<7)^(m_ins.x.func>>1)^(m_ins.x.func>>7)^(m_ins.x.func>>9))%342];
     (this->*func)();
}



// mod = 42
Ppc405Iss::func_t const Ppc405Iss::op_op4_table[42] = {
//             168 (0)             558 (1)               8 (2)             940 (3)  
    &Ppc405Iss::op_mulchw, &Ppc405Iss::op_nmachhw, &Ppc405Iss::op_mulhhwu, &Ppc405Iss::op_maclhw, 
//                                                     620 (6)             238 (7)  
                     _,                  _, &Ppc405Iss::op_machhws, &Ppc405Iss::op_nmacchws, 
//                                                     40 (10)            460 (11)  
                     _,                  _, &Ppc405Iss::op_mulhhw, &Ppc405Iss::op_maclhwsu, 
//            430 (12)                                140 (14)            392 (15)  
    &Ppc405Iss::op_nmaclhw,                  _, &Ppc405Iss::op_macchwu, &Ppc405Iss::op_mullhwu, 
//                                622 (17)                               1004 (19)  
                     _, &Ppc405Iss::op_nmachhws,                  _, &Ppc405Iss::op_maclhws, 
//                                                    172 (22)            424 (23)  
                     _,                  _, &Ppc405Iss::op_macchw, &Ppc405Iss::op_mullhw, 
//            524 (24)                                                              
    &Ppc405Iss::op_machhwu,                  _,                  _,                  _, 
//            494 (28)                                204 (30)                      
    &Ppc405Iss::op_nmaclhws,                  _, &Ppc405Iss::op_macchwsu,                  _, 
//             44 (32)            686 (33)            136 (34)                      
    &Ppc405Iss::op_machhw, &Ppc405Iss::op_nmacchw, &Ppc405Iss::op_mulchwu,                  _, 
//                                396 (37)            748 (38)                      
                     _, &Ppc405Iss::op_maclhwu, &Ppc405Iss::op_macchws,                  _, 
//             76 (40)                      
    &Ppc405Iss::op_machhwsu,                  _, 
};


void Ppc405Iss::op_op4()
{
     func_t func = op_op4_table[(0^(m_ins.x.func<<18)^(m_ins.x.func<<8)^(m_ins.x.func>>1)^(m_ins.x.func>>8))%42];
     (this->*func)();
}



// mod = 64
Ppc405Iss::func_t const Ppc405Iss::run_table[64] = {
//               0 (0)                                                       3 (3)  
    &Ppc405Iss::op_ill,                  _,                  _, &Ppc405Iss::op_twi, 
//               4 (4)                                                       7 (7)  
    &Ppc405Iss::op_op4,                  _,                  _, &Ppc405Iss::op_mulli, 
//               8 (8)                                 10 (10)             11 (11)  
    &Ppc405Iss::op_subfic,                  _, &Ppc405Iss::op_cmpli, &Ppc405Iss::op_cmpi, 
//             12 (12)             13 (13)             14 (14)             15 (15)  
    &Ppc405Iss::op_addic, &Ppc405Iss::op_addic_, &Ppc405Iss::op_addi, &Ppc405Iss::op_addis, 
//             16 (16)             17 (17)             18 (18)             19 (19)  
     &Ppc405Iss::op_bc,  &Ppc405Iss::op_sc,   &Ppc405Iss::op_b, &Ppc405Iss::op_op19, 
//             20 (20)             21 (21)                                 23 (23)  
    &Ppc405Iss::op_rlwimi, &Ppc405Iss::op_rlwinm,                  _, &Ppc405Iss::op_rlwnm, 
//             24 (24)             25 (25)             26 (26)             27 (27)  
    &Ppc405Iss::op_ori, &Ppc405Iss::op_oris, &Ppc405Iss::op_xori, &Ppc405Iss::op_xoris, 
//             28 (28)             29 (29)                                 31 (31)  
    &Ppc405Iss::op_andi, &Ppc405Iss::op_andis,                  _, &Ppc405Iss::op_op31, 
//             32 (32)             33 (33)             34 (34)             35 (35)  
    &Ppc405Iss::op_lwz, &Ppc405Iss::op_lwzu, &Ppc405Iss::op_lbz, &Ppc405Iss::op_lbzu, 
//             36 (36)             37 (37)             38 (38)             39 (39)  
    &Ppc405Iss::op_stw, &Ppc405Iss::op_stwu, &Ppc405Iss::op_stb, &Ppc405Iss::op_stbu, 
//             40 (40)             41 (41)             42 (42)             43 (43)  
    &Ppc405Iss::op_lhz, &Ppc405Iss::op_lhzu, &Ppc405Iss::op_lha, &Ppc405Iss::op_lhau, 
//             44 (44)             45 (45)             46 (46)             47 (47)  
    &Ppc405Iss::op_sth, &Ppc405Iss::op_sthu, &Ppc405Iss::op_lmw, &Ppc405Iss::op_stmw, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//                                                                                  
                     _,                  _,                  _,                  _, 
//  
    
};


void Ppc405Iss::run()
{
     func_t func = run_table[0^m_ins.d.op];
     (this->*func)();
}


#undef _

}}
