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
#ifndef MULTI_HBA_REGS_H
#define MULTI_HBA_REGS_H


/////////////////////////////////////////////////////////////////////////////
// Addressable Registers (for each channel)
/////////////////////////////////////////////////////////////////////////////

enum SoclibMultiAhciRegisters 
{
  HBA_PXCLB            = 0,         // command list base address 32 LSB bits
  HBA_PXCLBU           = 1,         // command list base address 32 MSB bits
  HBA_PXIS             = 4,         // interrupt status
  HBA_PXIE             = 5,         // interrupt enable
  HBA_PXCMD            = 6,         // run
  HBA_PXCI             = 14,        // command bit-vector     
  HBA_SPAN             = 0x400,     // 4 Kbytes per channel => 1024 slots
};

/////////////////////////////////////////////////////////////////////////////
// C structures for Command List
/////////////////////////////////////////////////////////////////////////////

/////// command descriptor  ///////////////////////
typedef struct hba_cmd_desc_s  // size = 16 bytes
{
	// WORD 0
    unsigned char       flag[2];    // W in bit 6 of flag[0]
    unsigned char       prdtl[2];	// Number of buffers

    // WORD 1
    unsigned int        prdbc;		// Number of bytes actually transfered

    // WORD 2, WORD 3
    unsigned int        ctba;		// Command Table base address 32 LSB bits
    unsigned int        ctbau;		// Command Table base address 32 MSB bits

} hba_cmd_desc_t;


/////////////////////////////////////////////////////////////////////////////
// C structures for Command Table
/////////////////////////////////////////////////////////////////////////////

/////// command header  ///////////////////////////
typedef struct hba_cmd_header_s     // size = 16 bytes
{
    // WORD 0
    unsigned int        res0;       // reserved	
  
    // WORD 1
    unsigned char	    lba0;	    // LBA 7:0
    unsigned char	    lba1;	    // LBA 15:8
    unsigned char	    lba2;	    // LBA 23:16
    unsigned char	    res1;	    // reserved
  
    // WORD 2
    unsigned char	    lba3;	    // LBA 31:24
    unsigned char	    lba4;	    // LBA 39:32
    unsigned char	    lba5;	    // LBA 47:40
    unsigned char	    res2;	    // reserved
  
    // WORD 3 to 31
    unsigned int        res3;       // reserved	

} hba_cmd_header_t;

/////// Buffer Descriptor ////////////////////////
typedef struct hba_cmd_buffer_s // size = 16 bytes
{
    unsigned int        dba;	    // Buffer base address 32 LSB bits
    unsigned int        dbau;	    // Buffer base address 32 MSB bits
    unsigned int        res0;	    // reserved
    unsigned int        dbc;	    // Buffer bytes count

} hba_cmd_buffer_t;

/////// command table //////////////////////////////
typedef struct hba_cmd_table_s  // size = 4 Kbytes
{

    hba_cmd_header_t   header;      // contains LBA
    hba_cmd_buffer_t   buffer[255]; // 255 buffers max

} hba_cmd_table_t;

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

