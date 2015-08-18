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
 */

#include "../../include/soclib/pci.h"
#include "register.h"
#include "../include/vci_pci.h"

#include <iostream>

namespace soclib { namespace caba {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciPci<vci_param>

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    int cell = (int)addr / vci_param::B;
	int reg = cell ;
	

	switch (reg) {
	case PCI_VALUE:
		if(r_pleinW==false) { 
		r_valueW[r_ptwrW] = data; 
		r_ptwrW=(r_ptwrW+1)%1024; 
		if (r_ptwrW==r_ptrdW) 
		r_pleinW=true; 
		else r_videW=false; }
		//std::cout << "write "  <<  data <<std::endl; 
		break;

	case PCI_RESETIRQ:   
		r_irq = false;
		break;

	case PCI_MODE:
		r_mode = (int)data ;
		break;

	case PCI_ADR:
		r_pagein2= (int) data;  //adresse
		break;
		
	case PCI_NB:
		r_nbbloc = (int) data;
		break;
	}
    m_cpt_write++;
	return true;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{
    int cell = (int)addr / vci_param::B;
	int reg = cell ;


	switch (reg) {
	case PCI_VALUE:
		data = r_valueR[r_ptrdR].read(); 
		if(r_videR==false) { 
		r_ptrdR=(r_ptrdR+1)%1024; 
		if (r_ptwrR==r_ptrdR) 
		r_videR=true; else r_pleinR=false;
		//std::cout << "read "  <<  data <<std::endl; 
		}
		break;

	case PCI_NB:
		data = r_nbbloc;
		break;
		
	case PCI_ADR:
		data= r_pagein2;  //adresse
		break;
		
	case PCI_MODE:
		data = r_mode; //r_ptrdR;
		break;

	case PCI_RESETIRQ:
		data = r_irq ; //r_nbbloc;
		break;
	}
    m_cpt_read++;

	return true;
}

tmpl(void)::transition()
{
	if (!p_resetn) {
	
   
		m_vci_fsm.reset();
        m_cpt_read = 0;
        m_cpt_write = 0; 

		//for (size_t i = 0 ; i < 1024 ; i++) r_value[i] = 0;
		r_ptrdW=0; r_ptwrW=0;
		r_ptrdR=0; r_ptwrR=0;
			r_pagein2 = m_badr;
			r_nbbloc = 16;
			r_mode = PCI_DMA_LOOPBACK;
			r_irq = false;
			r_videW=true;
			r_pleinW=false;
			r_videR=true;
			r_pleinR=false;
		
		return;
	}

	m_vci_fsm.transition();



		//r_value[i] = r_value[i].read() + 1;
if ((r_videW)&&(r_mode != PCI_DMA_LOOPBACK)) { r_nbbloc=0;
                      // std::cout << "irqactive "  << std::endl;
		}
		
	
}

tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();
	
	dmaw=(((r_mode == PCI_DMA_WRITE_NO_IRQ) 
	 || (r_mode == PCI_DMA_WRITE_IRQ) )==true);
	dmar=(((r_mode == PCI_DMA_READ_NO_IRQ) 
	 || (r_mode == PCI_DMA_READ_IRQ)  || (r_mode == PCI_DMA_LOOPBACK)
	  )==true);
	 
	 p_irq = ((r_irq && 
	 ((r_mode == PCI_DMA_READ_IRQ) 
	 || (r_mode == PCI_DMA_WRITE_IRQ) ))==true);  

}

tmpl(void)::transitionFIFO()
{
	if ( enabledma == true) { ///TIMEOUT!!!
		// std::cout << "enabledma "  << r_valueW[r_ptrdW].read() << ' '<< r_ptrdW << std::endl;
		if(r_videW==false) { 
		r_ptrdW=(r_ptrdW+1)%1024; 
		if (r_ptwrW==r_ptrdW) 
		r_videW=true; else r_pleinW=false;
		}
		}
		
	if (( enabledmawr == true) && (r_mode != PCI_DMA_LOOPBACK))  { 	
		if(r_pleinR==false) { 
		r_valueR[r_ptwrR] = busrext; 
		r_ptwrR=(r_ptwrR+1)%1024; 
		if (r_ptwrR==r_ptrdR) 
		r_pleinR=true; 
		else r_videR=false; }
		}
		
	if (( enabledmawr == true) && (r_mode == PCI_DMA_LOOPBACK))  { 
		//std::cout << "loopback "  << r_ptwrW <<r_videW<<r_nbbloc<<std::endl;
		if(r_pleinW==false) { 
		r_valueW[r_ptwrW] = busrext; 
		r_ptwrW=(r_ptwrW+1)%1024; 
		if (r_ptwrW==r_ptrdW) 
		r_pleinW=true; 
		else r_videW=false; }	
		}
}

