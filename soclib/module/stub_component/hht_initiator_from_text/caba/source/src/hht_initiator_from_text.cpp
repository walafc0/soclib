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
 
#include <limits>
#include <cassert>
#include "hht_initiator_from_text.h"

namespace soclib { 
namespace caba {

#define tmpl(x)  template<typename hht_param> x HhtInitiatorFromText<hht_param>

tmpl(/**/)::HhtInitiatorFromText(
    sc_module_name name )
    : soclib::caba::BaseModule(name),

      p_clk("clk"),
      p_resetn("resetn"),
	  p_hht("p_hht"),
	  
	  f_ctrlPCI("f_ctrlPCI",20),
	  f_ctrlNPCI("f_ctrlNPCI",20),
	  f_dataPCI("f_dataPCI",20),
	  f_dataNPCI("f_dataNPCI",20),
	  f_ctrlRO("f_ctrlRO",20),
	  f_dataRO("f_dataRO",20)
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

tmpl(/**/)::~HhtInitiatorFromText()
{
}

tmpl(bool)::start_send(char* filename, bool loop)
{
	m_file_cmd = fopen(filename, "r");
	if (m_file_cmd==0)
		return 0;
	m_sfilename = filename;
	has_to_loop = loop;
	inside_command=false;
	r_cmd_fsm = CMD_SEND;
	waiting=false;
	return 1;	
}

tmpl(void)::transition()
{
    if ( ! p_resetn.read() ) {
        r_cmd_fsm = CMD_RESET;
        r_rsp_fsm = RSP_RESET;
		f_ctrlPCI.init();
        f_ctrlNPCI.init();
        f_dataPCI.init();
        f_dataNPCI.init();
        f_ctrlRO.init();
        f_dataRO.init();
        return;
    }

    switch (r_cmd_fsm.read()) {
	case CMD_RESET:
	    r_cmd_fsm = CMD_IDLE;
		break;
    case CMD_IDLE:
		break;
    case CMD_SEND:
		if (waiting){
			f_ctrlNPCI.write=m_hht_cmd;
			f_ctrlNPCI.put=true;
			waiting=false;
		}else{
			if (f_ctrlPCI.wok() && f_ctrlNPCI.wok() && f_dataPCI.wok() && f_dataNPCI.wok()){
				if (!feof(m_file_cmd))
				{
					int num;
					int64_t numl;
					char chr;
					char str[1000];
					if (!inside_command){
						do{
							if (fscanf(m_file_cmd, " %d", &num))m_hht_cmd.unitid=num;
							if (fscanf(m_file_cmd, " %d", &num))m_hht_cmd.srctag=num;
							
							chr=0;
							fscanf(m_file_cmd, " %c", &chr);
							if (chr==39){ // The character ' introduces a commentary
								do{
									str[0]='\r';
									fscanf(m_file_cmd,"%c",str);
								}while (str[0]!='\r');
							}
						}while (chr==39);
						switch (chr){
						case 'N':
						case 'P':
							m_hht_cmd.cmd=hht_param::CMD_WRITE;
							if (chr=='P'){
								m_hht_cmd.cmd|=hht_param::CMD_POSTED_FLAG;
								f_ctrlPCI.put=true;
								f_dataPCI.put=true;
							}else{
								f_ctrlNPCI.put=true;
								f_dataNPCI.put=true;
							}
							fscanf(m_file_cmd,"%c",&chr);
							if (chr=='D')
								m_hht_cmd.cmd|=hht_param::CMD_DWORD_FLAG;
							break;
						case 'R':
							m_hht_cmd.cmd=hht_param::CMD_READ;
							f_ctrlNPCI.put=true;
							fscanf(m_file_cmd,"%c",&chr);
							if (chr=='D')
								m_hht_cmd.cmd|=hht_param::CMD_DWORD_FLAG;
							break;
						case 'A':
							m_hht_cmd.cmd=hht_param::CMD_READ_MODIFY_WRITE;
							f_ctrlNPCI.put=true;
							f_dataNPCI.put=true;
							break;
						default:
							m_hht_cmd.cmd=hht_param::CMD_UNKNOWN;
							break;
						}
						fscanf(m_file_cmd, "%c", &chr);
						fscanf(m_file_cmd, " (");
						
						fscanf(m_file_cmd, " %llX", &numl);	m_hht_cmd.addr=numl;
						if (m_hht_cmd.cmd==hht_param::CMD_READ)
							fscanf(m_file_cmd, " , %X", &num);
						else
							fscanf(m_file_cmd, " , %X", &num);
						m_hht_cmd.mskcnt=num;
						
						if (f_dataPCI.put || f_dataNPCI.put){
							if (((m_hht_cmd.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE) &&
								 ((m_hht_cmd.cmd & hht_param::CMD_DWORD_FLAG)==0)){ // Masked write
								fscanf(m_file_cmd, " , %X", &num);
							}else{
								fscanf(m_file_cmd, " , %X", &num);
							}
							m_hht_cmd_data=num;
						}
					}else{
						fscanf(m_file_cmd, " %X", &num); m_hht_cmd_data=num;
						if ((m_hht_cmd.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE &&
								(m_hht_cmd.cmd & hht_param::CMD_POSTED_FLAG)) //Posted write
							f_dataPCI.put=true;
						else
							f_dataNPCI.put=true;
					}
					fscanf(m_file_cmd, " %c", &chr);
					inside_command=(chr!=')');
					f_ctrlPCI.write=m_hht_cmd;
					f_ctrlNPCI.write=m_hht_cmd;
					f_dataPCI.write=m_hht_cmd_data;
					f_dataNPCI.write=m_hht_cmd_data;
				}
				if (feof(m_file_cmd)){
					fclose(m_file_cmd);
					if (has_to_loop)
						m_file_cmd=fopen(m_sfilename, "r");
					else
						r_cmd_fsm=CMD_IDLE;
				}

					/****************************
						Commands source file format:
						See commands source file
					*****************************/
			}
		}
		break;
    } // end switch r_cmd_fsm
	if (f_ctrlNPCI.will_put()){
		if (init_sending[((m_hht_cmd.unitid<<5) + m_hht_cmd.srctag) % 1024]){
			waiting=true;
			f_ctrlNPCI.put=false;
		}else{
			init_sending[((m_hht_cmd.unitid<<5) + m_hht_cmd.srctag) % 1024]=true;
		}
	}
	
	switch (r_rsp_fsm.read()) {
	case RSP_RESET:
	    r_rsp_fsm = RSP_IDLE;
		break;
    case RSP_IDLE:
		r_rsp_fsm = RSP_RECEIVE;
		break;
	case RSP_RECEIVE:
		if (f_ctrlRO.rok()){
			f_ctrlRO.get=true;
			init_sending[((f_ctrlRO.read().unitid<<5) + f_ctrlRO.read().srctag) % 1024]=false;
			//printf("Received a response header\n");
		}
		if (f_dataRO.rok()){
			f_dataRO.get=true;
			//printf("Received a response data\n");
		}
		break;
	} // end switch r_rsp_fsm
	
	f_ctrlPCI.get=p_hht.ctrlPC.wok;
	f_ctrlNPCI.get=p_hht.ctrlNPC.wok;
	f_dataPCI.get=p_hht.dataPC.wok;
	f_dataNPCI.get=p_hht.dataNPC.wok;
	
	f_ctrlRO.write.set_ctrl(p_hht.ctrlR.data);
	f_ctrlRO.put=p_hht.ctrlR.rok;
	f_dataRO.write=p_hht.dataR.data;
	f_dataRO.put=p_hht.dataR.rok;

	f_ctrlPCI.fsm();
	f_ctrlNPCI.fsm();
	f_dataPCI.fsm();
	f_dataNPCI.fsm();
	f_ctrlRO.fsm();
	f_dataRO.fsm();
}

tmpl(void)::genMoore()
{
    p_hht.ctrlPC.w=f_ctrlPCI.rok();
	p_hht.ctrlNPC.w=f_ctrlNPCI.rok();
	p_hht.dataPC.w=f_dataPCI.rok();
	p_hht.dataNPC.w=f_dataNPCI.rok();
	HhtCmdFlit<hht_param>	f_ctrlPCI_read=f_ctrlPCI.read();
	HhtCmdFlit<hht_param>	f_ctrlNPCI_read=f_ctrlNPCI.read();
	p_hht.ctrlPC.data=f_ctrlPCI_read.get_ctrl();
	p_hht.ctrlNPC.data=f_ctrlNPCI_read.get_ctrl();
	p_hht.dataPC.data=f_dataPCI.read();
	p_hht.dataNPC.data=f_dataNPCI.read();
	
	p_hht.ctrlR.r=f_ctrlRO.wok();
	p_hht.dataR.r=f_dataRO.wok();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

