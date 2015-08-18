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
 
#ifndef SOCLIB_CABA_STRUCT_HHT_H
#define SOCLIB_CABA_STRUCT_HHT_H

#include <systemc>
#include <sstream>
#include <inttypes.h>
#include "static_assert.h"
#include "static_fast_int.h"
#include "hht_param.h"
namespace soclib {
namespace caba {

    using namespace sc_core;
/********************************
** HHT Command & Response headers
********************************/
template <typename hht_param>
struct HhtCmdFlit{
	typename hht_param::seqid_t	seqid;
	typename hht_param::unitid_t unitid;
	typename hht_param::srctag_t srctag;
	typename hht_param::passpw_t passpw;
	typename hht_param::cmd_t	cmd;
	typename hht_param::addr_t	addr;
	typename hht_param::mskcnt_t	mskcnt;
	typename hht_param::compat_t	compat;
	
	void print(bool visible){
		if (visible){
			//printf( "seq(%d)\t", (int)seqid);
			//printf ("ppw(%d)\t", (int)passpw);
			//printf ("cmp(%d)\t", (int)compat);
			int nbchar=0;
			int sumnbchar=0;
			printf ("%.2d%.2d.", (int)unitid,(int)srctag);
			if ((cmd & hht_param::CMD_READ_MASK)==hht_param::CMD_READ){
				if (cmd & hht_param::CMD_DWORD_FLAG)
					printf ("RD");
				else
					printf ("RB");
			}else if ((cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE){
				if (cmd & hht_param::CMD_POSTED_FLAG)
					printf ("P");
				else
					printf ("N");
				if (cmd & hht_param::CMD_DWORD_FLAG)
					printf ("D");
				else
					printf ("B");
			}else if (cmd==hht_param::CMD_READ_MODIFY_WRITE){
				printf("AT");
			}else{
				printf("%.2X",(int)cmd);
			}
			sumnbchar=8;
			printf("(%llX,%X)%n",(int64_t)addr,(int)mskcnt,&nbchar);
			sumnbchar+=nbchar;
			if (sumnbchar<16)
				printf("        ");
			printf("\t");
		}else{
			printf("                \t");
		}
	}	
	
	typename hht_param::ctrl_t get_ctrl(){
		typename hht_param::ctrl_t res;
		res=0;
		res+=unitid << 	hht_param::unitid_shift;
		res+=srctag << 	hht_param::srctag_shift;
		res+=passpw << 	hht_param::passpw_shift;
		res+=cmd << 	hht_param::cmd_shift;
		res+=compat << 	hht_param::compat_shift;
		res+=(seqid & hht_param::seqid0_mask) << hht_param::seqid0_shift;
		res+=((seqid/(hht_param::seqid0_mask+1))) << hht_param::seqid1_shift;
		res+=(typename hht_param::ctrl_t)((addr/4) & hht_param::addr0_mask) << hht_param::addr0_shift;
		res+=(typename hht_param::ctrl_t)((addr/4)/(hht_param::addr0_mask+1)) << hht_param::addr1_shift;
		res+=(mskcnt & hht_param::mskcnt0_mask) << hht_param::mskcnt0_shift;
		res+=((mskcnt/(hht_param::mskcnt0_mask+1))) << hht_param::mskcnt1_shift;
		return res;
	}
	
	void set_ctrl(typename hht_param::ctrl_t ctrl){
		unitid=	((ctrl >> hht_param::unitid_shift) & hht_param::unitid_mask);
		srctag=	((ctrl >> hht_param::srctag_shift) & hht_param::srctag_mask);
		passpw=	((ctrl >> hht_param::passpw_shift) & hht_param::passpw_mask);
		cmd=	((ctrl >> hht_param::cmd_shift) & 	hht_param::cmd_mask);
		compat=	((ctrl >> hht_param::compat_shift) & hht_param::compat_mask);
		seqid=	((ctrl >> hht_param::seqid0_shift) & hht_param::seqid0_mask) +
				((ctrl >> hht_param::seqid1_shift) & hht_param::seqid1_mask)*(hht_param::seqid0_mask+1);

		addr=	(((ctrl >> hht_param::addr0_shift) & hht_param::addr0_mask) +
				((ctrl >> hht_param::addr1_shift) & hht_param::addr1_mask)*(long long)(hht_param::addr0_mask+1))*4;
		mskcnt=	((ctrl >> hht_param::mskcnt0_shift) & hht_param::mskcnt0_mask) +
				((ctrl >> hht_param::mskcnt1_shift) & hht_param::mskcnt1_mask)*(hht_param::mskcnt0_mask+1);
	}
};
template <typename hht_param>
struct HhtRspFlit{
	typename hht_param::isoc_t	isoc;
	typename hht_param::unitid_t unitid;
	typename hht_param::srctag_t srctag;
	typename hht_param::passpw_t passpw;
	typename hht_param::cmd_t	cmd;
	typename hht_param::error_t 	error;
	typename hht_param::count_t	count;
	typename hht_param::bridge_t bridge;
	typename hht_param::rvcset_t rvcset;
	void print(bool visible){
		if (visible){
			//printf ("isc(%d)\t", (int)isoc);
			//printf ("ppw(%d)\t", (int)passpw);
			//printf ("brg(%d)\t", (int)bridge);
			//printf ("rvc(%d)", (int)rvcset);
			int nbchar=0;
			int sumnbchar=0;
			printf ("%.2d%.2d.", (int)unitid,(int)srctag);
			switch (cmd){
			case hht_param::CMD_READRSP:
				printf ("RSP(");
				break;
			case hht_param::CMD_TARGETDONE:
				printf ("TGD(");
				break;
			default:
				printf("U%.2X(",(int)cmd);
				break;
			}
			sumnbchar=9;
			printf ("%X,%X)%n", (int)error,(int)count,&nbchar);
			if (sumnbchar<16)
				printf("        ");
			printf("\t");
		}else{
			printf("                \t");
		}
	}
	typename hht_param::ctrl_t get_ctrl(){
		typename hht_param::ctrl_t res;
		res=0;
		res+=isoc << 	hht_param::isoc_shift;
		res+=unitid << 	hht_param::unitid_shift;
		res+=srctag << 	hht_param::srctag_shift;
		res+=passpw << 	hht_param::passpw_shift;
		res+=cmd << 	hht_param::cmd_shift;
		res+=bridge << 	hht_param::bridge_shift;
		res+=rvcset << 	hht_param::rvcset_shift;
		res+=(error & hht_param::error0_mask) << hht_param::error0_shift;
		res+=((error/(hht_param::error0_mask+1))) << hht_param::error1_shift;
		res+=(count & hht_param::mskcnt0_mask) << hht_param::mskcnt0_shift;
		res+=((count/(hht_param::mskcnt0_mask+1))) << hht_param::mskcnt1_shift;
		return res;
	}
	