   tmpl(void)::target(void) {
   
  
   
   if (p_Sysrst==false) { r_wb=0; r_target_fsm=IDLE_TARGET; 
   p_AD32=(sc_lv<32>(SC_LOGIC_Z));
   p_Trdy=SC_LOGIC_Z; p_Devsel=SC_LOGIC_Z; p_Stop=SC_LOGIC_Z;
   r_badr0 =sc_uint<25>( int(m_badr)); //std::cout << "badr "  << int(m_badr) <<std::endl;
   } else
   if (r_target_fsm==IDLE_TARGET) {
     r_rdbanc =false;    
     r_conf  =false;
     r_wrexto =false;
     p_Devsel=SC_LOGIC_Z; //r_devseli  =true;
     p_Trdy=SC_LOGIC_Z; p_Stop=SC_LOGIC_Z;  //r_trdyout  =true; 	
     r_burst2=false;
   p_Stop=SC_LOGIC_Z; //r_stopout  =true;
   r_trdyen  =false;	 
   r_target_fsm=WAIT_TRANSACTION;}
  
   else if (r_target_fsm==WAIT_TRANSACTION) { 
   r_trdyen =false;
 
   p_Trdy=SC_LOGIC_Z;p_Stop=SC_LOGIC_Z;
   r_wrexto =false; p_Devsel=SC_LOGIC_Z;
   if (condwhile) { r_target_fsm=RESPOND; 
   r_adrmem = (int(sc_uint<32>(r_fifoout)) /4) % 32;  //.range(6,2);
    p_Devsel=sc_logic('0'); p_Trdy=sc_logic('1'); p_Stop=sc_logic('1');//r_trdyen =true; r_devseli  =false;
     r_rw=(r_cberegv==confw); 
     r_cber=r_cberegv;
   if (r_wb!=2) { r_badr0=sc_uint<25>( int(m_badr)); 
   //65535*512+511;  
   r_vect=0; }
     
   if (r_cberegv==confr)   r_conf  =true;
   else if ((r_cberegv==memr)||(r_cberegv==rdline)) r_rdbanc =true; 
    }
   }
  
   else if (r_target_fsm==RESPOND) { 
   if (r_rdbanc || r_conf)  p_AD32=(sc_lv<32>(datain));
      p_Trdy=sc_logic('0'); 
      p_Devsel=sc_logic('0'); p_Stop=sc_logic('1');//r_trdyout =false;  
   
   if (p_Frame=='0')  r_burst2=true; else {  if (r_burst2) {  //r_devseli  =true;   	 
                                           //r_trdyout  =true;  r_stopout  =true; 
                                           p_Trdy=sc_logic('1'); p_Devsel=sc_logic('1');
                                           p_AD32=(sc_lv<32>(SC_LOGIC_Z));
                                           r_target_fsm=IDLE_TARGET; }
                                           else { p_Stop=sc_logic('0'); //r_stopout =false;  
                                           r_target_fsm=END_TRANSACTION; }
   } 
   }
   else if (r_target_fsm==END_TRANSACTION) { 
   			 

   if (r_cber==memw )  r_wrexto =true;  // && adrmem>6
   else if (r_rw && r_adrmem==4)  {   r_badr0 =sc_uint<25>( int(sc_uint<32>(r_fifoout))/128); //.range(31 ,7);   
   r_wb=2;  }
   else if (r_rw && r_adrmem==15)  	r_vect = sc_uint<8>(int(sc_uint<32>(r_fifoout))%256); //(7,0); 
   else if (r_rw && r_adrmem==1)   r_cmd=sc_uint<3>(int(sc_uint<32>(r_fifoout))%8); //.range(2,0); 
   
   
   r_conf  =false; 
       
 
   r_rdbanc =false; r_wrexto =false;
   p_Trdy=sc_logic('1'); p_Devsel=sc_logic('1');p_AD32=(sc_lv<32>(SC_LOGIC_Z));
                                           p_Stop=sc_logic('1'); //r_devseli  =true;   	 

   r_target_fsm=IDLE_TARGET; }
   }
   
   
  tmpl(void)::combinatoire(void) {
     int k;
     sc_lv<4> cberegp; sc_lv<32> AD32p;
 fifoin=r_valueW[r_ptrdW].read();
   
   debframe2 = r_frameregin==true && r_framereginpred==false ; 
    condwhile= ( p_Sysrst==false || (debframe2 &&  (( r_idselreg && (r_cberegv==confw ||
    r_cberegv==confr)) ||
      ((r_cberegv==memr || r_cberegv==memw || r_cberegv==rdline) &&
      ((((int(sc_uint<32>(r_fifoout))/4096)==r_badr0))  ))))	) ;
   adrexto=r_adrmem;
   if (debframe2 && (r_cberegv==confr  || r_cberegv==rdline ||
   r_cberegv==memr  ||
   r_cberegv==memw   || r_cberegv==confw)) 
   	adrbanc = (int(sc_uint<32>(r_fifoout))/4)%32; //.range(6,2) ;
   else 	adrbanc = (int) r_adrmem ;
   if (r_adrmem==1 )	pcibusconf =    41*65536*16+r_cmd ; else 
   if (r_adrmem==4 )	pcibusconf =    r_badr0*4096 +0; else 
   if (r_adrmem==2 )	pcibusconf =    1+16+4*256*65536;         else 
   if (r_adrmem==0 || r_adrmem==11 )	pcibusconf =    vendev ; else 
   if (r_adrmem==15 )	pcibusconf =    65535*65536 + 256 + r_vect ; else 
   			pcibusconf =    0;
   
   enabledmawr=((r_regtrdy || r_dmard || r_irdymem  ||  r_rdbanc || r_idselreg )==false) ;
   enabledma=(((debframe2 && (((r_cberegv==rdline || r_cberegv==memr)&& !r_dmawr  &&!r_dmard) || r_dmard) ) || 
   ((r_rdbanc || r_dmard ) && (p_Trdy=='0') && (p_Irdy=='0')&& (p_Frame=='0')))==true);
   enad32=(r_conf || (r_rdbanc && r_frameregin)); // || r_adren || r_adrenpred); // || (r_dataen && p_dmar))  ;
   enad32b= (  r_adren || r_dataen)  ;
   cberegp=p_Cbe;
   AD32p=p_AD32;
    partmp=sc_logic('0');
    for (k=0; k<32; k++)  partmp= (partmp ^ (AD32p[k])) ; //-- parité
    for (k=0; k<4; k++)  partmp= (partmp ^ (cberegp[k])) ; //-- parité
   //if (wrexto ) 
   busrext=(r_fifoout);  //else busrext=bancdma[adrexti];

  
   TRDYbis= //adren || adrenpred || adrenpred2 || (dataen &&  trdyi==false) || 
   r_rdbanc || r_conf;
   if (r_conf) datain=pcibusconf;  
   else if  (r_adren ) datain=r_adrdma;
   else if 	(r_adrenpred && r_dmawr) datain=0;
 
   else if (r_memstop) datain=r_sauvreg; 
   else datain=fifoin; 
   p_Inta=SC_LOGIC_Z;
   debframe=debframe2;
   p_Req=r_reqi;
   dmaok = // r_rdbanc && !r_adrmem	&& 
   (dmar || dmaw);
   

   }
   
   
   
