/* -*- c++ -*-
 *
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
 * Copyright (c) INPG/GIPSA-Lab
 *         Dominique Houzet <houzet@lis.inpg.fr>, 2008
 *
 * 
 */

#include <systemc.h>
#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

#define vendev 25 //"00010000111010000001001000110100";
   #define status 1056 //std_logic_vector(15 downto 0):="0000010000100000";

   #define confw "1011"  
   #define confr "1010" 
   #define memr "0110" 
   #define memw "0111" 
   #define rdline "1100"  
   #define ior 2 
   #define iow 3 



namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciPci
	: public caba::BaseModule
{
private:
    soclib::caba::VciTargetFsm<vci_param, true> m_vci_fsm;

    bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
    bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
    void transition();
    void genMoore();
    void target();
    void master();
    void combinatoire();
    void retard();
    void transitionFIFO();
    
	size_t m_badr;

    sc_signal<typename vci_param::data_t> *r_valueW;   
    sc_signal<typename vci_param::data_t> *r_valueR;
    unsigned int r_pagein2;
    unsigned int r_nbbloc;
    unsigned int r_mode;
    bool r_irq;
    unsigned int  r_ptwrW ;
    unsigned int  r_ptrdW ;
    unsigned int  r_ptwrR ;
    unsigned int  r_ptrdR ;
bool r_videW;
bool r_pleinW;
bool r_videR;
bool r_pleinR;

//FIFO variables
			sc_signal<bool> r_wrexto	   ;		//-- ordre ecriture banc externe ('0')
   			//sc_signal<bool> wrexti     ;		//-- ordre ecriture banc interne ('0')
   			sc_uint<32>   fifoin      ;	//-- donnee lue du banc externe ou a ecrite dans banc interne
   			sc_lv<32>  busrext	;
   			sc_signal<sc_uint<16> >  adrexto	   ;	//-- No de registre
   			//sc_signal<sc_uint<16> >  adrexti	  ;
               sc_signal<bool> enabledmawr;
               sc_signal<bool> enabledma;
           sc_signal<bool> debframe;
           
           sc_signal<bool> dmar;
            sc_signal<bool> dmaw;
           //  sc_signal<bool> p_vide;
            // sc_signal<int >  p_pagein2	;
          // sc_signal<int>  p_nbbloc	;

	 // PCI Variable Declarations
     sc_signal<bool> enad32b;
    sc_signal<bool> enad32;
   sc_signal<bool> r_rdbanc;
   sc_signal<bool> r_conf;
   bool debframe2;
      sc_signal<bool> r_regstop 	;
      sc_signal<bool> r_stopout;
      sc_signal<bool>  r_regtrdy ;
      sc_signal<bool> r_trdyout;
      sc_signal<bool> r_trdyen;
      sc_signal<bool>  r_frameregin ;
      sc_signal<bool>  r_irdymem ;
    sc_signal<bool>   r_gntreg;
     sc_signal<bool>   r_framereginpred;
    sc_signal<bool>    r_cmdframereg;
   sc_signal<bool>  r_tcmdframe;
   sc_signal<bool>     r_idselreg ;
   sc_signal<bool>     r_devselreg;
   sc_signal<bool> r_devseli;
   sc_uint<3>    r_cmd;
   // sc_signal<sc_uint<4>>    cbereg;
   sc_lv<4>    r_cber;
   sc_lv<4>    r_cbeout;
   sc_uint<8>    r_vect;
   sc_signal<unsigned int>    r_badr0;
   sc_signal<bool> 	 r_adrenpred;
   sc_signal<bool> 	 r_adren;
   sc_signal<bool> 	 r_adrenpred2;
   sc_signal<bool> 	 r_irdyreg;
   sc_signal<bool>  r_framereg;
    sc_signal<bool>    TRDYbis ;
   sc_uint<32> r_AD32out ;
   sc_uint<32>  datain;
   sc_lv<32>  r_fifoout;
   sc_signal<bool> 	r_PARout;
    sc_logic     partmp;
    sc_signal<bool> condwhile;
    sc_signal<bool> r_rw;
    sc_signal<bool> r_paren;
   sc_uint<32>  pcibusconf ;
   int adrbanc ;
    sc_uint<16> r_adrmem ;
   //sc_uint<32> r_bancdma[6];
   int r_wb;
    bool r_burst2;
    
     sc_signal<bool> r_trdyen_dd;
    sc_signal<bool> r_paren_dd;
    sc_signal<bool> enad32_dd;
    sc_signal<bool> enad32b_dd;
    sc_signal<bool> r_cmdframereg_dd;
    sc_signal<bool> r_tcmdframe_dd;
    
     enum vci_target_fsm_state_e {
         IDLE_TARGET,
         WAIT_TRANSACTION,
         RESPOND,
         END_TRANSACTION
     };
    vci_target_fsm_state_e r_target_fsm;
    
     enum vci_master_fsm_state_e {
       IDLE_MASTER,WAITDMA,STARTDMA,DMA_ADRWAIT,DMA_ADRPAGE,DATAOK,
				     WAIT_GNT,DMA_CYCLE_ADR,DMA_CYCLE_DATA1,DMA_CYCLE_DATA2,
				     DMA_LAST_DATA,FINDMA 
     };
       vci_master_fsm_state_e r_master_fsm;
       
      sc_signal<bool> r_memrd;
    sc_signal<bool> r_dmard;
    sc_signal<bool> r_dmawr;
   sc_signal<bool> r_flag;
   sc_signal<bool> r_reqi;
   sc_signal<unsigned int> r_nbval;
   sc_signal<unsigned int> r_timeout;
   sc_signal<unsigned int> r_sauvreg;
   sc_signal<bool> r_memstop;
   sc_signal<bool> dmaok;
   sc_signal<bool> r_it;
   sc_signal<unsigned int> r_memadrdma;
   sc_signal<unsigned int> r_adrdma;
   sc_signal<unsigned int> r_adrmempages;
  sc_signal<bool> r_dataen;
   sc_lv<4>    r_cberegv;
  
    // Activity counters
    uint32_t m_cpt_read;   // Count READ access
    uint32_t m_cpt_write;  // Count WRITE access

protected:
    SC_HAS_PROCESS(VciPci);

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciTarget<vci_param> p_vci;
    sc_out<bool> p_irq;
    	sc_inout<sc_lv<4> >  p_Cbe     ;
   			sc_in_clk p_clkpci       ;
   			sc_in<bool> p_Sysrst    ;
   			sc_in<bool> p_Idsel    ;
   			sc_inout<sc_logic> p_Frame   ;
   			sc_inout<sc_logic> p_Devsel  ;
   			sc_inout<sc_logic> p_Irdy    ;
   			sc_in<bool> p_Gnt    ;
   			sc_inout<sc_logic> p_Trdy   ;  
   			sc_inout<sc_logic> p_Inta    ;
   			sc_inout<sc_logic> p_Stop    ;
   			sc_out<bool> p_Req       ;
   			sc_inout<sc_logic> p_Par      ;
   			sc_inout<sc_lv<32> >  p_AD32       ;

	VciPci(
		sc_module_name name,
		const soclib::common::IntTab &index,
		const soclib::common::MappingTable &mt,
        size_t badrval);
};

}}



// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