	void set_ctrl(typename hht_param::ctrl_t ctrl){
		isoc=	((ctrl >> hht_param::isoc_shift) & 	hht_param::isoc_mask);
		unitid=	((ctrl >> hht_param::unitid_shift) & hht_param::unitid_mask);
		srctag=	((ctrl >> hht_param::srctag_shift) & hht_param::srctag_mask);
		passpw=	((ctrl >> hht_param::passpw_shift) & hht_param::passpw_mask);
		cmd=	((ctrl >> hht_param::cmd_shift) & 	hht_param::cmd_mask);
		bridge=	((ctrl >> hht_param::bridge_shift) & hht_param::bridge_mask);
		rvcset=	((ctrl >> hht_param::rvcset_shift) & hht_param::rvcset_mask);
		error=	((ctrl >> hht_param::error0_shift) & hht_param::error0_mask) +
				((ctrl >> hht_param::error1_shift) & hht_param::error1_mask)*(hht_param::error0_mask+1);
		count=	((ctrl >> hht_param::mskcnt0_shift) & hht_param::mskcnt0_mask) +
				((ctrl >> hht_param::mskcnt1_shift) & hht_param::mskcnt1_mask)*(hht_param::mskcnt0_mask+1);
	}

};

}}
#endif /* SOCLIB_CABA_STRUCT_HHT_H */
