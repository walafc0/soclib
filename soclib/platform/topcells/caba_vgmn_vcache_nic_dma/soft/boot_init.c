//////////////////////////////////////////////////////////////////////////////////
// File     : boot_init.c
// Date     : 01/11/2012
// Author   : alain greiner
// Copyright (c) UPMC-LIP6
///////////////////////////////////////////////////////////////////////////////////

#include "mips32_registers.h"
#include "multi_nic.h"
#include "chbuf_dma.h"
#include "tty.h"

#define NIC_CHANNELS    2

typedef struct _ld_symbol_s _ld_symbol_t;

extern _ld_symbol_t pseg_tty_base;
extern _ld_symbol_t pseg_dma_base;
extern _ld_symbol_t pseg_nic_base;

//////////////////////////////////////////////////////////////////////////////
// boot_procid()
//////////////////////////////////////////////////////////////////////////////
inline unsigned int boot_procid()
{
    unsigned int ret;
    asm volatile("mfc0 %0, $15, 1" : "=r"(ret));
    return (ret & 0x3FF);
}
//////////////////////////////////////////////////////////////////////////////
// boot_proctime()
//////////////////////////////////////////////////////////////////////////////
inline unsigned int boot_proctime()
{
    unsigned int ret;
    asm volatile("mfc0 %0, $9" : "=r"(ret));
    return ret;
}
//////////////////////////////////////////////////////////////////////////////
// boot_exit()
//////////////////////////////////////////////////////////////////////////////
void boot_exit()
{
    while(1) asm volatile("nop");
}
//////////////////////////////////////////////////////////////////////////////
// boot_eret()
// The address of this function is used to initialise the return address (RA)
// in all task contexts (when the task has never been executed.
/////////////////////////////////"/////////////////////////////////////////////
void boot_eret()
{
    asm volatile("eret");
}
//////////////////////////////////////////////////////////////////////////////
// boot_set_mmu_mode()
// This function set a new value for the MMU MODE register.
//////////////////////////////////////////////////////////////////////////////
inline void boot_set_mmu_mode( unsigned int val )
{
    asm volatile("mtc2  %0, $1" : : "r"(val) );
}
////////////////////////////////////////////////////////////////////////////
// boot_puts()
// (it uses TTY0)
////////////////////////////////////////////////////////////////////////////
void boot_puts(const char *buffer)
{
    unsigned int* tty_address = (unsigned int*)(&pseg_tty_base);
    unsigned int n;

    for ( n=0; n<100; n++)
    {
        if (buffer[n] == 0) break;
        tty_address[TTY_WRITE] = (unsigned int)buffer[n];
    }
}
////////////////////////////////////////////////////////////////////////////
// boot_putx()
// (it uses TTY0)
////////////////////////////////////////////////////////////////////////////
void boot_putx(unsigned int val)
{
    static const char   HexaTab[] = "0123456789ABCDEF";
    char                buf[11];
    unsigned int        c;

    buf[0]  = '0';
    buf[1]  = 'x';
    buf[10] = 0;

    for ( c = 0 ; c < 8 ; c++ )
    {
        buf[9-c] = HexaTab[val&0xF];
        val = val >> 4;
    }
    boot_puts(buf);
}
////////////////////////////////////////////////////////////////////////////
// boot_putd()
// (it uses TTY0)
////////////////////////////////////////////////////////////////////////////
void boot_putd(unsigned int val)
{
    static const char   DecTab[] = "0123456789";
    char                buf[11];
    unsigned int        i;
    unsigned int        first;

    buf[10] = 0;

    for ( i = 0 ; i < 10 ; i++ )
    {
        if ((val != 0) || (i == 0))
        {
            buf[9-i] = DecTab[val % 10];
            first    = 9-i;
        }
        else
        {
            break;
        }
        val /= 10;
    }
    boot_puts( &buf[first] );
}
//////////////////////////////////////////////////////////////////////////////
// This function initialises both the global registers (hypervisor)
// and the channel registers for the vci_multi_nic component.
//////////////////////////////////////////////////////////////////////////////
void boot_activate_nic(unsigned int channels)
{
    unsigned int*    nic_hyper_base;
    unsigned int*    nic_channel_base;
    unsigned int     k;
    unsigned int     active;
    unsigned int     nb_chan = 0;
    unsigned int     i = 0;

    boot_puts( "NIC activation at cycle ");
    boot_putd( boot_proctime() );
    boot_puts( "\n" );

    // set channels registers (configuring channels)
    for ( k = 0 ; k < channels ; k++)
        {
            nic_channel_base = (unsigned int*)(&pseg_nic_base) + NIC_CHANNEL_SPAN*k;
            nic_channel_base[NIC_RX_DESC_LO_0 + 4096] = (unsigned int)(nic_channel_base);
            nic_channel_base[NIC_RX_DESC_LO_1 + 4096] = (unsigned int)(nic_channel_base + 1024);
            nic_channel_base[NIC_TX_DESC_LO_0 + 4096] = (unsigned int)(nic_channel_base + 2048);
            nic_channel_base[NIC_TX_DESC_LO_1 + 4096] = (unsigned int)(nic_channel_base + 3072);
            nic_channel_base[NIC_RX_RUN    + 4096] = 1;
            nic_channel_base[NIC_TX_RUN    + 4096] = 1;
        }

    // set global registers
    nic_hyper_base = (unsigned int*)(&pseg_nic_base) + NIC_CHANNEL_SPAN * 8;

    for ( k = 0 ; k < channels ; k++)
        {
            nic_hyper_base[NIC_G_MAC_4 + k]   = 0x00000001 + k;
            nic_hyper_base[NIC_G_MAC_2 + k]   = 0x0000FFFF;
        }
    
    boot_puts("[NIC] channels from param = ");
    boot_putd( channels );
    boot_puts( "\n" );

    nb_chan = nic_hyper_base[NIC_G_NB_CHAN];

    boot_puts("[NIC] channels from NIC = ");
    boot_putd( nb_chan );
    boot_puts( "\n" );

    for (i = 0; i < channels; i++)
        active |= (0x1 << i);

    // Activate NIC
    nic_hyper_base[NIC_G_BYPASS_ENABLE]   = 0x0;
    nic_hyper_base[NIC_G_BC_ENABLE]       = 0x1;
    nic_hyper_base[NIC_G_VIS]             = active;
    nic_hyper_base[NIC_G_ON]              = 0x1;

} // end boot_activate_nic()

