/* -*- mode: c++; coding: utf-8 -*-
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
 * Maintainers: fpecheux, nipo
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 */

#include "../include/vci_iss_nocache.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename iss_t, typename vci_param> x VciIssNocache<iss_t,vci_param>

tmpl(tlmt_core::tlmt_return&)::callback(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
										const tlmt_core::tlmt_time &time,
										void *private_data)
{
	// std::cout << name() << " callback" << std::endl;
	e0.notify(sc_core::SC_ZERO_TIME);
	c0.set_time(time);
	// std::cout << " c0.time=" << c0.time() << std::endl;
	return m_return;
}

tmpl(tlmt_core::tlmt_return&)::callback_synchro(soclib::tlmt::synchro_packet *pkt,
                                                                           const tlmt_core::tlmt_time &time,
                                                                                void *private_data)
{
        std::cout << name() << " callback_synchro" << std::endl;
        //e0.notify(sc_core::SC_ZERO_TIME);
        //c0.set_time(time);
        // std::cout << " c0.time=" << c0.time() << std::endl;
        return m_return;
}


tmpl(void)::behavior()
{
	soclib::tlmt::vci_cmd_packet<vci_param> cmd;
	uint32_t addresses[32];
	uint32_t localbuf[32];

	for(;;) {

		// std::cout << "Starting new processor cycle" << std::endl;
		c0.set_time(c0.time()+tlmt_core::tlmt_time(1));

		if (m_iss.isBusy())
			m_iss.nullStep();
		else {
        		bool ins_asked;
	        	uint32_t ins_addr;
	        	m_iss.getInstructionRequest( ins_asked, ins_addr );

			// printf("addr=%8.8x\n",ins_addr);
			if (ins_asked) {
				cmd.cmd = vci_param::CMD_READ;
				addresses[0] = ins_addr;
				cmd.address = addresses;
				cmd.be = 0xF;
				cmd.contig = 0;
				cmd.buf = localbuf;
				cmd.length = 1;
				cmd.eop = 1;
				cmd.srcid = 0;
				cmd.trdid = 0;
				cmd.pktid = 0;

				tlmt_core::tlmt_return ret;
				ret = p_vci.send(&cmd, c0.time());
				sc_core::wait(e0);
				// std::cout << std::dec <<  name() << " ret.time=" << ret.time() << std::endl;
				// printf("add=%8.8x inst=%8.8x\n",ins_addr,localbuf[0]);
				m_iss.setInstruction(0,localbuf[0]);
			}

	        	bool mem_asked;
	        	enum iss_t::DataAccessType mem_type;
	        	uint32_t mem_addr;
	        	uint32_t mem_wdata;
	        	m_iss.getDataRequest( mem_asked, mem_type, mem_addr, mem_wdata );

			if (mem_asked) {
				// std::cout << "\tmem asked" << std::endl;
				switch (mem_type)
				{
					case iss_t::READ_WORD:
					{
                                		cmd.cmd = vci_param::CMD_READ;
                                		addresses[0] = mem_addr;
// printf("Data read word @=%8.8x\n",addresses[0]);
                                		cmd.address = addresses;
                                		cmd.be = 0xF;
                                		cmd.contig = 0;
                                		cmd.buf = localbuf;
                                		cmd.length = 1;
                                		cmd.eop = 1;
                                		cmd.srcid = 0;
                                		cmd.trdid = 0;
                                		cmd.pktid = 0;

                                		tlmt_core::tlmt_return ret;
                                		ret = p_vci.send(&cmd, c0.time());
                                		sc_core::wait(e0);
                                		// std::cout << name() << "ret.time=" << ret.time() << std::endl;
                                		m_iss.setDataResponse(0,localbuf[0]);
					}
						break;
					case iss_t::READ_HALF:
					{
						uint32_t dcache_subcell=mem_addr & 0x3;

                                                cmd.cmd = vci_param::CMD_READ;
                                                addresses[0] = mem_addr & ~0x3;
// printf("Data read half @=%8.8x\n",addresses[0]);
                                                cmd.address = addresses;
                                                cmd.be = 0x3 << mem_addr & 0x3;
                                                cmd.contig = 0;
                                                cmd.buf = localbuf;
                                                cmd.length = 1;
                                                cmd.eop = 1;
                                                cmd.srcid = 0;
                                                cmd.trdid = 0;
                                                cmd.pktid = 0;

                                                tlmt_core::tlmt_return ret;
                                                ret = p_vci.send(&cmd, c0.time());
                                                sc_core::wait(e0);
                                                // std::cout << name() << "ret.time=" << ret.time() << std::endl;
						uint32_t d = localbuf[0];
						d = 0xffff&(d>>(8*dcache_subcell));
						d = d | (d<<16);
                                                m_iss.setDataResponse(0,d);
					}
						break;
					case iss_t::READ_BYTE:
                                        {
                                                uint32_t dcache_subcell=mem_addr & 0x3;

                                                cmd.cmd = vci_param::CMD_READ;
                                                addresses[0] = mem_addr & ~0x3;
// printf("Data read byte @=%8.8x\n",addresses[0]);
                                                cmd.address = addresses;
                                                cmd.be = 0x1 << mem_addr & 0x3;
                                                cmd.contig = 0;
                                                cmd.buf = localbuf;
                                                cmd.length = 1;
                                                cmd.eop = 1;
                                                cmd.srcid = 0;
                                                cmd.trdid = 0;
                                                cmd.pktid = 0;

                                                tlmt_core::tlmt_return ret;
                                                ret = p_vci.send(&cmd, c0.time());
                                                sc_core::wait(e0);
                                                // std::cout << name() << "ret.time=" << ret.time() << std::endl;
                                                uint32_t d = localbuf[0];
                                                d = 0xff&(d>>(8*dcache_subcell));
                                                d = d | (d<<8) | (d<<16) | (d<<24) ;
                                                m_iss.setDataResponse(0,d);
                                        }
						break;
					case iss_t::LINE_INVAL:
						break;
					case iss_t::WRITE_WORD:
                                        {
                                                cmd.cmd = vci_param::CMD_WRITE;
                                                addresses[0] = mem_addr;
// printf("Data write word @=%8.8x\n",addresses[0]);
                                                cmd.address = addresses;
                                                cmd.be = 0xF;
                                                cmd.contig = 0;
						localbuf[0]=mem_wdata;
// printf("Data write word D=%8.8x\n",localbuf[0]);
                                                cmd.buf = localbuf;
                                                cmd.length = 1;
                                                cmd.eop = 1;
                                                cmd.srcid = 0;
                                                cmd.trdid = 0;
                                                cmd.pktid = 0;

                                                tlmt_core::tlmt_return ret;
                                                ret = p_vci.send(&cmd, c0.time());
                                                sc_core::wait(e0);
                                                // std::cout << name() << "ret.time=" << ret.time() << std::endl;
                                                m_iss.setDataResponse(0,0);
                                        }
						break;
					case iss_t::WRITE_HALF:
						break;
					case iss_t::WRITE_BYTE:
                                        {
                                                cmd.cmd = vci_param::CMD_WRITE;
                                                addresses[0] = mem_addr & ~0x3;
// printf("Data write byte @=%8.8x\n",addresses[0]);
                                                cmd.address = addresses;
                                                cmd.be = 0x1 << mem_addr & 0x3;
                                                cmd.contig = 0;
						uint32_t d=0xff & mem_wdata ;
						d = d | (d<<8) | (d<<16) | (d<<24);
                                                localbuf[0]=d;
// printf("Data write byte D=%8.8x\n",localbuf[0]);
                                                cmd.buf = localbuf;
                                                cmd.length = 1;
                                                cmd.eop = 1; 
                                                cmd.srcid = 0;
                                                cmd.trdid = 0;
                                                cmd.pktid = 0;

                                                tlmt_core::tlmt_return ret;
                                                ret = p_vci.send(&cmd, c0.time());
                                                sc_core::wait(e0);
                                                // std::cout << name() << "ret.time=" << ret.time() << std::endl;
                                                m_iss.setDataResponse(0,0);
                                        }
						break;
					case iss_t::STORE_COND:
						break;
					case iss_t::READ_LINKED:
						break;
				}
			}
			// std::cout << "step" << std::endl;
			m_iss.step();
			// std::cout << "dump" << std::endl;
			// m_iss.dump();

		}
	}
}

tmpl(/**/)::VciIssNocache( sc_core::sc_module_name name, int id )
		   : soclib::tlmt::BaseModule(name),
		   m_iss(id),
		   p_vci("vci", new tlmt_core::tlmt_callback<VciIssNocache,soclib::tlmt::vci_rsp_packet<vci_param> *>(
					 this, &VciIssNocache<iss_t,vci_param>::callback), &c0),
		   p_in("in", new tlmt_core::tlmt_callback<VciIssNocache,soclib::tlmt::synchro_packet*>(
					 this, &VciIssNocache<iss_t,vci_param>::callback_synchro))
{
	m_iss.reset();
	SC_THREAD(behavior);
}

}}
