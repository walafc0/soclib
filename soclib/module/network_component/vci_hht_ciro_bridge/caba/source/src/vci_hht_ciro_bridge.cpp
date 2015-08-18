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
 * Copyright (c) UPMC, Lip6, SoC
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */
 
#include "vci_hht_ciro_bridge.h"
namespace soclib { 
namespace caba {

#define READPASSWRITES 0
#define VCI_READOK 0				// Successful read error code
#define VCI_WRITEOK 2				// Successful write error code
#define tmpl(x)  template<typename vci_param, typename hht_param> x VciHhtCiroBridge<vci_param, hht_param>

tmpl(/**/)::VciHhtCiroBridge(
    sc_module_name name )
    : soclib::caba::BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn"),
	  
      p_vci_io("vci_io"),
	  p_vci_config("vci_config"),
	  p_hht("p_hht"),
	  
	  f_vciCI("f_vciCI",2),	  
	  f_vciRO("f_vciRO",2),
	  
	  f_ROtoCI("f_ROtoCI",nb_atids),
	  f_CItoRO("f_CItoRO",2),
	  f_vciids("f_vciids",nb_ids),
	  f_atids("f_atids",nb_atids),
	  
	  f_ctrlPCI("f_ctrlPCI",2),
	  f_ctrlNPCI("f_ctrlNPCI",2),
	  f_dataPCI("f_dataPCI",17),
	  f_dataNPCI("f_dataNPCI",17),
	  f_ctrlRO("f_ctrlRO",2),
	  f_dataRO("f_dataRO",17)
	  
{
    
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

}

tmpl(/**/)::~VciHhtCiroBridge()
{
	
}

tmpl(void)::transition()
{
	
    if ( !p_resetn.read() ) {
		r_hhtci_fsm = HHTCI_RESET;
		r_hhtro_fsm = HHTRO_RESET;
		
		f_vciCI.init();
        f_vciRO.init();
        
        f_ROtoCI.init();
		f_CItoRO.init();
		f_vciids.init();
        f_atids.init();
        
		f_ctrlPCI.init();
        f_ctrlNPCI.init();
        f_dataPCI.init();
        f_dataNPCI.init();
        f_ctrlRO.init();
        f_dataRO.init();
        return;
    }
	/**********************************************************************************************
	*** BLOCK TRANSLATE COMMAND IN : HHTCI
	**********************************************************************************************/
	
	// Copy locally the outputs of the fifos
	HhtCmdFlit<hht_param> 	f_ctrlNPCI_read =	f_ctrlNPCI.read();
	HhtCmdFlit<hht_param> 	f_ctrlPCI_read =	f_ctrlPCI.read();
	// Link fifos' fields not depending on the FSM state
	f_vciCI.write.contig=0;
	f_vciCI.write.cons=0;
	f_vciCI.write.wrap=0;
	f_vciCI.write.cfixed=0;
	f_vciCI.write.clen=0;
	
	// Pointers to the active input fifos
	LazyFifo<HhtCmdFlit<hht_param> > *f_ctrlPNPCI;
	LazyFifo<typename hht_param::data_t> *f_dataPNPCI;
	HhtCmdFlit<hht_param> f_ctrlPNPCI_read;
	switch (r_hhtci_fsm.read()) {
    case HHTCI_RESET:
		r_hhtci_fsm=HHTCI_IDLE;
		break;
	case HHTCI_IDLE:
		if (f_vciCI.wok() && f_ROtoCI.rok()){	// Sends LL and SC from the block RO
			f_ROtoCI.get=true;
			f_vciCI.write=f_ROtoCI.read();
			f_vciCI.put=true;
		}else if (f_vciCI.wok() && f_vciids.rok() && f_atids.rok()){
			bool b_posted_write=false;
			if (f_ctrlPCI.rok()){ // PCI Channel
				if ((f_ctrlPCI_read.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE){			// Posted write
					if (f_dataPCI.rok())
						b_posted_write=true;
				}else{
					switch (f_ctrlPCI_read.cmd){
					case hht_param::CMD_FENCE:
					case hht_param::CMD_BROADCAST:
						f_ctrlPCI.get=true;
						break;
					case hht_param::CMD_ADDREXT:
						f_ctrlPCI.get=true;
						printf("Unhandled command in channel PCI: ");
						f_ctrlPCI_read.print(true);
						printf("\n");
						break;
					default:
						f_ctrlPCI.get=true;
						printf("Unknown command in channel PCI: ");
						f_ctrlPCI_read.print(true);
						printf("\n");
						break;
					}
				}
			}
			if (b_posted_write){
				f_ctrlPNPCI=&f_ctrlPCI;
				f_dataPNPCI=&f_dataPCI;
				f_ctrlPNPCI_read=f_ctrlPCI_read;
			}else{	
				f_ctrlPNPCI=&f_ctrlNPCI;
				f_dataPNPCI=&f_dataNPCI;
				f_ctrlPNPCI_read=f_ctrlNPCI_read;
			}
			
			if (f_ctrlPNPCI->rok()){
				if ((f_ctrlPNPCI_read.cmd & hht_param::CMD_READ_MASK)==hht_param::CMD_READ){					// Read
					f_vciCI.write.srcid=f_vciids.read();
					f_vciCI.write.pktid=f_ctrlPNPCI_read.srctag % 2;
					f_vciCI.write.trdid=(f_ctrlPNPCI_read.srctag/2) % 2;

					f_ctrlPNPCI->get=true;
					f_vciids.get=true;
					f_vciCI.write.cmd=vci_param::CMD_READ;
					f_vciCI.write.address=f_ctrlPNPCI_read.addr;
					f_vciCI.write.eop=true;
					if (f_ctrlPNPCI_read.cmd & hht_param::CMD_DWORD_FLAG){					// Non-masked read
						f_vciCI.write.be=0xF;
						f_vciCI.write.plen=(f_ctrlPNPCI_read.mskcnt+1)*4;
					}else{																	// Masked read
						f_vciCI.write.be=f_ctrlPNPCI_read.mskcnt;
						f_vciCI.write.plen=4;
					}
					r_ctrl_table[f_vciids.read()]=f_ctrlPNPCI_read.get_ctrl();	
					f_vciCI.put=true;
					
				}else if ((f_ctrlPNPCI_read.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE){			// Write
					f_vciCI.write.srcid=f_vciids.read();
					f_vciCI.write.pktid=f_ctrlPNPCI_read.srctag % 2;
					f_vciCI.write.trdid=(f_ctrlPNPCI_read.srctag/2) % 2;

					if (f_dataPNPCI->rok()){
						f_dataPNPCI->get=true;
						if (f_ctrlPNPCI_read.cmd & hht_param::CMD_DWORD_FLAG){				// Non-masked write
							if (f_ctrlPNPCI_read.mskcnt==0){		// Sending in 1 clock cycle
								f_ctrlPNPCI->get=true;
								f_vciids.get=true;
								f_vciCI.write.eop=true;
							}else{									// Start sending in several cycles
								f_vciCI.write.eop=false;
								r_hhtci_mask=0xFFFFFFFF;
								r_hhtci_dec=f_ctrlPNPCI_read.mskcnt-1;
								r_hhtci_addr=(f_ctrlPNPCI_read.addr+4);
								r_hhtci_fsm=b_posted_write?HHTCI_WPOSTED:HHTCI_WNPOSTED;
							}
							f_vciCI.write.cmd=vci_param::CMD_WRITE;
							f_vciCI.write.be=0xF;
							f_vciCI.write.address=f_ctrlPNPCI_read.addr;
							f_vciCI.write.plen=(f_ctrlPNPCI_read.mskcnt+1)*4;
							f_vciCI.write.wdata=f_dataPNPCI->read();
							f_vciCI.put=true;
						}else{																				// Masked write
							r_hhtci_mask=f_dataPNPCI->read();
							r_hhtci_addr=f_ctrlPNPCI_read.addr;
							r_hhtci_dec=f_ctrlPNPCI_read.mskcnt-1;
							r_hhtci_fsm=b_posted_write?HHTCI_WPOSTED:HHTCI_WNPOSTED;
						}
						r_ctrl_table[f_vciids.read()]=f_ctrlPNPCI_read.get_ctrl();	
					}
				}else{
					switch (f_ctrlPNPCI_read.cmd){
					case hht_param::CMD_READ_MODIFY_WRITE:													//	Atomic transaction
						if (f_dataPNPCI->rok()){
							f_dataPNPCI->get=true;
							r_hhtci_dec=f_ctrlPNPCI_read.mskcnt-1;
							r_atdata_table[f_atids.read()][0]=f_dataPNPCI->read();
							r_hhtci_fsm=HHTCI_ATOMIC;
						}
						break;
					case hht_param::CMD_FLUSH:
						f_ctrlPNPCI->get=true;
						break;
					case hht_param::CMD_ADDREXT:
						f_ctrlPNPCI->get=true;
						printf("Unhandled command in channel NPCI: ");
						f_ctrlPCI_read.print(true);
						printf("\n");
						break;
					default:
						f_ctrlPNPCI->get=true;
						printf("Unknow command in channel NPCI: ");
						f_ctrlPCI_read.print(true);
						printf("\n");
						break;
					}
				}
			}
		}
		break;
	case HHTCI_WPOSTED:
	case HHTCI_WNPOSTED:
		if (r_hhtci_fsm.read()==HHTCI_WPOSTED){
			f_ctrlPNPCI=&f_ctrlPCI;
			f_dataPNPCI=&f_dataPCI;
			f_ctrlPNPCI_read=f_ctrlPCI_read;
		}else{	
			f_ctrlPNPCI=&f_ctrlNPCI;
			f_dataPNPCI=&f_dataNPCI;
			f_ctrlPNPCI_read=f_ctrlNPCI_read;
		}
		if (f_vciCI.wok() && f_vciids.rok()){
			f_dataPNPCI->get=true;
			if (r_hhtci_dec.read()==0){					// End of sending
				f_ctrlPNPCI->get=true;
				f_vciids.get=true;
				r_hhtci_fsm=HHTCI_IDLE;
				f_vciCI.write.eop=true;
			}else{										// Continue sending
				r_hhtci_dec=r_hhtci_dec.read()-1;
				r_hhtci_addr=(r_hhtci_addr.read()+4);
				f_vciCI.write.eop=false;
			}
			f_vciCI.write.cmd=vci_param::CMD_WRITE;
			f_vciCI.write.be=((r_hhtci_mask.read()) >>(r_hhtci_addr.read() & 0x1C)) & 0xF;
			f_vciCI.write.address=r_hhtci_addr.read();
			if (f_ctrlPNPCI_read.cmd & hht_param::CMD_DWORD_FLAG) // Non-masked write
				f_vciCI.write.plen=(f_ctrlPNPCI_read.mskcnt+1)*4;
			else
				f_vciCI.write.plen=(f_ctrlPNPCI_read.mskcnt)*4;
			f_vciCI.write.wdata=f_dataPNPCI->read();
			f_vciCI.put=true;
		}
		break;
	case HHTCI_ATOMIC:
		if (f_vciCI.wok() && f_atids.rok()){
			f_dataNPCI.get=true;
			f_ctrlNPCI.get=true;
			if (r_hhtci_dec.read()==0){				// Fetch & Add
				f_ctrlNPCI.get=true;
				f_atids.get=true;
				r_hhtci_fsm=HHTCI_IDLE;
			}else{									// Compare & Swap
				r_hhtci_dec=r_hhtci_dec.read()-1;
				r_hhtci_fsm=HHTCI_CS;
			}
			r_ctrl_table[f_atids.read()]=f_ctrlNPCI_read.get_ctrl();
			r_atdata_table[f_atids.read()][1]=f_dataNPCI.read();
			f_vciCI.write.srcid=f_atids.read();
			f_vciCI.write.pktid=f_ctrlPNPCI_read.srctag % 2;
			f_vciCI.write.trdid=(f_ctrlPNPCI_read.srctag/2) % 2;
			f_vciCI.write.wdata=0;
			f_vciCI.write.plen=8;
			f_vciCI.write.cmd=vci_param::CMD_LOCKED_READ;
			f_vciCI.write.address=r_hhtci_addr.read();
			f_vciCI.write.be=0xF;
			f_vciCI.write.eop=true;
			f_vciCI.put=true;
		}
		break;
	case HHTCI_CS:
		f_dataNPCI.get=true;
		if (r_hhtci_dec.read()==0){						// End of Compare & Swap
			f_ctrlNPCI.get=true;
			f_atids.get=true;
			r_hhtci_fsm=HHTCI_IDLE;
		}
		r_hhtci_dec=r_hhtci_dec.read()-1;
		r_atdata_table[f_atids.read()][3-r_hhtci_dec.read()]=f_dataNPCI.read();
		break;
	} // end switch r_hhtci_fsm 
	
	/**********************************************************************************************
	*** BLOCK HHT RESPONSE OUT : HHTRO
	**********************************************************************************************/
	
	VciRspFlit<vci_param> f_vciRO_read=f_vciRO.read();
	HhtCmdFlit<hht_param> r_ctrl_table_read;
	int id=f_vciRO_read.rsrcid;
	r_ctrl_table_read.set_ctrl(r_ctrl_table[id].read());
	
	f_ROtoCI.write.srcid=f_vciRO_read.rsrcid;
	f_ROtoCI.write.trdid=f_vciRO_read.rtrdid;
	f_ROtoCI.write.pktid=f_vciRO_read.rpktid;
	f_ROtoCI.write.plen=8;
	f_ROtoCI.write.be=0xF;
	
	if (!f_ROtoCI.wok())
		printf("Error : Deadlock risk, f_ROtoCI is not deep enough\n");
	switch (r_hhtro_fsm.read()) {
	case HHTRO_RESET:
		r_reset_numid=0;
		r_hhtro_fsm=HHTRO_FILLATID;
		break;
	case HHTRO_FILLATID:							// Fills atomic ids fifo
		f_atids.write=r_reset_numid.read();
		if (r_reset_numid.read()==nb_atids-1)
			r_hhtro_fsm=HHTRO_FILLID;
		r_reset_numid=r_reset_numid.read()+1;
		f_atids.put=true;
		break;
	case HHTRO_FILLID:							// Fills normal ids fifo
		f_vciids.write=r_reset_numid.read();
		if (r_reset_numid.read()==nb_atids+nb_ids-1)
			r_hhtro_fsm=HHTRO_IDLE;
		r_reset_numid=r_reset_numid.read()+1;
		f_vciids.put=true;
		break;
	case HHTRO_IDLE:
	case HHTRO_SENDING:
		if (f_ctrlRO.wok() && f_CItoRO.rok() && r_hhtro_fsm==HHTRO_IDLE){	// Sends responses generated by the block CI
			f_CItoRO.get=true;
			f_ctrlRO.write=f_CItoRO.read();
			f_ctrlRO.put=true;
		}else if (f_vciRO.rok() && f_ctrlRO.wok()){		// Send responses coming from VCI
			f_vciids.write=id;
			f_ctrlRO.write.unitid=r_ctrl_table_read.unitid;
			f_ctrlRO.write.srctag=r_ctrl_table_read.srctag;
			f_ctrlRO.write.bridge=1;
			f_ctrlRO.write.rvcset=0;
			f_ctrlRO.write.error=0;
			if ((r_ctrl_table_read.cmd & hht_param::CMD_READ_MASK)==hht_param::CMD_READ){ 
		// Read response
				if (f_dataRO.wok()){
					f_vciRO.get=true;
					if (f_vciRO_read.reop){ 				// Finishes sending
						if (r_ctrl_table_read.cmd & hht_param::CMD_DWORD_FLAG)
							f_ctrlRO.write.count=r_ctrl_table_read.mskcnt;
						else
							f_ctrlRO.write.count=0;
						f_ctrlRO.write.isoc  =(r_ctrl_table_read.cmd & hht_param::CMD_ISOC_FLAG)?1:0;
						f_ctrlRO.write.passpw=(r_ctrl_table_read.cmd & hht_param::CMD_RPASSPW_FLAG)?1:0;
						f_ctrlRO.write.cmd=hht_param::CMD_READRSP;
						f_ctrlRO.put=true;
						f_vciids.put=true;
						r_hhtro_fsm=HHTRO_IDLE;
					}else{
						r_hhtro_fsm=HHTRO_SENDING;
					}
					f_dataRO.write=(int)f_vciRO_read.rdata;
					f_dataRO.put=true;
				}
			}else if((r_ctrl_table_read.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE){
		// Write response
				if (r_ctrl_table_read.cmd & hht_param::CMD_POSTED_FLAG){
					f_vciRO.get=true;
					r_hhtro_fsm=HHTRO_IDLE;
					f_vciids.put=true;
				}else{
					f_vciRO.get=true;
					f_ctrlRO.write.count=r_ctrl_table_read.mskcnt;
					f_ctrlRO.write.isoc  =(r_ctrl_table_read.cmd & hht_param::CMD_ISOC_FLAG)?1:0;
					f_ctrlRO.write.passpw=0;
					f_ctrlRO.write.cmd=hht_param::CMD_TARGETDONE;
					r_hhtro_fsm=HHTRO_IDLE;
					f_ctrlRO.put=true;
					f_vciids.put=true;
					break;
				}
			}else if(r_ctrl_table_read.cmd==hht_param::CMD_READ_MODIFY_WRITE){
				if (f_vciRO_read.reop==0){
		// LL response
					f_vciRO.get=true;
					r_atdata_table[id][4]= f_vciRO_read.rdata;
					if (r_ctrl_table_read.mskcnt==1){ 		// Fetch & Add
						unsigned int sumres=f_vciRO_read.rdata + r_atdata_table[id][0].read();
						r_sumoverflow=(sumres<r_atdata_table[id][0].read());
						r_attmpdata=sumres;
					}else{									// Compare & Swap
						r_sumoverflow= (r_atdata_table[id][0].read()==f_vciRO_read.rdata);
						r_attmpdata=r_atdata_table[id][2].read();
					}
					r_hhtro_fsm=HHTRO_LL1;
				}else{
		// SC response
					if (f_vciRO_read.rdata==1){				// Success
						if (f_dataRO.wok()){
							r_hhtro_fsm=HHTRO_ATSUCCESS;
							f_dataRO.write=r_atdata_table[id][4].read();
							f_dataRO.put=true;
						}
					}else{									// Failure
						f_vciRO.get=true;
						f_ROtoCI.write.cmd=vci_param::CMD_LOCKED_READ;
						f_ROtoCI.write.eop=1;
						f_ROtoCI.write.address=r_ctrl_table_read.addr;
						f_ROtoCI.write.wdata=0;
						r_hhtro_fsm=HHTRO_IDLE;
						f_ROtoCI.put=true;
					}
				}
			}else{
				printf("Unexpected response : ");
				f_vciRO_read.print(true);
				printf(" corresponding to command ");
				r_ctrl_table_read.print(true);
				printf("\n");
			}
		}
		break;
	case HHTRO_LL1: // Receives the 2nd LL word and sends the 1st SC word
		if (f_vciRO.rok()){
			f_ROtoCI.write.cmd=vci_param::CMD_STORE_COND;
			f_ROtoCI.write.eop=0;
			f_ROtoCI.write.address=r_ctrl_table_read.addr;
			f_ROtoCI.write.wdata=r_attmpdata.read();
			r_atdata_table[id][5]= f_vciRO_read.rdata;
			if (r_ctrl_table_read.mskcnt==1){ 			// Fetch & Add
				r_attmpdata=f_vciRO_read.rdata + r_atdata_table[id][1].read() + (r_sumoverflow.read()?1:0);
				r_hhtro_fsm=HHTRO_LL2;
				f_ROtoCI.put=true;
			}else{										// Compare & Swap
				if (r_sumoverflow.read() && r_atdata_table[id][1]==f_vciRO_read.rdata){	// Processes the swap
					r_attmpdata=r_atdata_table[id][3].read();
					r_hhtro_fsm=HHTRO_LL2;
					f_ROtoCI.put=true;
				}else{																	// Reponds to CS
					if (f_dataRO.wok()){
						r_hhtro_fsm=HHTRO_ATSUCCESS;
						f_dataRO.write=r_atdata_table[id][4].read();
						f_dataRO.put=true;
					}
				}
			}
		}
		break;
	case HHTRO_LL2: // Sends the 2nd SC word
		f_vciRO.get=true;
		f_ROtoCI.write.cmd=vci_param::CMD_STORE_COND;
		f_ROtoCI.write.eop=1;
		f_ROtoCI.write.address=r_ctrl_table_read.addr+4;
		f_ROtoCI.write.wdata=r_attmpdata.read();
		r_hhtro_fsm=HHTRO_IDLE;
		f_ROtoCI.put=true;
		break;
	case HHTRO_ATSUCCESS:
		if (f_vciRO.rok() && f_ctrlRO.wok() && f_dataRO.wok()){
			f_vciRO.get=true;
			r_hhtro_fsm=HHTRO_IDLE;
			f_dataRO.write=r_atdata_table[id][5].read();
			f_ctrlRO.write.unitid=r_ctrl_table_read.unitid;
			f_ctrlRO.write.srctag=r_ctrl_table_read.srctag;
			f_ctrlRO.write.bridge=1;
			f_ctrlRO.write.rvcset=0;
			f_ctrlRO.write.error=0;
			f_ctrlRO.write.isoc=0;
			f_ctrlRO.write.passpw=0;
			f_ctrlRO.write.cmd=hht_param::CMD_READRSP;
			f_ctrlRO.write.count=1;
			f_dataRO.put=true;
			f_ctrlRO.put=true;
		}
		break;
	} // end switch r_hhtro_fsm 
	
	// Links fifos to ports
	f_vciRO.write.rerror=p_vci_io.rerror;
	f_vciRO.write.rdata=p_vci_io.rdata;
	f_vciRO.write.reop=p_vci_io.reop;
	f_vciRO.write.rsrcid=p_vci_io.rsrcid;
	f_vciRO.write.rtrdid=p_vci_io.rtrdid;
	f_vciRO.write.rpktid=p_vci_io.rpktid;  
	f_vciRO.put=p_vci_io.rspval;
	f_vciCI.get=p_vci_io.cmdack;
	
	f_ctrlRO.get=p_hht.ctrlR.wok;
	f_dataRO.get=p_hht.dataR.wok;
	f_ctrlPCI.write.set_ctrl(p_hht.ctrlPC.data);
	f_ctrlNPCI.write.set_ctrl(p_hht.ctrlNPC.data);
	f_ctrlPCI.put=p_hht.ctrlPC.rok;
	f_ctrlNPCI.put=p_hht.ctrlNPC.rok;
	f_dataPCI.write=p_hht.dataPC.data;
	f_dataNPCI.write=p_hht.dataNPC.data;
	f_dataPCI.put=p_hht.dataPC.rok;
	f_dataNPCI.put=p_hht.dataNPC.rok;

	// Processing of put and get for each fifo
	f_vciCI.fsm();
	f_vciRO.fsm();

	f_ROtoCI.fsm();
	f_CItoRO.fsm();
	f_vciids.fsm();
	f_atids.fsm();
	
	f_ctrlPCI.fsm();
	f_ctrlNPCI.fsm();
	f_dataPCI.fsm();
	f_dataNPCI.fsm();
	f_ctrlRO.fsm();
	f_dataRO.fsm();
	

}


tmpl(void)::genMoore()
{	
    // Links the VCI initiator port
	p_vci_io.rspack = f_vciRO.wok();
	p_vci_io.cmdval = f_vciCI.rok();
	
	p_vci_io.address=f_vciCI.read().address;
	p_vci_io.be=f_vciCI.read().be;
	p_vci_io.cmd=f_vciCI.read().cmd;
	p_vci_io.contig=f_vciCI.read().contig;
	p_vci_io.wdata=f_vciCI.read().wdata;
	p_vci_io.eop=f_vciCI.read().eop;
	p_vci_io.cons=f_vciCI.read().cons;
	p_vci_io.plen=f_vciCI.read().plen;
	p_vci_io.wrap=f_vciCI.read().wrap;
	p_vci_io.cfixed=f_vciCI.read().cfixed;
	p_vci_io.clen=f_vciCI.read().clen;
	p_vci_io.srcid=f_vciCI.read().srcid;
	p_vci_io.trdid=f_vciCI.read().trdid;
	p_vci_io.pktid=f_vciCI.read().pktid;  
	
	// Links the HHT target port
	p_hht.ctrlR.w=f_ctrlRO.rok();
	p_hht.dataR.w=f_dataRO.rok();
	HhtRspFlit<hht_param>	f_ctrlRO_read=f_ctrlRO.read();
	p_hht.ctrlR.data=f_ctrlRO_read.get_ctrl();
	p_hht.dataR.data=f_dataRO.read();
	
	p_hht.ctrlPC.r=f_ctrlPCI.wok();
	p_hht.ctrlNPC.r=f_ctrlNPCI.wok();
	p_hht.dataPC.r=f_dataPCI.wok();
	p_hht.dataNPC.r=f_dataNPCI.wok();
	
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

