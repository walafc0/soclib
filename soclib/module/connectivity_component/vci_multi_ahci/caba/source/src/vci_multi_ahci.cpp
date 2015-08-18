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
 * Copyright (c) UPMC, Lip6, Asim
 * alain.greiner@lip6.fr / xiaowu.zhang@lip6.fr september 2013
 *    
 * Maintainers: alain
 */

//////////////////////////////////////////////////////////////////////////////////
//  Implementation notes :
//
//  This component contains four FSM types :
//  - the target_fsm controls the configuration commands and responses 
//    on the VCI target port.
//  - the cmd_fsm controls the read and write DMA commands
//    on the VCI initiator port.
//  - the rsp_fsm controls the read and write DMA responses
//    on the VCI initiator port.
//  - the channel_fsm[k] controls the transfers for channel [k],
//    accessing the Command List and Command Table associated to channel k.
//
//  For all VCI bursts, the burst length is multiple of 8 bytes:
//  - read Command Descriptor:    16 bytes
//  - read Command Table Header:  16 bytes
//  - read Buffer Descriptor:     16 bytes
//  - read or write Data:         burst size (8, 16, 32, or 64 bytes)
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include "alloc_elems.h"
#include "vci_multi_ahci.h"
#include "multi_ahci.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciMultiAhci<vci_param>

using namespace soclib::caba;
using namespace soclib::common;
using namespace std;

