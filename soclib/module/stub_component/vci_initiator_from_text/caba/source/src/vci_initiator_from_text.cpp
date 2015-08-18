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
#include "vci_initiator_from_text.h"

namespace soclib { 
namespace caba {

#define tmpl(x)  template<typename vci_param> x VciInitiatorFromText<vci_param>

tmpl(/**/)::VciInitiatorFromText(
    sc_module_name name )
    : soclib::caba::BaseModule(name),

      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci")
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
	for (int x=1023;x>=0;x--)
		init_sending[x]=false;
}

tmpl(/**/)::~VciInitiatorFromText()
{
	if (m_file_cmd!=0)
		fclose(m_file_cmd);
}

tmpl(bool)::start_send(char* filename, bool loop)
{
	m_file_cmd = fopen(filename, "r");
	if (m_file_cmd==0)
		return 0;
	m_sfilename = filename;
	has_to_loop = loop;
	ligne_en_cours=false;
	r_cmd_fsm = CMD_SEND;
	return 1;	
}
tmpl(void)::transition()
{
	

	
    if ( ! p_resetn.read() ) {
        r_cmd_fsm = CMD_RESET;
        r_rsp_fsm = RSP_RESET;
        return;
    }

    switch (r_cmd_fsm.read()) {
	case CMD_RESET:
	    r_cmd_fsm = CMD_IDLE;
		break;
    case CMD_IDLE:
		break;
    case CMD_SEND:
		;
		if (p_vci.cmdack == true && !init_sending[((m_vci_cmd.srcid<<2) + (m_vci_cmd.trdid<<1) + m_vci_cmd.pktid) % 1024]){
			if (m_vci_cmd.eop==1)
				init_sending[((m_vci_cmd.srcid<<2) + (m_vci_cmd.trdid<<1) + m_vci_cmd.pktid) % 1024]=true;
			if (!feof(m_file_cmd))
			{
				unsigned int num;
				int64_t numl;
				char chr;
				char str[1000];
				if (!ligne_en_cours){
					do{
						if (fscanf(m_file_cmd, " %d", &num))m_vci_cmd.srcid=num;
						if (fscanf(m_file_cmd, " %d", &num))m_vci_cmd.trdid=num;
						if (fscanf(m_file_cmd, " %d", &num))m_vci_cmd.pktid=num;
						
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
					case 'R':
						m_vci_cmd.cmd=vci_param::CMD_READ;
						break;
					case 'W':
						m_vci_cmd.cmd=vci_param::CMD_WRITE;
						break;
					case 'L':
						m_vci_cmd.cmd=vci_param::CMD_LOCKED_READ;
						break;
					case 'S':
						m_vci_cmd.cmd=vci_param::CMD_STORE_COND;
						break;
					default:
						m_vci_cmd.cmd=vci_param::CMD_NOP;
						break;
					}
					fscanf(m_file_cmd, "%c", &chr);
					fscanf(m_file_cmd, " (");
					
					fscanf(m_file_cmd, " %llX", &numl);	m_vci_cmd.address=numl;
					fscanf(m_file_cmd, " , %X", &num); m_vci_cmd.plen=num;
					if (m_vci_cmd.cmd==vci_param::CMD_WRITE || m_vci_cmd.cmd==vci_param::CMD_STORE_COND){
						fscanf(m_file_cmd, " , %X", &num); m_vci_cmd.wdata=num;
					}					
				}else{
					fscanf(m_file_cmd, " %X", &num); m_vci_cmd.wdata=num;
					m_vci_cmd.address=m_vci_cmd.address+4;
				}
				if (fscanf(m_file_cmd, " | %X",&num)){
					m_vci_cmd.be=num;
				}else{
					m_vci_cmd.be=0xF;
				}
				fscanf(m_file_cmd, " %c", &chr);
				m_vci_cmd.eop=(chr==')');
				ligne_en_cours=!(chr==')' || chr==';');
			}
			if (feof(m_file_cmd)){
				fclose(m_file_cmd);
				m_file_cmd=0;
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
		break;
    } // end switch r_cmd_fsm
	switch (r_rsp_fsm.read()) {
	case RSP_RESET:
	    r_rsp_fsm = RSP_IDLE;
		break;
    case RSP_IDLE:
		r_rsp_fsm=RSP_RECEIVE;
		break;
	case RSP_RECEIVE:
		if (p_vci.rspval==true && p_vci.reop==1)
		{
			init_sending[((p_vci.rsrcid.read()<<2) + (p_vci.rtrdid.read()<<1) + p_vci.rpktid.read()) % 1024]=false;
		}
		break;
	} // end switch r_rsp_fsm
	
    
}

tmpl(void)::genMoore()
{
    switch (r_cmd_fsm.read()) {
    case CMD_IDLE:
        p_vci.cmdval  = false;
        break;
    case CMD_SEND:
        p_vci.cmdval  = !init_sending[((m_vci_cmd.srcid<<2) + (m_vci_cmd.trdid<<1) + m_vci_cmd.pktid) % 1024];
        p_vci.address = m_vci_cmd.address;
        p_vci.wdata   = m_vci_cmd.wdata;
        p_vci.be      = m_vci_cmd.be;
        p_vci.plen    = m_vci_cmd.plen;
        p_vci.cmd     = m_vci_cmd.cmd;
        p_vci.trdid   = m_vci_cmd.trdid;
        p_vci.pktid   = m_vci_cmd.pktid;
        p_vci.srcid   = m_vci_cmd.srcid;
        p_vci.cons    = m_vci_cmd.cons;
        p_vci.wrap    = m_vci_cmd.wrap;
        p_vci.contig  = m_vci_cmd.contig;
        p_vci.clen    = m_vci_cmd.clen;
        p_vci.cfixed  = m_vci_cmd.cfixed;
        p_vci.eop     = m_vci_cmd.eop;
        break;
    } // end switch r_cmd_fsm
	
	switch (r_rsp_fsm.read()) {
	case RSP_RESET:
		p_vci.rspack=false;
		break;
	case RSP_IDLE:
		p_vci.rspack=false;
		break;
    case RSP_RECEIVE:	
		p_vci.rspack=true;
		break;
	}// end switch r_rsp_fsm
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

