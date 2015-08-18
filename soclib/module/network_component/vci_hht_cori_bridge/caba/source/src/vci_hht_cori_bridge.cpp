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
 
#include "vci_hht_cori_bridge.h"
namespace soclib { 
namespace caba {

#define READPASSWRITES 0			// Lets reads pass writes during the store & forward
#define VCI_READOK 0				// Successful read error code
#define VCI_WRITEOK 2				// Successful write error code
#define tmpl(x)  template<typename vci_param, typename hht_param> x VciHhtCoriBridge<vci_param, hht_param>

tmpl(/**/)::VciHhtCoriBridge(
    sc_module_name name )
    : soclib::caba::BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn"),
	  
      p_vci_io("vci_io"),
	  p_vci_config("vci_config"),
	  p_hht("p_hht"),
	  
	  f_ctrlPCO("f_ctrlPCO",2),
	  f_ctrlNPCO("f_ctrlNPCO",2),
	  f_dataPCO("f_dataPCO",2),
	  f_dataNPCO("f_dataNPCO",2),
	  f_ctrlRI("f_ctrlRI",2),
	  f_dataRI("f_dataRI",17),
	  
	  f_vciCO("f_vciCO",2),	  
	  f_vciRI("f_vciRI",2),
	  
	  f_COtoRI("f_COtoRI",2),
	  f_hhtids("f_hhtids",nb_ids),
	  