////////////////////////
tmpl(void)::transition()
{
   
    if( p_resetn.read() == false )
    {
        r_tgt_fsm       = T_IDLE;
        r_cmd_fsm       = CMD_IDLE;
        r_rsp_fsm       = RSP_IDLE;

        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            r_channel_fsm[k]      = CHANNEL_IDLE;
            r_channel_run[k]      = false;
            r_channel_slot[k]     = 0;
            r_channel_done[k]     = false;
            r_channel_error[k]    = false;
            r_channel_latency[k]  = m_latency;
        }
        return;
    } 
    
    ///////////////////////////////////////
    for ( size_t k=0 ; k<m_channels ; k++ )
    {
        switch(r_channel_fsm[k].read())
        {
        //////////////////
        case CHANNEL_IDLE:    // waiting a valid command in Command List
        {
            uint32_t pxci = r_channel_pxci[k].read();
            if ( pxci != 0 and r_channel_run[k].read() )  // Command List not empty
            {
                // select a command with round-robin priority
                // last served command has the lowest priority
                unsigned int cmd_id;

                for ( size_t n = 0 ; n < 32 ; n++ )
                {
                    cmd_id = (r_channel_slot[k].read() + n + 1) % 32;
                    if ( pxci & (1 << cmd_id) )
                    {
                        r_channel_slot[k] = cmd_id;
                        break;
                    }
                }

                // request VCI read for Command Descriptor
                r_channel_address[k] = ( ((uint64_t)(r_channel_pxclbu[k].read()))<<32 | 
                                          r_channel_pxclb[k].read() )
                                          + cmd_id * sizeof(hba_cmd_desc_t);
                r_channel_length[k]  =   sizeof(hba_cmd_desc_t);
                r_channel_word[k]    =   0;
                r_channel_prdbc[k]   =   0;   
                r_channel_fsm[k]     =   CHANNEL_READ_CMD_DESC_CMD;
            }
            break;
        }
        ////////////////////////////////
        case CHANNEL_READ_CMD_DESC_CMD:   // waiting cmd_fsm acknowledge for command descriptor
        {
            if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) ) // command posted
            {
                r_channel_fsm[k] = CHANNEL_READ_CMD_DESC_RSP;
            }
            break;
        }
        ////////////////////////////////
        case CHANNEL_READ_CMD_DESC_RSP:  // waiting rsp_fsm response for command descriptor
        {
            if ( r_channel_done[k] ) 
            {
                if ( r_channel_error[k] )
                {
                    r_channel_fsm[k] = CHANNEL_ERROR;
                }
                else
                {
                    hba_cmd_desc_t* desc = (hba_cmd_desc_t*)r_channel_buffer[k];
                    r_channel_prdtl[k] = (desc->prdtl[1]<<8) | desc->prdtl[0];
                    r_channel_write[k] = desc->flag[0] & 0x40;
                    r_channel_ctba[k]  = (((uint64_t)desc->ctbau)<<32) | desc->ctba;
                    
                    // prepare VCI read for Command Table Header
                    r_channel_address[k] = (((uint64_t)desc->ctbau) <<32) | desc->ctba;
                    r_channel_length[k]  = 16;  
                    r_channel_word[k]    = 0;
                    r_channel_fsm[k]     = CHANNEL_READ_LBA_CMD;
                }
                r_channel_done[k] = false;
            }
            break;
        }
        //////////////////////////            
        case CHANNEL_READ_LBA_CMD:   // waiting cmd_fsm acknowledge for lba read
        {
            if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) ) // command posted
            {
                r_channel_fsm[k] = CHANNEL_READ_LBA_RSP;
            }
            break;
        }
        //////////////////////////
        case CHANNEL_READ_LBA_RSP:   // waiting rsp_fsm respons for lba read
        {
            if ( r_channel_done[k] ) 
            {
                if ( r_channel_error[k] )
                {
                    r_channel_fsm[k] = CHANNEL_ERROR;
                }
                else
                {
                    hba_cmd_header_t* head = (hba_cmd_header_t*)r_channel_buffer[k];
                    r_channel_lba[k] = (((uint64_t)head->lba5)<<40)|(((uint64_t)head->lba4)<<32)|
                                       (((uint64_t)head->lba3)<<24)|(((uint64_t)head->lba2)<<16)|
                                       (((uint64_t)head->lba1)<<8 )|(((uint64_t)head->lba0));

                    // initialize the buffer counter 
                    r_channel_bufid[k] = 0; 
                    r_channel_fsm[k]   = CHANNEL_READ_BUF_DESC_START;
                }
                r_channel_done[k]=false;
            }
            break;
        }
        /////////////////////////////////
        case CHANNEL_READ_BUF_DESC_START:   // entering loop on buffer descriptors
        {
            // prepare VCI read for buffer descriptor read
            r_channel_address[k] = r_channel_ctba[k].read() + sizeof(hba_cmd_header_t) +
                                   sizeof(hba_cmd_buffer_t)*r_channel_bufid[k].read();
            r_channel_length[k]  = sizeof(hba_cmd_buffer_t);
            r_channel_word[k]    = 0;
            r_channel_fsm[k]     = CHANNEL_READ_BUF_DESC_CMD;
            break;
        }
        ///////////////////////////////
        case CHANNEL_READ_BUF_DESC_CMD:   // waiting cmd_fsm acknowledge for descriptor read
        {
            if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) )   // command posted
            {
                r_channel_fsm[k] = CHANNEL_READ_BUF_DESC_RSP;
            }
            break;
        }
        ///////////////////////////////
        case CHANNEL_READ_BUF_DESC_RSP:  // waiting rsp_fsm response for descriptor read
        {
            if ( r_channel_done[k] ) 
            {
                if ( r_channel_error[k] )
                {
                    r_channel_fsm[k] = CHANNEL_ERROR;
                }
                else
                {
                    hba_cmd_buffer_t* pbuf = (hba_cmd_buffer_t*)r_channel_buffer[k];

                    assert( ((pbuf->dba & ((m_words_per_burst<<2)-1)) == 0) and 
                    "VCI_MULTI_AHCI ERROR: buffer address not burst aligned");

                    assert( ((pbuf->dbc & ((m_words_per_block<<2)-1)) == 0) and 
                    "VCI_MULTI_AHCI ERROR: buffer length not multiple of block size");

                    r_channel_dba[k]    = (((uint64_t)pbuf->dbau)<<32) | pbuf->dba;
                    r_channel_dbc[k]    = pbuf->dbc;
                    r_channel_bursts[k] = 0;
                    r_channel_blocks[k] = 0;    
                    r_channel_word[k]   = 0;  

                    if( r_channel_write[k].read() ) r_channel_fsm[k] = CHANNEL_READ_START;
                    else                            r_channel_fsm[k] = CHANNEL_READ_BLOCK;
                }
                r_channel_done[k] = false;
            }
            break;
        }
        ////////////////////////
        case CHANNEL_READ_START:   // prepare data burst read
        {
            r_channel_address[k] = r_channel_dba[k].read();
            r_channel_length[k]  = m_words_per_burst<<2;
            r_channel_fsm[k]     = CHANNEL_READ_CMD;
            break;
        }
        //////////////////////
        case CHANNEL_READ_CMD:    // waiting cmd_fsm acknowledge for VCI burst read
        {
            if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) )  //command posted
            {
                r_channel_dba[k] = r_channel_dba[k].read() + r_channel_length[k].read(); 
                r_channel_fsm[k] = CHANNEL_READ_RSP;
            }
            break;
        }
        //////////////////////    
        case CHANNEL_READ_RSP:    // waiting rsp_fsm response for VCI burst read
       {   
            if ( r_channel_done[k] ) 
            {
                if ( r_channel_error[k] )
                {
                    r_channel_fsm[k] = CHANNEL_ERROR;
                }
                else if ( r_channel_bursts[k].read() == (m_bursts_per_block-1) )
                {
                    r_channel_fsm[k]  = CHANNEL_WRITE_BLOCK;
                }
                else 
                {
                    r_channel_bursts[k] = r_channel_bursts[k].read() + 1;
                    r_channel_fsm[k] = CHANNEL_READ_START;
                }
                r_channel_done[k] = false;
            }
            break;
        }
        /////////////////////////
        case CHANNEL_WRITE_BLOCK:   // write one block to block device
        {
            if ( r_channel_latency[k].read() == 0 )
            {
                r_channel_latency[k] = m_latency;

                uint64_t seek = (r_channel_lba[k].read() + r_channel_blocks[k].read()) *
                                (m_words_per_block<<2) ;

                assert( (::lseek( m_fd[k], seek, SEEK_SET ) >= 0 ) and
                "VCI_MULTI_AHCI ERROR: cannot access virtual disk");

                assert( (::write( m_fd[k], r_channel_buffer[k], m_words_per_block<<2) >= 0 ) and
                "VCI_MULTI_AHCI ERROR: cannot write on virtual disk");

                if ( (r_channel_blocks[k].read()+1)*(m_words_per_block<<2) == r_channel_dbc[k] )
                {
                    r_channel_fsm[k] = CHANNEL_TEST_LENGTH;
                }
                else
                {
                    r_channel_bursts[k] = 0;
                    r_channel_word[k]   = 0;    
                    r_channel_prdbc[k]  = r_channel_prdbc[k].read() + 1;
                    r_channel_blocks[k] = r_channel_blocks[k].read() + 1;
                    r_channel_fsm[k]    = CHANNEL_READ_START;
                }
            }
            else
            {
                r_channel_latency[k] = r_channel_latency[k] - 1;
            }
            break;
        }
        /////////////////////////
        case CHANNEL_TEST_LENGTH:
        {
            if( r_channel_bufid[k].read() == (r_channel_prdtl[k].read()-1) ) // last buffer
            {
                r_channel_fsm[k] = CHANNEL_SUCCESS;
            }
            else
            {
                r_channel_bufid[k] = r_channel_bufid[k].read()+1;
                r_channel_prdbc[k] = r_channel_prdbc[k].read() + 1;
                r_channel_fsm[k]   = CHANNEL_READ_BUF_DESC_START;
            } 
            
            break;
        }
        ////////////////////////    
        case CHANNEL_READ_BLOCK:    // read one block from device
        {
            if( r_channel_latency[k].read() == 0 )
            {
                r_channel_latency[k] = m_latency;

                uint64_t seek = (r_channel_lba[k].read() + r_channel_blocks[k].read()) *
                                (m_words_per_block<<2) ;

                assert( ( ::lseek( m_fd[k], seek, SEEK_SET ) >= 0 ) and
                "VCI_MULTI_AHCI ERROR: cannot access virtual disk");

                assert( ( ::read( m_fd[k], 
                                  r_channel_buffer[k],
                                  m_words_per_block<<2 ) >= 0 ) and
                "VCI_MULTI_AHCI ERROR: cannot read from virtual disk");

                r_channel_word[k] = 0;  
                r_channel_fsm[k]  = CHANNEL_WRITE_START;
            }
            else
            {
                r_channel_latency[k] = r_channel_latency[k].read() - 1;
            }
            break;
        }
        /////////////////////////
        case CHANNEL_WRITE_START:   // prepare VCI burst write
        {
            r_channel_address[k] = r_channel_dba[k].read();
            r_channel_length[k]  = m_words_per_burst << 2;
            r_channel_fsm[k]     = CHANNEL_WRITE_CMD;
            break;
        }
        ///////////////////////
        case CHANNEL_WRITE_CMD:   // wait cmd_fsm acknowledge for a VCI burst write
        {
            if ( (r_cmd_fsm == CMD_WRITE) and (r_cmd_index.read() == k) )  //command posted
            {
                r_channel_dba[k] = r_channel_dba[k].read() + r_channel_length[k].read(); 
                r_channel_fsm[k] = CHANNEL_WRITE_RSP;
            }
            break;
        }
        ///////////////////////
        case CHANNEL_WRITE_RSP:    // wait response from rsp_fsm for a VCI burst write
        {
            if ( r_channel_done[k] )  // burst completed
            {
                if ( r_channel_error[k] )
                {
                    r_channel_fsm[k] = CHANNEL_ERROR;
                }
                else 
                {
                    if  ( r_channel_bursts[k].read() == (m_bursts_per_block-1) )  // last burst
                    {
                        if ( (r_channel_blocks[k].read()+1) * (m_words_per_block<<2)
                                 == r_channel_dbc[k].read() )
                        {
                            r_channel_fsm[k] = CHANNEL_TEST_LENGTH;
                        }
                        else
                        {
                            r_channel_word[k]   = 0; 
                            r_channel_bursts[k] = 0;
                            r_channel_blocks[k] = r_channel_blocks[k].read() + 1;
                            r_channel_prdbc[k]  = r_channel_prdbc[k].read() + 1;
                            r_channel_fsm[k]    = CHANNEL_READ_BLOCK;
                        }
                    }
                    else
                    {
                        r_channel_bursts[k] = r_channel_bursts[k].read() + 1;
                        r_channel_fsm[k] = CHANNEL_WRITE_START;
                    }
                }
                r_channel_done[k] = false;
            }
            break;
        }
        /////////////////////
        case CHANNEL_SUCCESS:   // set PXIS and reset current command in PXCI
        {
            if ( not p_vci_target.cmdval.read() )  // to avoid conflict with soft access
            {
                uint32_t cmd_id   = r_channel_slot[k].read();
                r_channel_slot[k] = (cmd_id + 1) % 32;
                r_channel_pxci[k] = r_channel_pxci[k].read() & (~(1<<cmd_id));
                r_channel_pxis[k] = 0x1;
                r_channel_fsm[k]  = CHANNEL_IDLE;
            }
            break;
        }
        ///////////////////
        case CHANNEL_ERROR:   // set PXIS with faulty cmd_id and buf_id
                              // and reset faulty command in PXCI
        {
            if ( not p_vci_target.cmdval.read() )  // to avoid conflict with soft access
            {
                uint32_t cmd_id   = r_channel_slot[k].read();
                uint32_t buf_id   = r_channel_bufid[k].read();
                r_channel_slot[k] = (cmd_id + 1) % 32;
                r_channel_pxci[k] = r_channel_pxci[k].read() & (~(1<<cmd_id));
                r_channel_pxis[k] = 0x40000001 | cmd_id<<24 | buf_id<<8 ;
                r_channel_fsm[k]  = CHANNEL_BLOCKED;
            }
            break;
        }
        /////////////////////
        case CHANNEL_BLOCKED:   // channel blocked: waiting software reset on PXIS
        {
            if( r_channel_pxis[k].read() == 0 )
            {
                r_channel_fsm[k] = CHANNEL_IDLE;
            }
            break;
        }
            
        }  // end switch channel_fsm
    }  // end for channels
    
    /////////////////
    switch(r_tgt_fsm) 
    {
        case T_IDLE:
        {
        if ( p_vci_target.cmdval.read() ) 
        { 
            r_tgt_srcid = p_vci_target.srcid.read();
            r_tgt_trdid = p_vci_target.trdid.read();
            r_tgt_pktid = p_vci_target.pktid.read();
          
            vci_addr_t address = p_vci_target.address.read();
            
            // get wdata for both 32 bits and 64 bits data width
            uint32_t    wdata;
            if( (vci_param::B == 8) and (p_vci_target.be.read() == 0xF0) )
                wdata = (uint32_t)(p_vci_target.wdata.read()>>32);
            else
                wdata = (uint32_t)(p_vci_target.wdata.read());

            // check segment
            bool found = false;
            std::list<soclib::common::Segment>::iterator seg;
            for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
            {
                if ( seg->contains(address) ) found = true;
            }
            
            bool        read     = (p_vci_target.cmd.read() == vci_param::CMD_READ);
            uint32_t    cell     = (uint32_t)( (address>>2) & 0x1F);
            uint32_t    channel  = (uint32_t)( (address>>12) & 0x7);
            
            if     ( not read && ((not found) or (not p_vci_target.eop.read())))
            {
                r_tgt_fsm = T_WRITE_ERROR;
            }
            else if(  read && ((not found) or (not p_vci_target.eop.read()))) 
            {
                r_tgt_fsm = T_READ_ERROR;
            }
            else if( not read )         // write command 
            {
                if( cell == HBA_PXCLB )
                {
                    assert( (r_channel_pxclb[channel].read() & 0x3f)==0 and 
                    "VCI_MULTI_AHCI ERROR: command list address must be 64 bytes aligned");

                    r_channel_pxclb[channel] = wdata; 
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else if( cell == HBA_PXCLBU )
                {
                    r_channel_pxclbu[channel] = wdata; 
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else if( cell == HBA_PXIS )
                {
                    r_channel_pxis[channel] = 0;
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else if( cell == HBA_PXIE )
                {
                    r_channel_pxie[channel] = wdata; 
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else if( cell == HBA_PXCI )    // should only set one bit
                {
                    r_channel_pxci[channel] = r_channel_pxci[channel].read() | wdata;
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else if( cell == HBA_PXCMD )   
                {
                    if( p_vci_target.wdata.read() == 0 ) // channel soft reset
                    {
                        r_channel_pxci[channel]  = 0;
                        r_channel_pxis[channel]  = 0;
                        r_channel_slot[channel]  = 0;
                        r_channel_run[channel]   = false;
                    }
                    else                                 // channel activation
                    {
                        r_channel_run[channel]   = true;
                    }
                    r_tgt_fsm = T_WRITE_RSP;
                }
                else
                {
                    r_tgt_fsm = T_WRITE_ERROR;
                }
            }
            else           // read command
            {
                if     ( cell == HBA_PXCLB)
                {
                    r_rdata   = r_channel_pxclb[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else if( cell == HBA_PXCLBU)
                {
                    r_rdata   = r_channel_pxclbu[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else if( cell == HBA_PXIS)
                {
                    r_rdata   = r_channel_pxis[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else if( cell == HBA_PXIE)
                {
                    r_rdata   = r_channel_pxie[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else if( cell == HBA_PXCI)
                {
                    r_rdata   = r_channel_pxci[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else if( cell == HBA_PXCMD)
                {
                    r_rdata   = r_channel_run[channel];  
                    r_tgt_fsm = T_READ_RSP;
                }
                else 
                {
                    r_tgt_fsm = T_READ_ERROR;
                }
            }
        }
        break;
    }
    case T_READ_RSP:
    case T_WRITE_RSP:
    case T_READ_ERROR:
    case T_WRITE_ERROR: 
    {
        if ( p_vci_target.rspack.read() ) r_tgt_fsm = T_IDLE;
        break;
    }
    } // end switch target fsm

    ////////////////////////
    switch(r_cmd_fsm.read())
    {
        //////////////
        case CMD_IDLE:
        {
            bool not_found = true;
            for( size_t n = 0 ; (n < m_channels) and not_found ; n++ )
            {
                size_t k = (r_cmd_index.read() + n + 1) % m_channels;
            
                switch(r_channel_fsm[k])
                {
                    case CHANNEL_READ_CMD_DESC_CMD:
                    case CHANNEL_READ_LBA_CMD: 
                    case CHANNEL_READ_BUF_DESC_CMD:     
                    case CHANNEL_READ_CMD:
                        not_found    = false;
                        r_cmd_index  = k;
                        r_cmd_fsm    = CMD_READ;
                    break;
                    case  CHANNEL_WRITE_CMD:
                        not_found    = false;
                        r_cmd_index  = k;
                        r_cmd_fsm    = CMD_WRITE;
                        r_cmd_words  = 0;
                    break;
                }
            }
            break;
        }
        //////////////
        case CMD_READ:      // read command : always one flit
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            }
            break;
        }
        ///////////////
        case CMD_WRITE:     // write command : the number of flits depends on DATA width
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                size_t k = r_cmd_index.read();

                if( vci_param::B == 4 )    // one word per flit
                {
                    if( r_cmd_words.read()  == ((r_channel_length[k].read()>>2)-1) )
                    {
                        r_cmd_fsm = CMD_IDLE;
                    }
                    r_cmd_words          = r_cmd_words.read()+1;
                    r_channel_word[k]    = r_channel_word[k].read()+1;
                    r_channel_address[k] = r_channel_address[k].read()+4;
                }
                else                       // two words per flit
                {
                    if( r_cmd_words.read()  == ((r_channel_length[k].read()>>2)-2) )
                    {
                        r_cmd_fsm = CMD_IDLE;
                    }
                    r_cmd_words          = r_cmd_words.read()+2;
                    r_channel_word[k]    = r_channel_word[k].read()+2;
                    r_channel_address[k] = r_channel_address[k].read()+8;
                }
            }
            break;
        }
    }

    ////////////////////////
    switch(r_rsp_fsm.read())
    {
        //////////////
        case RSP_IDLE:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                size_t k = (size_t)p_vci_initiator.rtrdid.read();
                switch(r_channel_fsm[k].read())
                {
                case CHANNEL_READ_CMD_DESC_RSP:
                case CHANNEL_READ_BUF_DESC_RSP:
                case CHANNEL_READ_LBA_RSP:
                case CHANNEL_READ_RSP:
                    r_rsp_index  = k;
                    r_rsp_fsm    = RSP_READ;
                break;
                case CHANNEL_WRITE_RSP:
                    r_rsp_index  = k;
                    r_rsp_fsm    = RSP_WRITE;
                break;
                default:  
                std::cout << "VCI_MULTI_AHCI ERROR : unexpected VCI response" << std::endl;
                exit(0);
                }
            }
            break;
        }
        //////////////    
        case RSP_READ:     // read response : number of flits depends on VCI DATA width
        {
            if ( p_vci_initiator.rspval.read() )
            {
                size_t k    = r_rsp_index.read();
                size_t word = r_channel_word[k].read();
                if ( vci_param::B == 4 )      // VCI DATA = 32 bits
                {
                    uint32_t data = p_vci_initiator.rdata.read(); 
                    r_channel_buffer[k][word] = data;
                    r_channel_word[k]         = word+1;
                }
                else                          // VCI DATA = 64 bits
                {
                    uint64_t data = p_vci_initiator.rdata.read(); 
                    r_channel_buffer[k][word]   = (uint32_t)data;
                    r_channel_buffer[k][word+1] = (uint32_t)(data>>32);
                    r_channel_word[k]           = word+2;
                }
                if ( p_vci_initiator.reop.read() )
                {
                    r_channel_done[k] = true;
                    r_channel_error[k] = ((p_vci_initiator.rerror.read() & 0x1) != 0);
                    r_rsp_fsm = RSP_IDLE;
                } 
            }
            break;
        }
        ///////////////
        case RSP_WRITE:    // write response : must be one flit
        {
            if ( p_vci_initiator.rspval.read() )
            {
                assert( (p_vci_initiator.reop.read() == true) and
                "VCI_MULTI_AHCI ERROR : write response packed must contain one flit");  
                size_t k  = r_rsp_index.read();
                r_channel_done[k]   = true;
                r_channel_error[k]  = ((p_vci_initiator.rerror.read() & 0x1) != 0);
                r_rsp_fsm           = RSP_IDLE;
            }
            break;
        }
    }

}  // end transition

//////////////////////
tmpl(void)::genMoore()
{
    // p_vci_target port   
    p_vci_target.rsrcid = (sc_dt::sc_uint<vci_param::S>)r_tgt_srcid.read();
    p_vci_target.rtrdid = (sc_dt::sc_uint<vci_param::T>)r_tgt_trdid.read();
    p_vci_target.rpktid = (sc_dt::sc_uint<vci_param::P>)r_tgt_pktid.read();
    p_vci_target.reop   = true;

    switch(r_tgt_fsm) 
    {
        case T_IDLE:
        {
            p_vci_target.cmdack = true;
            p_vci_target.rspval = false;
            break;
        }
        case T_READ_RSP:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = (vci_data_t)r_rdata.read();
            p_vci_target.rerror = VCI_READ_OK;
            break;
        }     
        case T_READ_ERROR:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = VCI_READ_ERROR;
            break;
        }
        case T_WRITE_ERROR:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = VCI_WRITE_ERROR;
            break;
        }
        case T_WRITE_RSP:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = VCI_WRITE_OK;
            break;
        }
    }  // end switch target fsm
    
    // p_vci initiator command signals
    switch (r_cmd_fsm.read()) 
    {
        case CMD_IDLE:
        {
            p_vci_initiator.cmdval  = false;
            p_vci_initiator.address = 0;
            p_vci_initiator.wdata   = 0;
            p_vci_initiator.be      = 0;
            p_vci_initiator.plen    = 0;
            p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.trdid   = 0;
            p_vci_initiator.pktid   = 0;
            p_vci_initiator.srcid   = 0;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = false;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            p_vci_initiator.eop     = false;
            break;
        }
        case CMD_READ:
        {
            size_t k = r_cmd_index.read();
            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = (sc_dt::sc_uint<vci_param::N>)r_channel_address[k].read(); 
            p_vci_initiator.wdata   = 0;
            p_vci_initiator.be      = 0;
            p_vci_initiator.plen    = (sc_dt::sc_uint<vci_param::K>)r_channel_length[k].read();
            p_vci_initiator.cmd     = vci_param::CMD_READ;
            p_vci_initiator.trdid   = k;
            p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; // compatible with TSAR 
            p_vci_initiator.srcid   = m_srcid;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = true;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            p_vci_initiator.eop     = true;
            break;
        }
        case CMD_WRITE:
        {
            size_t k = r_cmd_index.read();
            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = (sc_dt::sc_uint<vci_param::N>)r_channel_address[k].read();
            p_vci_initiator.plen    = (sc_dt::sc_uint<vci_param::K>)r_channel_length[k].read();
            p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.trdid   = k;
            p_vci_initiator.pktid   = TYPE_WRITE; // compatible with TSAR 
            p_vci_initiator.srcid   = m_srcid;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = true;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;

            if ( (vci_param::B == 8) and (((r_channel_length[k].read()>>2) - r_cmd_words.read()) > 1) )
            {
                p_vci_initiator.wdata =  ((uint64_t)r_channel_buffer[k][r_channel_word[k].read()  ]) +
                                        (((uint64_t)r_channel_buffer[k][r_channel_word[k].read()+1]) << 32);
                p_vci_initiator.be    = 0xFF;
                p_vci_initiator.eop   = ( (r_channel_length[k].read()>>2) - r_cmd_words.read() <= 2 );
            }
            else
            {
                p_vci_initiator.wdata = (uint64_t)r_channel_buffer[k][r_channel_word[k].read()];
                p_vci_initiator.be    = 0xF;
                p_vci_initiator.eop   = ( (r_channel_length[k].read()>>2) - r_cmd_words.read() <= 1 );
            }
            break;
        }
    }
    
    // p_vci initiator response signals
    if ( r_rsp_fsm.read() == RSP_IDLE )  p_vci_initiator.rspack = false;
    else                                 p_vci_initiator.rspack = true;
    
    // p_channel_irq[k]
    for ( size_t k=0 ; k<m_channels ; k++ )
    {
        p_channel_irq[k] = (((r_channel_pxis[k].read() & 0x1)!=0) and 
                            ((r_channel_pxie[k].read() & 0x1)!=0)) or         //ok 
                           (((r_channel_pxis[k].read() & 0x40000000)!=0) and 
                            ((r_channel_pxie[k].read() & 0x40000000)!=0));    //error
    }
    
} // end GenMoore()

//////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiAhci( sc_core::sc_module_name             name, 
                         const soclib::common::MappingTable   &mt,
                         const soclib::common::IntTab         &srcid,
                         const soclib::common::IntTab         &tgtid,
                         std::vector<std::string>             &filenames,
                         const uint32_t                       block_size,
                         const uint32_t                       burst_size,
                         const uint32_t                       latency)
           
           : caba::BaseModule(name),
         
           m_srcid(mt.indexForId(srcid)),
           m_words_per_block(block_size>>2),
           m_words_per_burst(burst_size>>2),
           m_bursts_per_block(block_size/burst_size),
           m_latency(latency),
           m_channels(filenames.size()),
           m_seglist(mt.getSegmentList(tgtid)),
           p_clk("p_clk"),
           p_resetn("p_resetn"),
           p_vci_initiator("p_vci_initiator"),
           p_vci_target("p_vci_target")
{
    std::cout << "  - Building VciMultiAhci " << name << std::endl;
    
	SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
    
	SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
    
    
    size_t nbsegs = 0;
    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
    {
        nbsegs++;
        if ( (seg->baseAddress() & 0x00007FFF) != 0 ) 
        {
		    std::cout << "Error in component VciMultiAhci : " << name 
		              << " The base address of segment " << seg->name()
                      << " must be multiple of 32 Kbytes" << std::endl;
		    exit(1);
	    }
	    if ( seg->size() < (m_channels*0x1000) ) 
        {
		    std::cout << "Error in component VciMultiAhci : " << name 
	                  << " The size of segment " << seg->name()
                      << " is smaller than (channels * 4096)" << std::endl;
		    exit(1);
	    }
        
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }
    
    if( nbsegs == 0 )
    {
		std::cout << "Error in component VciMultiAhci : " << name
		          << " No segment allocated" << std::endl;
		exit(1);
    }
    
    if( (block_size != 128)  && 
        (block_size != 256)  && 
        (block_size != 512)  && 
        (block_size != 1024) && 
        (block_size != 2048) && 
        (block_size != 4096) )
    {
        std::cout << "Error in component VciMultiAhci : " << name
                  << " The block size must be 128, 256, 512, 1024, 2048 or 4096 bytes"
                  << std::endl;
        exit(1);
    }
    
    if( (burst_size != 8 ) && 
        (burst_size != 16) && 
        (burst_size != 32) && 
        (burst_size != 64) )
    {
        std::cout << "Error in component VciMultiAhci : " << name 
                  << " The burst size must be 8, 16, 32 or 64 bytes" << std::endl;
        exit(1);
    }
    
	if ( (vci_param::B != 4) and (vci_param::B != 8) )
    {
		std::cout << "Error in component VciMultiAhci : " << name              
		          << " The VCI data field must have 32 or 64 bits" << std::endl;
		exit(1);
	}

    if( m_channels > 8 )
    {
        std::cout << "Error in component VciMultiAhci: " << name
                  << " the number of channels should not be larger than 8" << std::endl;
    }

    p_channel_irq      = alloc_elems<sc_out<bool> >("p_channel_irq" , m_channels);
    
    m_fd               = new uint32_t[m_channels];
    m_device_size      = new uint64_t[m_channels];

    r_channel_fsm      = alloc_elems<sc_signal<int>      >("r_channel_fsm"     , m_channels);
    r_channel_pxclb    = alloc_elems<sc_signal<uint32_t> >("r_channel_pxclb"   , m_channels);    
    r_channel_pxclbu   = alloc_elems<sc_signal<uint32_t> >("r_channel_pxclbu"  , m_channels); 
    r_channel_pxis     = alloc_elems<sc_signal<uint32_t> >("r_channel_pxis"    , m_channels);
    r_channel_pxie     = alloc_elems<sc_signal<uint32_t> >("r_channel_pxie"    , m_channels);
    r_channel_pxci     = alloc_elems<sc_signal<uint32_t> >("r_channel_pxci"    , m_channels); 
    r_channel_run      = alloc_elems<sc_signal<bool>     >("r_channel_run"     , m_channels);
    r_channel_slot     = alloc_elems<sc_signal<uint32_t> >("r_channel_slot"    , m_channels);
    r_channel_offset   = alloc_elems<sc_signal<uint32_t> >("r_channel_offset"  , m_channels);
    r_channel_address  = alloc_elems<sc_signal<uint64_t> >("r_channel_address" , m_channels);
    r_channel_length   = alloc_elems<sc_signal<uint32_t> >("r_channel_length"  , m_channels);
    r_channel_word     = alloc_elems<sc_signal<uint32_t> >("r_channel_word"    , m_channels);
    r_channel_ctba     = alloc_elems<sc_signal<uint64_t> >("r_channel_ctba"    , m_channels);
    r_channel_dba      = alloc_elems<sc_signal<uint64_t> >("r_channel_dba"     , m_channels);
    r_channel_dbc      = alloc_elems<sc_signal<uint32_t> >("r_channel_dbc"     , m_channels);
    r_channel_prdtl    = alloc_elems<sc_signal<uint32_t> >("r_channel_prdtl"   , m_channels);
    r_channel_write    = alloc_elems<sc_signal<bool>     >("r_channel_write"   , m_channels);
    r_channel_count    = alloc_elems<sc_signal<uint32_t> >("r_channel_count"   , m_channels);
    r_channel_lba      = alloc_elems<sc_signal<uint64_t> >("r_channel_lba"     , m_channels);
    r_channel_bufid    = alloc_elems<sc_signal<uint32_t> >("r_channel_bufid"   , m_channels);
    r_channel_latency  = alloc_elems<sc_signal<uint32_t> >("r_channel_latency" , m_channels);
    r_channel_prdbc    = alloc_elems<sc_signal<uint32_t> >("r_channel_prdbc"   , m_channels);
    r_channel_blocks   = alloc_elems<sc_signal<uint32_t> >("r_channel_blocks"  , m_channels);
    r_channel_bursts   = alloc_elems<sc_signal<uint32_t> >("r_channel_bursts"  , m_channels);
    r_channel_done     = alloc_elems<sc_signal<bool>     >("r_channel_done"    , m_channels);
    r_channel_error    = alloc_elems<sc_signal<bool>     >("r_channel_error"   , m_channels);

    r_channel_buffer   = new uint32_t*[m_channels];
    for( size_t k=0 ; k<m_channels ; k++ ) r_channel_buffer[k]=new uint32_t[block_size];

    // open files containing virtual disks images
    std::vector<std::string>::iterator it;
    size_t index=0;
    for( it=filenames.begin() ; it!=filenames.end() ; it++ , index++ )
    {
        m_fd[index] = ::open(it->c_str(), O_RDWR );
        if ( m_fd[index] < 0 )
        {
            std::cout << "Error in component VciMultiAhci : " << name 
                      << " Unable to open file " << *it << std::endl;
            exit(1);
        }

        m_device_size[index] = lseek(m_fd[index], 0, SEEK_END) / block_size;
    
        if ( m_device_size[index] > ((uint64_t)1<<vci_param::N ) ) 
        {
            std::cout << "Error in component VciMultiAhci" << name 
                      << " The file " << *it
                      << " has more blocks than addressable with the VCI address" << std::endl;
            exit(1);
        }
        std::cout << "    => disk image " << *it << " successfully open" << std::endl;
    }
} // end constructor

///////////////////////////
tmpl(/**/)::~VciMultiAhci()
{
    delete[] m_fd;
    delete[] m_device_size;

    for(size_t k=0;k<m_channels;k++) delete[] r_channel_buffer[k];
    delete[] r_channel_buffer;

    soclib::common::dealloc_elems<sc_signal<int>      >(r_channel_fsm    , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_pxclb  , m_channels);    
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_pxclbu , m_channels);
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_pxis   , m_channels);
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_pxie   , m_channels);
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_pxci   , m_channels); 
    soclib::common::dealloc_elems<sc_signal<bool>     >(r_channel_run    , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_slot   , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_offset , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_address, m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_length , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_word   , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_ctba   , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_dba    , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_dbc    , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_prdtl  , m_channels); 
    soclib::common::dealloc_elems<sc_signal<bool>     >(r_channel_write  , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_count  , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_lba    , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_bufid  , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_prdbc  , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_blocks , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_bursts , m_channels); 
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_latency, m_channels);  
    soclib::common::dealloc_elems<sc_signal<bool>     >(r_channel_done   , m_channels); 
    soclib::common::dealloc_elems<sc_signal<bool>     >(r_channel_error  , m_channels); 

    soclib::common::dealloc_elems<sc_out<bool> >(p_channel_irq, m_channels);
}

