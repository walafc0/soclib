/*
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
 */

#ifndef AHCI_SDC_H
#define AHCI_SDC_H

/////////////////////////////////////////////////////////////////////////////
// SDC Addressable Registers (up to 64 registers)
/////////////////////////////////////////////////////////////////////////////

enum SoclibSdcRegisters
{
    SDC_PERIOD       = 32,          // system cycles    / Write-Only
    SDC_CMD_ID       = 33,          // command index    / Write-Only
    SDC_CMD_ARG      = 34,          // command argument / Write-Only
    SDC_RSP_STS      = 35,          // response status  / Read)Only
};

enum SoclibSdcCommands
{
    SDC_CMD0         = 0,           // Soft reset
    SDC_CMD3         = 3,           // Relative Card Address
    SDC_CMD7         = 7,           // Toggle mode
    SDC_CMD8         = 8,           // Voltage info 
    SDC_CMD17        = 17,          // RX single block (hidden command)
    SDC_CMD24        = 24,          // TX single block (hidden command)
    SDC_CMD41        = 41,          // Operation Condition
};

enum SoclibSdcErrorCodes
{
    SDC_ERROR_LBA    = 0x40000000,  // LBA larger tnan SD card capacity
    SDC_ERROR_CRC    = 0x00800000,  // CRC error reported by SD card
    SDC_ERROR_CMD    = 0x00400000,  // command notsupported by SD card
};
           
/////////////////////////////////////////////////////////////////////////////
// AHCI Addressable Registers 
/////////////////////////////////////////////////////////////////////////////

enum SoclibAhciRegisters 
{
    AHCI_PXCLB       = 0,           // command list base address 32 LSB bits
    AHCI_PXCLBU      = 1,           // command list base address 32 MSB bits
    AHCI_PXIS        = 4,           // interrupt status
    AHCI_PXIE        = 5,           // interrupt enable
    AHCI_PXCMD       = 6,           // run
    AHCI_PXCI        = 14,          // command bit-vector     
};

/////////////////////////////////////////////////////////////////////////////
// AHCI structures for Command List
/////////////////////////////////////////////////////////////////////////////

/////// command descriptor  ///////////////////////
typedef struct ahci_cmd_desc_s  // size = 16 bytes
{
    unsigned char       flag[2];    // W in bit 6 of flag[0]
    unsigned char       prdtl[2];	// Number of buffers
    unsigned int        prdbc;		// Number of bytes actually transfered
    unsigned int        ctba;		// Command Table base address 32 LSB bits
    unsigned int        ctbau;		// Command Table base address 32 MSB bits
} ahci_cmd_desc_t;


/////////////////////////////////////////////////////////////////////////////
// AHCI structures for Command Table
/////////////////////////////////////////////////////////////////////////////

/////// command header  ///////////////////////////
typedef struct ahci_cmd_header_s     // size = 16 bytes
{
    unsigned int        res0;       // reserved	
    unsigned char	    lba0;	    // LBA 7:0
    unsigned char	    lba1;	    // LBA 15:8
    unsigned char	    lba2;	    // LBA 23:16
    unsigned char	    res1;	    // reserved
    unsigned char	    lba3;	    // LBA 31:24
    unsigned char	    lba4;	    // LBA 39:32
    unsigned char	    lba5;	    // LBA 47:40
    unsigned char	    res2;	    // reserved
    unsigned int        res3;       // reserved	
} ahci_cmd_header_t;

/////// Buffer Descriptor ////////////////////////
typedef struct ahci_cmd_buffer_s // size = 16 bytes
{
    unsigned int        dba;	    // Buffer base address 32 LSB bits
    unsigned int        dbau;	    // Buffer base address 32 MSB bits
    unsigned int        res0;	    // reserved
    unsigned int        dbc;	    // Buffer bytes count
} ahci_cmd_buffer_t;

/////// command table //////////////////////////////
typedef struct ahci_cmd_table_s  // size = 4 Kbytes
{
    ahci_cmd_header_t   header;      // contains LBA
    ahci_cmd_buffer_t   buffer[255]; // 255 buffers max
} ahci_cmd_table_t;


#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