	  f_ctrlCO("f_ctrlCO",2),
	  f_maskCO("f_maskCO",2),
	  f_dataCO("f_dataCO",17)
	  
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

tmpl(/**/)::~VciHhtCoriBridge()
{
	
}

tmpl(void)::transition()
{

	if ( !p_resetn.read() ) {
		r_vcico_fsm = VCICO_RESET;
		r_hhtco_fsm = HHTCO_RESET;
		r_vciri_fsm = VCIRI_RESET;
		
		f_vciCO.init();
        f_vciRI.init();
        
		f_hhtids.init();
        f_COtoRI.init();
        f_ctrlCO.init();
		f_dataCO.init();
		f_maskCO.init();
		f_ctrlPCO.init();
        f_ctrlNPCO.init();
        f_dataPCO.init();
        f_dataNPCO.init();
        f_ctrlRI.init();
        f_dataRI.init();
        return;
    }
	
	/**********************************************************************************************
	*** BLOCK TRANSLATE & STORE COMMAND OUT : VCICO
	**********************************************************************************************/
	// Copy locally the outputs of the fifos
	VciCmdFlit<vci_param> 	f_vciCO_read =	f_vciCO.read();
	int		 			f_hhtids_read = 	f_hhtids.read();

	// Link fifos' fields not depending on the FSM state
	f_ctrlCO.write.seqid=0;
	f_ctrlCO.write.passpw=0;
	f_ctrlCO.write.compat=0;
	f_dataCO.write=(typename hht_param::data_t)f_vciCO_read.wdata;
	f_COtoRI.write.rsrcid=f_vciCO_read.srcid;
	f_COtoRI.write.rtrdid=f_vciCO_read.trdid;
	f_COtoRI.write.rpktid=f_vciCO_read.pktid;
	switch (r_vcico_fsm.read()) {
    case VCICO_RESET:
		r_vcico_fsm=VCICO_IDLE;
		break;
	case VCICO_IDLE:
		if (f_vciCO.rok() && f_hhtids.rok()){
			
			r_srcid_table[f_hhtids_read]=f_vciCO_read.srcid;
			r_trdid_table[f_hhtids_read]=f_vciCO_read.trdid;
			r_pktid_table[f_hhtids_read]=f_vciCO_read.pktid;
			f_ctrlCO.write.unitid=f_hhtids_read/8;
			f_ctrlCO.write.srctag=(f_hhtids_read%8)*4+ (f_vciCO_read.trdid%2)*2+(f_vciCO_read.pktid%2);
			
			switch (f_vciCO_read.cmd){
			case vci_param::CMD_LOCKED_READ:
			case vci_param::CMD_STORE_COND:
				std::cout << "Unhandled command\n";
				if (f_COtoRI.wok()){			// Doesn't handle those commands => responds an error
					f_vciCO.get=true;
					f_COtoRI.write.rerror=VCI_READOK+1;
					f_COtoRI.put=true;
				}
				break;

			case vci_param::CMD_READ:
				if (f_ctrlCO.wok()){
					if (f_vciCO_read.plen<=4 && f_vciCO_read.be != 0xF){ 	// Masked read
						f_ctrlCO.write.cmd=hht_param::CMD_READ;
						f_ctrlCO.write.mskcnt=f_vciCO_read.be;
					}else{													// Non-masked read
						f_ctrlCO.write.cmd=hht_param::CMD_READ | hht_param::CMD_DWORD_FLAG;
						f_ctrlCO.write.mskcnt=((f_vciCO_read.plen+3)>>2)-1;
					}
					f_vciCO.get=true;
					f_hhtids.get=true;
					f_ctrlCO.write.addr=f_vciCO_read.address;
					f_ctrlCO.put=true;
				}
				break;

			case vci_param::CMD_WRITE:
				if (f_dataCO.wok()){
					if (((f_vciCO_read.plen+3)>>2)==1){				// Stores in 1 clock cycle
						if (f_ctrlCO.wok() && (f_vciCO_read.trdid ==0 || f_COtoRI.wok())){
							f_vciCO.get=true;
							
							f_ctrlCO.write.cmd=hht_param::CMD_WRITE;
							if (f_vciCO_read.be == 0xF){		// Non-masked write
								f_ctrlCO.write.cmd|=hht_param::CMD_DWORD_FLAG;
								f_ctrlCO.write.mskcnt=((f_vciCO_read.plen+3)>>2)-1;
							}else{								// Masked write
								f_ctrlCO.write.mskcnt=(f_vciCO_read.plen+3)>>2;
								f_maskCO.write=f_vciCO_read.be<<(f_vciCO_read.address & 0x1C);
								f_maskCO.put=true;
							}
							if (f_vciCO_read.trdid == 1){		// Posted write
								f_ctrlCO.write.cmd|=hht_param::CMD_POSTED_FLAG;
								f_COtoRI.write.rerror= VCI_WRITEOK ;
								f_COtoRI.put=true;
							}else{								// Non-posted write
								f_hhtids.get=true;
							}
							f_ctrlCO.write.addr=f_vciCO_read.address;
							f_ctrlCO.put=true;
							f_dataCO.put=true;
						}
					}else{										// Starts storing
						f_vciCO.get=true;
						for (int x=0;x<7;x++)
							r_vcico_mask[x]=(x==(int)((f_vciCO_read.address & 0x1C)>>2))?(int)f_vciCO_read.be:0;
						r_vcico_dec=(((f_vciCO_read.plen+3)>>2)-2);
						r_vcico_fsm=VCICO_SENDING;
						r_vcico_addr=f_vciCO_read.address;
						r_vcico_masked=(f_vciCO_read.be!=0xF);
						f_dataCO.put=true;
					}
				}
			}
		}
		break;
	case VCICO_SENDING:
		if (f_vciCO.rok() && f_hhtids.rok() && f_dataCO.wok()){
			if (r_vcico_dec.read()==0){						// Finishes storing
				if (f_ctrlCO.wok() && (f_vciCO_read.trdid ==0 || f_COtoRI.wok())){
					f_vciCO.get=true;
					f_ctrlCO.write.cmd=hht_param::CMD_WRITE;
					if (r_vcico_masked.read() || (f_vciCO_read.be!=0xF)){		// Masked write
						f_ctrlCO.write.mskcnt=(f_vciCO_read.plen+3)>>2;
						f_maskCO.write=	(typename hht_param::data_t)
										((f_vciCO_read.be << (f_vciCO_read.address&0x1C))+ (r_vcico_mask[6].read() << 24) + 
													(r_vcico_mask[5].read() << 20) + (r_vcico_mask[4].read() << 16) + 
													(r_vcico_mask[3].read() << 12) + (r_vcico_mask[2].read() << 8)  + 
													(r_vcico_mask[1].read() << 4)  + (r_vcico_mask[0].read() << 0));
						f_maskCO.put=true;
					}else{														// Non-masked write
						f_ctrlCO.write.cmd|=hht_param::CMD_DWORD_FLAG;
						f_ctrlCO.write.mskcnt=((f_vciCO_read.plen+3)>>2)-1;
					}
					if (f_vciCO_read.trdid == 1){					// Posted write
						f_ctrlCO.write.cmd|=hht_param::CMD_POSTED_FLAG;
						f_COtoRI.write.rerror= VCI_WRITEOK;
						f_COtoRI.put=true;
					}else{											// Non-posted write
						f_hhtids.get=true;
					}
					f_ctrlCO.write.addr=r_vcico_addr.read();
					f_ctrlCO.put=true;
					f_dataCO.put=true;
					r_vcico_fsm=VCICO_IDLE;
				}
			}else{													// Continues storing
				f_vciCO.get=true;
				if ((f_vciCO_read.address & 0x1C)>>2==7)
				{
					printf("Write packet not included in an aligned 8 words block: ");
					f_vciCO_read.print(true);
					printf("\n");
				}
				r_vcico_mask[(f_vciCO_read.address & 0x1C)>>2]=f_vciCO_read.be;
				r_vcico_masked=r_vcico_masked.read() || (f_vciCO_read.be!=0xF);
				r_vcico_dec=r_vcico_dec.read()-1;
				f_dataCO.put=true;
			}
		}
		break;
    } // end switch r_vcico_fsm 
	/**********************************************************************************************
	*** BLOCK FORWARD COMMAND OUT : HHTCO
	**********************************************************************************************/
	// Copy locally the outputs of the fifos
	HhtCmdFlit<hht_param> 	f_ctrlCO_read =	f_ctrlCO.read();
	HhtCmdFlit<hht_param>		r_ctrlCO_read;
	r_ctrlCO_read.set_ctrl(r_ctrlCO.read());
	
	// Pointers to the output fifos, according to posted/non-posted packets
	LazyFifo<HhtCmdFlit<hht_param> > *f_ctrlPNPCO;
	LazyFifo<typename hht_param::data_t> *f_dataPNPCO;
	
	switch (r_hhtco_fsm.read()) {
    case HHTCO_RESET:
		r_hhtco_fsm=HHTCO_IDLE;
		break;
	case HHTCO_IDLE:
		if (f_ctrlCO.rok()){	// Everything is stored, we can forward
			if ((f_ctrlCO_read.cmd & hht_param::CMD_READ_MASK) == hht_param::CMD_READ){ 			// Read
				if (f_ctrlNPCO.wok()){
					f_ctrlCO.get=true;
					f_ctrlNPCO.write=f_ctrlCO_read;
					f_ctrlNPCO.put=true;
				}
			}else if ((f_ctrlCO_read.cmd & hht_param::CMD_WRITE_MASK) == hht_param::CMD_WRITE){ 	// Write
				if ((f_ctrlCO_read.cmd & hht_param::CMD_POSTED_FLAG)){
					f_ctrlPNPCO=&f_ctrlPCO;
					f_dataPNPCO=&f_dataPCO;
				}else{	
					f_ctrlPNPCO=&f_ctrlNPCO;
					f_dataPNPCO=&f_dataNPCO;
				}
				
				if (f_dataPNPCO->wok()){
					if ((f_ctrlCO_read.cmd & hht_param::CMD_DWORD_FLAG)){ 	// Non-masked write
						if ((f_ctrlCO_read.mskcnt)==0){
								if (f_ctrlPNPCO->wok()){
								f_ctrlCO.get=true;
								f_dataCO.get=true;
								f_ctrlPNPCO->write=f_ctrlCO_read;
								f_dataPNPCO->write=f_dataCO.read();
								f_ctrlPNPCO->put=true;
								f_dataPNPCO->put=true;
							}
						}else{
							f_ctrlCO.get=true;
							f_dataCO.get=true;
							r_ctrlCO=f_ctrlCO_read.get_ctrl();
							f_dataPNPCO->write=f_dataCO.read();
							f_dataPNPCO->put=true;
							r_hhtco_dec=f_ctrlCO_read.mskcnt-1;
							r_hhtco_fsm=HHTCO_SENDING;
						}
					}else{													// Masked write
						f_ctrlCO.get=true;
						f_maskCO.get=true;
						r_ctrlCO=f_ctrlCO_read.get_ctrl();
						f_dataPNPCO->write= f_maskCO.read();
						f_dataPNPCO->put=true;
						r_hhtco_dec=f_ctrlCO_read.mskcnt-1;
						r_hhtco_fsm=HHTCO_SENDING;
					}
				}
			}else{
				printf("CMD field of HHT packet in f_ctrlCO is invalid: ");
				f_ctrlCO_read.print(true);
				printf("\n");
			}
		}
		break;
	case HHTCO_SENDING:
		if (f_ctrlCO.rok() && READPASSWRITES){ 	// If we can send a read before the end of the write, we do
			if ((f_ctrlCO_read.cmd & hht_param::CMD_READ_MASK) == hht_param::CMD_READ){
				if (!(f_dataNPCO.wok() && r_hhtco_dec.read()==0) || (r_ctrlCO_read.cmd & hht_param::CMD_POSTED_FLAG)){ // <=>f_ctrlNPCO.put=false
					if (f_ctrlNPCO.wok()){
						f_ctrlCO.get=true;
						f_ctrlNPCO.write=f_ctrlCO_read;
						f_ctrlNPCO.put=true;
					}
				}
			}
		}
		if (f_dataCO.rok()){ 					// Continues sending the write
			if ((r_ctrlCO_read.cmd & hht_param::CMD_POSTED_FLAG)){
				f_ctrlPNPCO=&f_ctrlPCO;
				f_dataPNPCO=&f_dataPCO;
			}else{	
				f_ctrlPNPCO=&f_ctrlNPCO;
				f_dataPNPCO=&f_dataNPCO;
			}
			if (f_dataPNPCO->wok()){
				if (r_hhtco_dec.read()==0){								// Finishes sending
					if (f_ctrlPNPCO->wok()){
						f_dataCO.get=true;
						f_ctrlPNPCO->write=r_ctrlCO_read;
						f_dataPNPCO->write=f_dataCO.read();
						f_ctrlPNPCO->put=true;
						f_dataPNPCO->put=true;
						r_hhtco_fsm=HHTCO_IDLE;
					}
				}else{											// Continues sending
					f_dataCO.get=true;
					f_dataPNPCO->write=f_dataCO.read();
					f_dataPNPCO->put=true;
					r_hhtco_dec=r_hhtco_dec.read()-1;
				}
			}	
		}else{
			printf ("Not enough data in f_dataCO: f_ctrlCO=");
			r_ctrlCO_read.print(true);
			printf("\n");
		}
		break;

	} // end switch r_hhtco_fsm 
	
	/**********************************************************************************************
	*** Partie VCI RESPONSE IN : VCIRI
	**********************************************************************************************/
	HhtRspFlit<hht_param> f_ctrlRI_read=f_ctrlRI.read();
				
	switch (r_vciri_fsm.read()) {
	case VCIRI_RESET:
		r_reset_numid=0;
		r_vciri_fsm=VCIRI_FILLID;
		break;
	case VCIRI_FILLID:							// Fills ids fifo
		if (r_reset_numid.read()==nb_ids-1)
			r_vciri_fsm=VCIRI_IDLE;
		f_hhtids.write=r_reset_numid.read();
		f_hhtids.put=true;
		r_reset_numid=r_reset_numid.read()+1;
		break;
	case VCIRI_IDLE:
		f_hhtids.write=f_ctrlRI_read.unitid*8+f_ctrlRI_read.srctag/4;
		if (f_vciRI.wok()) {
			if (f_COtoRI.rok()){			// Sends responses generated by the block CO
				f_vciRI.write=((CoToRi<vci_param>)f_COtoRI.read()).get_vci_rsp_flit();
				f_COtoRI.get=true;
				f_vciRI.put=true;
			}else if (f_ctrlRI.rok()){
				f_vciRI.write.rsrcid=r_srcid_table[f_ctrlRI_read.unitid*8+f_ctrlRI_read.srctag/4].read();
				f_vciRI.write.rtrdid=r_trdid_table[f_ctrlRI_read.unitid*8+f_ctrlRI_read.srctag/4].read();
				f_vciRI.write.rpktid=r_pktid_table[f_ctrlRI_read.unitid*8+f_ctrlRI_read.srctag/4].read();
				switch (f_ctrlRI_read.cmd){
				case hht_param::CMD_TARGETDONE:
					f_ctrlRI.get=true;
					if (f_hhtids.wok())
						f_hhtids.put=true;
					f_vciRI.write.rerror = VCI_WRITEOK;
					f_vciRI.write.reop=1;
					f_vciRI.write.rdata=0;
					f_vciRI.put=true;
					break;
				case hht_param::CMD_READRSP:
					if (f_dataRI.rok()){
						f_dataRI.get=true;
						if (f_ctrlRI_read.count==0){ // Sends in 1 clock cycle
							f_ctrlRI.get=true;
							f_vciRI.write.reop=1;
							if (f_hhtids.wok())
								f_hhtids.put=true;
						}else{						// Starts sending a response needing more than 1 clock cycle.
							f_vciRI.write.reop=0;
							r_vciri_dec=f_ctrlRI_read.count-1;
							r_vciri_fsm=VCIRI_SENDING;
						}
						f_vciRI.write.rerror = VCI_READOK;
						f_vciRI.write.rdata=f_dataRI.read();
						f_vciRI.put=true;
					}
					break;
				default:
					f_ctrlRI.get=true;
					printf("Uknown HHT response: ");
					f_ctrlRI_read.print(true);
					printf("\n");
					break;
				}
			}
		}
		break;
	case VCIRI_SENDING:
		f_hhtids.write=f_ctrlRI_read.unitid*8+f_ctrlRI_read.srctag/4;
		if (f_vciRI.wok() && f_dataRI.rok()){		// Sends responses from HHT needing more than 1 clock cycle to be sent
			f_dataRI.get=true;
			if (r_vciri_dec.read()==0){
				f_ctrlRI.get=true;
				f_vciRI.write.reop=1;
				r_vciri_fsm=VCIRI_IDLE;
				if (f_hhtids.wok())
					f_hhtids.put=true;
			}else{
				r_vciri_dec=r_vciri_dec.read()-1;
				f_vciRI.write.reop=0;
			}
			f_vciRI.write.rerror=VCI_READOK;
			f_vciRI.write.rdata=f_dataRI.read();
			f_vciRI.put=true;
		}
		break;
	} // end switch r_vciri_fsm 
	
	// Links fifos to ports
	f_vciCO.write.address=p_vci_io.address;
	f_vciCO.write.be=p_vci_io.be;
	f_vciCO.write.cmd=p_vci_io.cmd;
	f_vciCO.write.contig=p_vci_io.contig;
	f_vciCO.write.wdata=p_vci_io.wdata;
	f_vciCO.write.eop=p_vci_io.eop;
	f_vciCO.write.cons=p_vci_io.cons;
	f_vciCO.write.plen=p_vci_io.plen;
	f_vciCO.write.wrap=p_vci_io.wrap;
	f_vciCO.write.cfixed=p_vci_io.cfixed;
	f_vciCO.write.clen=p_vci_io.clen;
	f_vciCO.write.srcid=p_vci_io.srcid;
	f_vciCO.write.trdid=p_vci_io.trdid;
	f_vciCO.write.pktid=p_vci_io.pktid;  
	f_vciCO.put=p_vci_io.cmdval;
	f_vciRI.get=p_vci_io.rspack;
	
	f_ctrlPCO.get=p_hht.ctrlPC.wok;
	f_ctrlNPCO.get=p_hht.ctrlNPC.wok;
	f_dataPCO.get=p_hht.dataPC.wok;
	f_dataNPCO.get=p_hht.dataNPC.wok;
	f_ctrlRI.write.set_ctrl(p_hht.ctrlR.data);
	f_ctrlRI.put=p_hht.ctrlR.rok;
	f_dataRI.put=p_hht.dataR.rok;
	f_dataRI.write=p_hht.dataR.data;

	// Processing of put and get for each fifo
	f_hhtids.fsm();
	f_COtoRI.fsm();		
    f_ctrlCO.fsm();
	f_dataCO.fsm();
	f_maskCO.fsm();

	f_vciCO.fsm();
	f_vciRI.fsm();
	
	f_ctrlPCO.fsm();
	f_ctrlNPCO.fsm();
	f_dataPCO.fsm();
	f_dataNPCO.fsm();
	f_ctrlRI.fsm();
	f_dataRI.fsm();
	
}

tmpl(void)::genMoore()
{

	
    // Links the VCI target port
	p_vci_io.cmdack = f_vciCO.wok();
	p_vci_io.rspval = f_vciRI.rok();
	
	p_vci_io.rdata = f_vciRI.read().rdata;
	p_vci_io.reop = f_vciRI.read().reop;
	p_vci_io.rerror = f_vciRI.read().rerror;
	p_vci_io.rsrcid = f_vciRI.read().rsrcid;
	p_vci_io.rtrdid = f_vciRI.read().rtrdid;
	p_vci_io.rpktid = f_vciRI.read().rpktid;
	
	
	// Links the HHT initiator port
	p_hht.ctrlPC.w=f_ctrlPCO.rok();
	p_hht.ctrlNPC.w=f_ctrlNPCO.rok();
	p_hht.dataPC.w=f_dataPCO.rok();
	p_hht.dataNPC.w=f_dataNPCO.rok();
	HhtCmdFlit<hht_param>	f_ctrlPCO_read=f_ctrlPCO.read();
	HhtCmdFlit<hht_param>	f_ctrlNPCO_read=f_ctrlNPCO.read();
	p_hht.ctrlPC.data=f_ctrlPCO_read.get_ctrl();
	p_hht.ctrlNPC.data=f_ctrlNPCO_read.get_ctrl();
	p_hht.dataPC.data=f_dataPCO.read();
	p_hht.dataNPC.data=f_dataNPCO.read();
	
	p_hht.ctrlR.r=f_ctrlRI.wok();
	p_hht.dataR.r=f_dataRI.wok();
	
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