/////////////////////////
tmpl(void)::print_trace()
{
    const char* channel_state_str[] = 
    {
	    "CHANNEL_IDLE",
        "CHANNEL_READ_CMD_DESC_CMD",
        "CHANNEL_READ_CMD_DESC_RSP",
        "CHANNEL_READ_LBA_CMD",
        "CHANNEL_READ_LBA_RSP",
        "CHANNEL_READ_BUF_DESC_START",
        "CHANNEL_READ_BUF_DESC_CMD",
        "CHANNEL_READ_BUF_DESC_RSP",
        "CHANNEL_READ_START",
        "CHANNEL_READ_CMD",
        "CHANNEL_READ_RSP",
        "CHANNEL_WRITE_BLOCK",
        "CHANNEL_WRITE_START",
        "CHANNEL_WRITE_CMD",
        "CHANNEL_WRITE_RSP",
        "CHANNEL_READ_BLOCK",
        "CHANNEL_TEST_LENGTH",
        "CHANNEL_SUCCESS",
        "CHANNEL_ERROR",
        "CHANNEL_BLOCKED",
	};
	const char* tgt_state_str[] = 
    {
         "TGT_IDLE",
         "TGT_WRITE_RSP",
         "TGT_READ_RSP",
         "TGT_WRITE_ERROR",
         "TGT_READ_ERROR",
	};
    const char* cmd_state_str[] =
    {
        "CMD_IDLE",
        "CMD_READ",
        "CMD_WRITE",
    };
    const char* rsp_state_str[] = 
    {
        "RSP_IDLE",
        "RSP_READ",
        "RSP_WRITE"
    };

    std::cout << "MULTI_AHCI " << name() << " : " 
              << tgt_state_str[r_tgt_fsm.read()] 
              << " / " << cmd_state_str[r_cmd_fsm.read()]    
              << " / " << rsp_state_str[r_rsp_fsm.read()] << std::endl;

    for ( size_t k = 0 ; k < m_channels ; k++ )
    {
        std::cout << "  CHANNEL " << std::dec << k << " : " 
                  << channel_state_str[r_channel_fsm[k].read()] 
                  << " / pxci = " << std::hex << r_channel_pxci[k].read()
                  << " / pxis = " << std::hex << r_channel_pxis[k].read()
                  << " / pxie = " << std::hex << r_channel_pxie[k].read()
                  << " / cmd_id = " << std::dec << r_channel_slot[k].read()
                  << std::endl
                  << "              lba = " << std::hex << r_channel_lba[k].read()
                  << " / buf_paddr = " << std::hex << r_channel_dba[k].read()
                  << " / nbytes = " << std::dec << r_channel_dbc[k].read()
                  << " / blocks = " << std::dec << r_channel_blocks[k].read()
                  << " / bursts = " << std::dec << r_channel_bursts[k].read()
                  << std::endl;
    }
}


}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

