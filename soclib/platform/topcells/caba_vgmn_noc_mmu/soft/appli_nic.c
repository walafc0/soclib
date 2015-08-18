///////////////////////////////////////////////////////////////////////
// This very simple application is intended to test the 
// NOC_MMU component:
// - processor (0) is supposed to run the hypervisor code
// - others processors (k) are running the same user application.
// Each user application [k] controls directly one NIC channel,
// and two DMA channels. Each application [k] contains two user space 
// chbufs, called chbuf_in[k] and chbuf_out[k]. 
// Each application performs three concurrent stream transfers:
// - first DMA channel does the nic_rx[k] -> chbuf_in[k] transfer
// - processor [k] does the transfer chbuf_in[k] -> chbuf_out[k]
// - second DMA channel does the chbuf_out[k] -> nic_tx[k] transfer
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "multi_nic.h"
#include "chbuf_dma.h"

#define NBUFS  4
#define NWORDS 1024

typedef struct _ld_symbol_s _ld_symbol_t;

extern _ld_symbol_t pseg_dma_base;
extern _ld_symbol_t pseg_nic_base;

///////////////////////////////////////////////////////////
void activate_dma_channel( unsigned int   channel, 
                           unsigned int*  src_desc,
                           unsigned int   src_nbufs,
                           unsigned int*  dst_desc,
                           unsigned int   dst_nbufs,
                           unsigned int   buf_size,
                           unsigned int   period )
{
    unsigned int* dma_channel_base (unsigned int*)(&pseg_dma_base) 
                                   + CHBUF_CHANNEL_SPAN * channel;

    // source chbuf descriptor address and number of buffers
    dma_channel_base[CHBUF_SRC_DESC]  = src_desc;
    dma_channel_base[CHBUF_SRC_NBUFS] = src_nbufs;

    // destination chbuf descriptor address and number of buffers
    dma_channel_base[CHBUF_DST_DESC]  = dst_desc;
    dma_channel_base[CHBUF_DST_NBUFS] = dst_nbufs;

    // set buffer size, polling period, and start
    dma_channel_base[CHBUF_BUF_SIZE]  = buf_size;
    dma_channel_base[CHBUF_PERIOD]    = period;
    dma_channel_base[CHBUF_RUN]       = 1;
}

/////////////////////////////////////////////////
void activate_nic_channel( unsigned int channel )
{
    unsigned int* nic_channel_base (unsigned int*)(&pseg_nic_base) 
                                   + NIC_CHANNEL_SPAN * channel;

    // TX chbuf descriptor
    nic_channel_base[NIC_RX_FULL_0 + 4096] = 0;
    nic_channel_base[NIC_RX_PBUF_0 + 4096] = (unsigned int)(nic-channel_base);
    nic_channel_base[NIC_RX_FULL_1 + 4096] = 0;
    nic_channel_base[NIC_RX_PBUF_1 + 4096] = (unsigned int)(nic-channel_base + 1024);

    // RX chbuf descriptor
    nic_channel_base[NIC_TX_FULL_0 + 4096] = 0;
    nic_channel_base[NIC_TX_PBUF_0 + 4096] = (unsigned int)(nic-channel_base + 2048);
    nic_channel_base[NIC_TX_FULL_1 + 4096] = 0;
    nic_channel_base[NIC_TX_PBUF_1 + 4096] = (unsigned int)(nic-channel_base + 3072);

    // activation
    nic_channel_base[NIC_RX_RUN    + 4096] = 1;
    nic_channel_base[NIC_TX_RUN    + 4096] = 1;
}

////////////////////////////////////////////////////
void activate_soft_transfert( unsigned int* src_desc,
                              unsigned int  src_nbufs,
                              unsigned int* dst_desc,
                              unsigned int  dst_nbufs,
                              unsigned int  buf_words )
{
    volatile unsigned int  src_full;
    volatile unsigned int* src_buf;
    volatile unsigned int  dst_full;
    volatile unsigned int* dst_buf;

    unsigned int word;

    unsigned int src_index = 0;
    unsigned int dst_index = 0;

    // infinite loop
    while (1)
    {
        // waiting next src_buf full
        while ( (src_full = src_desc[2*src_index]) == 0 ) { asm volatile("nop"); }

        // get src_buf address
        src_buf = (unsigned int*)src_desc[2*src_index+1];

        // waiting next dst_buf empty
        while ( (dst_full = dst_desc[2*dst_index]) != 0 ) { asm volatile("nop"); }

        // get src_buf address
        dst_buf = (unsigned int*)dst_desc[2*dst_index+1];

        // transfer a buffer
        for ( word = 0 ; word < buf_words ; word++ ) { dst_buf[word] = src_buf[word]; }
         
        // update src_index & dst index
        src_index = (src_index + 1) % src_nbufs;
        dst_index = (dst_index + 1) % dst_nbufs;
    }
}
 
///////////
void main()
{
    unsigned int chbuf_in[NBUFS][NWORDS];
    unsigned int chbuf_in_desc[2*NBUFS];
    
    unsigned int chbuf_out[NBUFS][NWORDS];
    unsigned int chbuf_out_desc[2*NBUFS];

    // Each NIC application running on a different processor
    // uses one nic channel, and two dma channels
    // processor 0 is running the GIET
    unsigned int nic_channel = procid() - 1;
    unsigned int dma_channel = nic_channel*2;

    // get nic_rx_desc and nic_tx_desc base addresses
    unsigned int* nic_channel_base (unsigned int*)(&pseg_nic_base) 
                                   + NIC_CHANNEL_SPAN * channel;
    unsigned int* nic_rx_desc = &nic_channel_base[NIC_RX_FULL_0 + 4096];
    unsigned int* nic_tx_desc = &nic_channel_base[NIC_TX_FULL_0 + 4096];

    // start nic channel
    actvate_nic_channel( nic_channel );

    // start first dma channel: nic_rx -> chbuf_in 
    activate_dma_channel( dma_channel,
                          nic_rx_desc,      // src_desc
                          2,                // src_nbufs
                          chbuf_in_desc,    // dst_desc
                          NBUFS,            // dst_nbufs
                          NWORDS,           // buf size
                          50 )              // polling period 
                          
    // start second dma channel: chbuf_out -> nic_tx 
    activate_dma_channel( dma_channel + 1,
                          chbuf_out_desc,   // src_desc
                          NBUFS,            // src_nbufs
                          nic_tx_desc,      // dst_desc
                          2,                // dst_nbufs
                          NWORDS,           // buf size
                          50 )              // polling period 

    // start soft transfer: chbuf_in -> chbuf_out
    activate_soft_transfer( chbuf_in_desc,  // src_desc
                            NBUFS,          // src_nbufs
                            chbuf_out_desc, // dst_desc
                            NBUFS,          // dst_nbufs
                            NWORDS );       // buf size
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

