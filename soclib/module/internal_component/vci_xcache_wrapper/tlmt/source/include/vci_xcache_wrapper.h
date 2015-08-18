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
 * Maintainers: fpecheux, nipo, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */
 

#include <tlmt>
#include "tlmt_base_module.h"
#include "vci_ports.h"

#include <inttypes.h>
#include "soclib_endian.h"

#include "buffer.h"
#include "generic_cache.h"
#include "mapping_table.h"

#define MAXIMUM_PACKET_SIZE 100

namespace soclib { namespace tlmt {

  template<typename iss_t,typename vci_param>
  class VciXcacheWrapper
    : public soclib::tlmt::BaseModule
  {
    enum dcache_fsm_state_e {
      DCACHE_INIT,
      DCACHE_IDLE,
      DCACHE_WRITE_UPDT,
      DCACHE_WRITE_REQ,
      DCACHE_MISS_REQ,
      DCACHE_MISS_WAIT,
      DCACHE_MISS_UPDT,
      DCACHE_UNC_REQ,
      DCACHE_UNC_WAIT,
      DCACHE_INVAL,
      DCACHE_ERROR,
    };

    enum icache_fsm_state_e {
      ICACHE_INIT,
      ICACHE_IDLE,
      ICACHE_WAIT,
      ICACHE_UPDT,
      ICACHE_ERROR,
    };

    enum cmd_fsm_state_e {
      CMD_IDLE,
      CMD_DATA_MISS,
      CMD_DATA_UNC_READ,
      CMD_DATA_UNC_STORE_COND,
      CMD_DATA_UNC_READ_LINKED,
      CMD_DATA_WRITE,
      CMD_INS_MISS,
    };

    enum rsp_fsm_state_e {
      RSP_IDLE,
      RSP_INS_MISS,
      RSP_INS_ERROR_WAIT,
      RSP_INS_ERROR,
      RSP_INS_OK,
      RSP_DATA_MISS,
      RSP_DATA_UNC,
      RSP_DATA_WRITE_UNC,
      RSP_DATA_WRITE,
      RSP_WAIT_TIME,
      RSP_UNC_WAIT_TIME,
      RSP_DATA_READ_ERROR_WAIT,
      RSP_DATA_READ_ERROR,
      RSP_DATA_MISS_OK,
      RSP_DATA_UNC_OK,
      RSP_DATA_WRITE_ERROR,
      RSP_DATA_WRITE_ERROR_WAIT,
    };

    enum {
      READ_PKTID,
      WRITE_PKTID,
    };

  public:

    soclib::tlmt::VciInitiator<vci_param> p_vci;
    tlmt_core::tlmt_in<bool> *p_irq;

  private:
    
    FILE *      pFile;
    uint32_t    m_id;
    iss_t       m_iss;

    genericCache<vci_param> m_dcache ;
    genericCache<vci_param> m_icache ;
    soclib::common::AddressDecodingTable<typename vci_param::addr_t, bool> m_cacheability_table;

 
    //VCI COMMUNICATION
    soclib::tlmt::vci_cmd_packet<vci_param>   m_cmd;
    sc_core::sc_event                         m_rsp_received;
    tlmt_core::tlmt_time                      m_rsp_vci_time;
    tlmt_core::tlmt_thread_context            c0;
    size_t                                    m_counter;
    size_t                                    m_lookahead;
    size_t                                    m_simulation_time;

    //BUFFER OF READ AND WRITE DATA
    buffer<iss_t,vci_param> m_buf;

    //BUFFERS OF VCI PACKETS 
    typename vci_param::data_t m_write_buffer[MAXIMUM_PACKET_SIZE];
    typename vci_param::data_t m_read_buffer[MAXIMUM_PACKET_SIZE];
    typename vci_param::data_t m_read_buffer_ins[MAXIMUM_PACKET_SIZE];

    // CONTROL STATE MACHINE VARIABLES
    int                    m_dcache_fsm;
    int                    m_icache_fsm;
    int                    m_vci_cmd_fsm;
    int                    m_vci_rsp_fsm;

    bool                   m_icache_req;
    bool                   m_dcache_miss_req;
    bool                   m_dcache_unc_req;
    bool                   m_dcache_unc_valid;    

    bool                   m_vci_write;
    bool                   m_icache_frz;
    bool                   m_dcache_frz;

    bool                   m_read_error;
    bool                   m_write_error;
    
    tlmt_core::tlmt_time   m_req_icache_time;

    size_t                 m_cpt_lookhead;  
    size_t                 m_cpt_idle;

    size_t                 m_icache_cpt_init;
    size_t                 m_icache_cpt_cache_read;
    size_t                 m_icache_cpt_uncache_read;

    size_t                 m_dcache_cpt_init;
    size_t                 m_dcache_cpt_cache_read;
    size_t                 m_dcache_cpt_uncache_read;
    size_t                 m_dcache_cpt_cache_write;
    size_t                 m_dcache_cpt_uncache_write;

    size_t                 m_cpt_fifo_read;
    size_t                 m_cpt_fifo_write;

  protected:
    SC_HAS_PROCESS(VciXcacheWrapper);

  public:
    VciXcacheWrapper(sc_core::sc_module_name name,
		     const soclib::common::IntTab &index,
		     const soclib::common::MappingTable &mt,
		     size_t icache_lines,
		     size_t icache_words,
		     size_t dcache_lines,
		     size_t dcache_words );

    VciXcacheWrapper(sc_core::sc_module_name name,
		     const soclib::common::IntTab &index,
		     const soclib::common::MappingTable &mt,
		     size_t icache_lines,
		     size_t icache_words,
		     size_t dcache_lines,
		     size_t dcache_words,
		     size_t simulation_time);

    size_t getTotalCycles();
    size_t getActiveCycles();
    size_t getIdleCycles();
    size_t getNLookhead();
    size_t getNIcache_Cache_Read();
    size_t getNIcache_Uncache_Read();
    size_t getNDcache_Cache_Read();
    size_t getNDcache_Uncache_Read();
    size_t getNDcache_Cache_Write();
    size_t getNDcache_Uncache_Write();
    size_t getNFifo_Read();
    size_t getNFifo_Write();
    size_t getNTotal_Cache_Read();
    size_t getNTotal_Uncache_Read();
    size_t getNTotal_Cache_Write();
    size_t getNTotal_Uncache_Write();

  private:

    void rspReceived(soclib::tlmt:: vci_rsp_packet < vci_param > *pkt,
		     const tlmt_core:: tlmt_time & time, void *private_data);
    
    void irqReceived(bool,
		     const tlmt_core::tlmt_time &time,
		     void *private_data);

    void update_time(tlmt_core::tlmt_time t);

    void add_time(tlmt_core::tlmt_time t);

    void xcacheAccess(bool &ins_asked,
                      uint32_t &ins_addr,
                      bool &mem_asked,
                      enum iss_t::DataAccessType &mem_type,
                      uint32_t &mem_addr,
                      uint32_t &mem_wdata,
                      uint32_t &mem_rdata,
                      bool &mem_dber,
                      uint32_t &ins_rdata,
                      bool &ins_iber
                      );

    void execLoop();

  };

}}