//////////////////////////////////////////////////////////////////////////////
// This function initialises the vci_chbuf_dma component.
// One DMA channel activated for each NIC channel RX_PBUF => TX_PBUF
//////////////////////////////////////////////////////////////////////////////
void boot_activate_dma(unsigned int channels)
{
    unsigned int*   dma_channel_base;
    unsigned int*   nic_channel_base;
    unsigned int    k;

    boot_puts( "DMA activation at cycle ");
    boot_putd( boot_proctime() );
    boot_puts( "\n" );

    for( k = 0 ; k < channels ; k++ )
    {
        dma_channel_base = (unsigned int*)(&pseg_dma_base) + CHBUF_CHANNEL_SPAN*k;
        nic_channel_base = (unsigned int*)(&pseg_nic_base) + NIC_CHANNEL_SPAN*k;

        // source chbuf descriptor address and number of buffers
        dma_channel_base[CHBUF_SRC_DESC]  = (unsigned int)nic_channel_base + 0x4000;
        dma_channel_base[CHBUF_SRC_NBUFS] = 2;

        // destination chbuf descriptor address and number of buffers
        dma_channel_base[CHBUF_DST_DESC]  = (unsigned int)nic_channel_base + 0x4010;
        dma_channel_base[CHBUF_DST_NBUFS] = 2;

        // set buffer size, polling period, and start
        dma_channel_base[CHBUF_BUF_SIZE]  = 1024;
        dma_channel_base[CHBUF_PERIOD]    = 30;
        dma_channel_base[CHBUF_RUN]       = 1;
    }
} // end boot_activate_dma()

/////////////////////////////////////////////////////////////////////
// This function is the entry point of the initialisation procedure
/////////////////////////////////////////////////////////////////////
void boot_init()
{
    boot_activate_dma(NIC_CHANNELS);
    boot_activate_nic(NIC_CHANNELS);
    boot_exit();
} // end boot_init()

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