  tmpl(void)::retard(void) {
				

       r_regstop = p_Stop.read()!='0';	//-- stop retardé
       r_regtrdy = p_Trdy.read()!='0';
      r_framereginpred=r_frameregin;
       r_frameregin = p_Frame.read()=='0';
       r_irdymem = p_Irdy.read()!='0';
     // if (p_AD32[0]==sc_logic('0') || p_AD32[0]==sc_logic('1')) 
      r_fifoout = (p_AD32);
       r_gntreg=p_Gnt.read();
       r_cmdframereg= r_tcmdframe;
       if (p_Irdy!='0') r_idselreg = p_Idsel.read();
       r_devselreg=p_Devsel.read()!='0';
      
       r_cberegv=(p_Cbe.read());
   	 r_adrenpred2=r_adrenpred; 
   	 	 r_adrenpred=	 r_adren;
   	 r_irdyreg = r_framereg;
   		if (r_paren && (enad32 || enad32b)) p_Par=partmp; else p_Par=SC_LOGIC_Z;

   	 r_paren=enad32 || enad32b;
   	 
   	  if (p_Sysrst.read()==false) { r_paren=false; 	 r_adrenpred2=false; 
   	 	 r_adrenpred=	false; }
   
   }
   
   
   tmpl(void)::master(void) {
   
   if (p_Sysrst.read()==false) { r_master_fsm=IDLE_MASTER;    r_framereg= true;  r_tcmdframe=false; r_memstop=false;
                            p_Irdy=sc_logic('Z'); p_Frame=sc_logic('Z'); p_Cbe=(sc_lv<4>(SC_LOGIC_Z));p_AD32=(sc_lv<32>(SC_LOGIC_Z));
                          r_reqi=true;	 r_dmard= false; r_dmawr= false;	
   r_dataen=false; r_adren=false; r_irq=false; r_adrmempages=0; }
   else
   if (r_master_fsm==IDLE_MASTER) {
	   r_framereg= true;  r_tcmdframe=false; r_memstop=false;
	   r_reqi=true;	 r_dmard= false; r_dmawr= false;	r_dataen=false; r_adren=false;  
	   if (!dmaok)  r_master_fsm=WAITDMA; else 
	   {  r_it=false; 
		   ////adrmempages= bancdma(2)(18 downto 0) ;
	   r_master_fsm = STARTDMA; } }
   else if (r_master_fsm==WAITDMA) { if (dmaok) { 
		   r_it=false; 
		   ////adrmempages<= bancdma(2)(18 downto 0) ;
		   r_adrmempages=1;
		   r_master_fsm = STARTDMA; }}
   else if (r_master_fsm== STARTDMA) { 
		   r_master_fsm = DMA_ADRWAIT; }
   else if (r_master_fsm== DMA_ADRWAIT) r_master_fsm=DMA_ADRPAGE;
   else if (r_master_fsm== DMA_ADRPAGE) {
	   if (r_adrmempages != 0)  
		  { if ((r_videW==false)&& (r_nbbloc>4)) { r_nbval= r_nbbloc/4-1  ;	
			   r_master_fsm = DATAOK; } }
		   else  { r_irq=true;	p_Frame=sc_logic('Z');
			   r_master_fsm= FINDMA; } }
	   
   
   else if (r_master_fsm== DATAOK ) { r_adrdma=r_pagein2*4096; 
 	   r_memadrdma=r_pagein2*4096; r_flag=false;
	   if (r_nbval.read()>1 ){ r_reqi=false; 
			   r_master_fsm=WAIT_GNT; 
			  // std::cout << "waitgnt " << r_nbval.read() << r_nbbloc << std::endl;
			   }
	   else {
 	 	   r_reqi=true;
		   r_adrmempages=r_adrmempages-1;
		   r_master_fsm=DMA_ADRPAGE;
	   } }
   else if (r_master_fsm== WAIT_GNT){ if ((r_gntreg==false) && (r_target_fsm!=RESPOND)) {
	   //r_framereg = false; 
	   r_adren=true;	r_tcmdframe=true; r_dmard = dmar; r_dmawr= dmaw;
	   r_master_fsm=DMA_CYCLE_ADR;	
	   } }
   else if (r_master_fsm== DMA_CYCLE_ADR) { r_timeout=0;	 p_Frame=sc_logic('0'); 
   r_adren=false; r_dataen= dmar;	
   if (r_dmawr)  	p_Cbe = memr; 
	  	else if (r_dmard)  p_Cbe= memw; 
   p_AD32=sc_lv<32>(datain); // std::cout << "datain " << (int)datain << std::endl;
		   r_master_fsm = DMA_CYCLE_DATA1; }
   else if ((r_master_fsm== DMA_CYCLE_DATA1) || (r_master_fsm==DMA_CYCLE_DATA2)) { 
          p_Irdy=sc_logic('0'); p_Cbe=0; if (r_dataen && enabledma) p_AD32=sc_lv<32>(datain);
	  else if ((r_master_fsm== DMA_CYCLE_DATA1) && r_dataen) p_AD32=sc_lv<32>(0);  
	  // std::cout << "datain " << (int)datain << ' ' << r_valueW[r_ptrdW].read() << ' '<< r_ptrdW << std::endl;
          p_Frame=sc_logic('0'); 
		   if (r_master_fsm==DMA_CYCLE_DATA1) { if (r_nbval.read()==2)  r_flag=true;  }
		   else r_memstop=false; 
		   if (r_timeout<16 && p_Stop!='0'  &&	(r_nbval.read()>2 || r_flag) ) {
				    r_timeout= r_timeout +1;
				   if (r_regtrdy==false) { 
				 		   r_nbval=r_nbval.read()-1; r_timeout=0; r_adrdma= r_adrdma+4; r_flag=false;
				   }
				   r_master_fsm = DMA_CYCLE_DATA2; }
		   else {
			   p_Frame=sc_logic('1'); //r_framereg	 = true;  
			   if (r_regtrdy==false) {		  // derniere donnee
				   r_nbval=r_nbval.read()-1; r_timeout=0; r_adrdma= r_adrdma+4; 
			   }
			   r_tcmdframe=false;
			   r_master_fsm = DMA_LAST_DATA;
		   } }
   else if (r_master_fsm== DMA_LAST_DATA) { p_Irdy=sc_logic('1'); p_Cbe=(sc_lv<4>(SC_LOGIC_Z));
   p_AD32=(sc_lv<32>(SC_LOGIC_Z));
          if (r_regtrdy==false) { 
			   r_nbval=r_nbval.read()-1;
			   r_timeout=0; 
			   r_adrdma= r_adrdma+8; 
			   if (r_regstop==false)  r_nbval=r_nbval.read()-2;  }
		   else if (r_regstop==false) {  r_memstop=true; r_sauvreg=  r_AD32out; }
	   r_dataen=false; 
	   if (r_nbval.read()>1) { r_reqi = false; 
		   r_master_fsm = WAIT_GNT; }
	   else {
		   r_reqi = true; 
		   r_adrmempages=r_adrmempages-1;
		   r_master_fsm = DMA_ADRPAGE;
	   } }
				   
   else { p_Irdy=sc_logic('Z'); p_Cbe=(sc_lv<4>(SC_LOGIC_Z)); p_AD32=(sc_lv<32>(SC_LOGIC_Z)); r_master_fsm=IDLE_MASTER; }
   
   }
   
   


tmpl(/**/)::VciPci(
    sc_module_name name,
    const IntTab &index,
    const MappingTable &mt,
    size_t badrval)
	: caba::BaseModule(name),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      m_badr(badrval),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),
       p_Cbe("Cbe")    ,
   			 p_clkpci("clkpci")      ,
   			 p_Sysrst("Sysrst")    ,
   			p_Idsel("Idsel")    ,
   			 p_Frame("Frame")   ,
   			 p_Devsel("Devsel")  ,
   			 p_Irdy("Irdy")    ,
   			 p_Gnt("Gnt")    ,
   			 p_Trdy("Trdy")  ,  
   			 p_Inta("Inta")   ,
   			 p_Stop("Stop")    ,
   			 p_Req("Req")       ,
   			 p_Par("Par")      ,
   			 p_AD32("AD32")       
{
	m_vci_fsm.on_read_write(on_read, on_write);

	r_valueW   = new sc_signal<typename vci_param::data_t>[1024];
	r_valueR   = new sc_signal<typename vci_param::data_t>[1024];

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
	SC_METHOD(transitionFIFO);
	dont_initialize();
	sensitive << p_clkpci.pos();
	   SC_METHOD(master);
           sensitive << p_clkpci.pos();
           SC_METHOD(target);
           sensitive << p_clkpci.pos();
           SC_METHOD(retard);
           sensitive << p_clkpci.pos();
           SC_METHOD(combinatoire);
	   dont_initialize();
           sensitive << p_clkpci.neg();      
}

}}

