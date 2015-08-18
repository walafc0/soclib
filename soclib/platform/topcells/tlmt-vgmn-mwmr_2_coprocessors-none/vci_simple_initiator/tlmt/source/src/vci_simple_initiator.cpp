#include "../include/vci_simple_initiator.h"

#define RAM_BASE           0xBFC00000
#define MWMR0_BASE         0x10000000
#define MWMR1_BASE         0xC0200000
#define CHANNEL_DEPTH      0x00000100  //256 bytes = 64 positions

#ifndef SIMPLE_INITIATOR_DEBUG
#define SIMPLE_INITIATOR_DEBUG 0
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciSimpleInitiator<vci_param>

  tmpl(void)::rspReceived(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
			  const tlmt_core::tlmt_time &time,
			  void *private_data)
  {
    c0.update_time(time);
    m_vci_event.notify(sc_core::SC_ZERO_TIME);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURE CHANNEL STATUS 
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::configureChannelStatus(int n_channels) 
  {
    typename vci_param::addr_t current_address = RAM_BASE;
    typename vci_param::data_t localbuf[32];
    uint32_t depth = CHANNEL_DEPTH;

#if SIMPLE_INITIATOR_DEBUG
    std::cout << "[ CONFIGURE STATUS FIFO ]" << std::endl;
#endif

    for (int i = 0; i < n_channels; i++){
      localbuf[0] = 0; // read index  (read pointer)
      localbuf[1] = 0; // write index (write pointer)
      localbuf[2] = 0; // content     (number of elements in the channel)
      localbuf[3] = 0; // lock

      m_cmd.cmd     = vci_param::CMD_WRITE;
      m_cmd.nwords  = 4;
      m_cmd.address = current_address;
      m_cmd.buf     = localbuf; 
      m_cmd.srcid   = m_srcid; 
      m_cmd.trdid   = 0;
      m_cmd.pktid   = m_pktid;
      m_cmd.be      = 0xF;
      m_cmd.contig  = true;

#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator " << m_srcid << "] Send packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif

      p_vci.send(&m_cmd, c0.time());
      sc_core::wait(m_vci_event);

#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif
      m_pktid++;

      current_address += (4*vci_param::nbytes); //positions correspond to descriptor status
      current_address += depth;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURE THE MWMR
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::configureMwmr(typename vci_param::addr_t address, int n_read_channels, int n_write_channels, int read_depth, int write_depth)
  {
    typename vci_param::addr_t current_address;
    typename vci_param::data_t localbuf[32];

    address += 0x00000044; // WAY
    current_address = RAM_BASE;

    //CHANNELS FROM COPROCESSEUR
    for (int i = 0; i < n_read_channels; i++){
      localbuf[0] = 0;                      // 0 = MWMR_FROM_COPROC = READ
      localbuf[1] = i;                      // NO
      localbuf[2] = current_address;        // STATUS_ADDRESS
    
      current_address += (4*vci_param::nbytes);    // 4 words to status registers

      localbuf[3] = read_depth;             // DEPTH
      localbuf[4] = current_address;        // BASE_ADDRESS 
      localbuf[5] = 0x00000000;             // LOCK
      localbuf[6] = 0x00000001;             // RUNNING 

      current_address += read_depth;        // n bytes to fifo

      m_cmd.cmd     = vci_param::CMD_WRITE;
      m_cmd.nwords  = 7;
      m_cmd.address = address;
      m_cmd.buf     = localbuf;
      m_cmd.srcid   = m_srcid;
      m_cmd.trdid   = 0;
      m_cmd.pktid   = m_pktid;
      m_cmd.contig  = true;

#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator " << m_srcid << "] Send packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif
    
      p_vci.send(&m_cmd, c0.time());
      sc_core::wait(m_vci_event);

#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif
      m_pktid++;

    }

    current_address = RAM_BASE;

    //CHANNELS TO COPROCESSEUR
    for (int i = 0; i < n_write_channels; i++){
      localbuf[0] = 1;                      // 1 = MWMR_TO_COPROC = WRITE
      localbuf[1] = i;
      localbuf[2] = current_address;        // STATUS_ADDRESS
      
      current_address += (4*vci_param::nbytes);    // 4 words to status registers

      localbuf[3] = write_depth;            // DEPTH
      localbuf[4] = current_address;        // BASE_ADDRESS 
      localbuf[5] = 0x00000000;             // LOCK
      localbuf[6] = 0x00000001;             // RUNNING 
      current_address += write_depth;       // n bytes to fifo

      m_cmd.cmd     = vci_param::CMD_WRITE;
      m_cmd.nwords  = 7;
      m_cmd.address = address;
      m_cmd.buf     = localbuf;
      m_cmd.srcid   = m_srcid;
      m_cmd.trdid   = 0;
      m_cmd.pktid   = m_pktid;
      m_cmd.contig  = true;
      
#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator " << m_srcid << "] Send packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif

      p_vci.send(&m_cmd, c0.time());
      sc_core::wait(m_vci_event);

#if SIMPLE_INITIATOR_DEBUG
      std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << c0.time() << std::endl;
#endif
      m_pktid++;

    }
  }

  tmpl(void)::behavior()
  {
    typename vci_param::addr_t address;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONFIGURE THE STATUS DESCRIPTOR IN MEMORY OF ALL CHANNELS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    configureChannelStatus(1);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONFIGURE THE MWMR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    address = MWMR0_BASE;
    configureMwmr(address,0,1,CHANNEL_DEPTH,CHANNEL_DEPTH);
    address = MWMR1_BASE;
    configureMwmr(address,1,0,CHANNEL_DEPTH,CHANNEL_DEPTH);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISABLE THE INITIATOR THREAD
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    c0.disable();
  }

  tmpl(/**/)::VciSimpleInitiator( sc_core::sc_module_name name, uint32_t index, uint32_t dt, tlmt_core::tlmt_time st )
    : soclib::tlmt::BaseModule(name),
      m_srcid(index),
      m_deltaTime(dt),
      m_simulationTime(st),
      p_vci("vci", new tlmt_core::tlmt_callback<VciSimpleInitiator,soclib::tlmt::vci_rsp_packet<vci_param> *>(this, &VciSimpleInitiator<vci_param>::rspReceived), &c0)
  {
    m_pktid = 1;
    SC_THREAD(behavior);
  }

}}
