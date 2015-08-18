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
 
#ifndef SOCLIB_CABA_STRUCT_VCI_H
#define SOCLIB_CABA_STRUCT_VCI_H

#include <systemc>
#include <sstream>
#include <stdio.h>
#include <inttypes.h>
#include "static_assert.h"
#include "static_fast_int.h"

namespace soclib {
namespace caba {

    using namespace sc_core;
/********************************
** VCI Command & Response fields
********************************/
template <typename vci_param>
struct VciCmdFlit{
	typename vci_param::addr_t    address;
	typename vci_param::be_t      be;
	typename vci_param::cmd_t     cmd;
	typename vci_param::contig_t  contig;
	typename vci_param::data_t    wdata;
	typename vci_param::eop_t     eop;
	typename vci_param::const_t   cons;
	typename vci_param::plen_t    plen;
	typename vci_param::wrap_t    wrap;
	typename vci_param::cfixed_t  cfixed;
	typename vci_param::clen_t    clen;
	typename vci_param::srcid_t   srcid;
	typename vci_param::trdid_t   trdid;
	typename vci_param::pktid_t   pktid;  
	void print(bool visible){
		if (visible){
			int nbchar=0;
			int sumnbchar=0;
			//printf( "con(%d)\t", (int)contig);
			//printf ("wrap(%d)\t", (int)wrap);
			//printf ("be(%X)\t", (int)be);
			printf ("%.2d%d%d.", (int)srcid,(int)trdid,(int)pktid);
			switch (cmd){
			case vci_param::CMD_READ:
				printf ("RE(");
				break;
			case vci_param::CMD_WRITE:
				printf ("WR(");
				break;
			case vci_param::CMD_LOCKED_READ:
				printf ("LL(");
				break;
			case vci_param::CMD_STORE_COND:
				printf ("SC(");
				break;
			}
			sumnbchar=8;
			printf ("%llX,%X%n",(int64_t)address,(int)plen,&nbchar);
			sumnbchar+=nbchar;
			if (cmd==vci_param::CMD_WRITE || cmd==vci_param::CMD_STORE_COND){
				printf (",%X%n",(int)wdata,&nbchar);
				sumnbchar+=nbchar;
			}
			printf ("|%X",(int)be);
			if ((int)eop==1)
				printf(")");
			else
				printf(";");
			sumnbchar+=3;
			if (sumnbchar<16)
				printf("        ");
			printf("\t");
		}else{
			printf("                \t");
		}
	}
	void fprint(FILE* out_file){
		fprintf (out_file,"%.2d%d%d.", (int)srcid,(int)trdid,(int)pktid);
		switch (cmd){
		case vci_param::CMD_READ:
			fprintf (out_file, "RE(");
			break;
		case vci_param::CMD_WRITE:
			fprintf (out_file, "WR(");
			break;
		case vci_param::CMD_LOCKED_READ:
			fprintf (out_file, "LL(");
			break;
		case vci_param::CMD_STORE_COND:
			fprintf (out_file, "SC(");
			break;
		}
		fprintf (out_file, "%llX,%X",(int64_t)address,(int)plen);
		if (cmd==vci_param::CMD_WRITE || cmd==vci_param::CMD_STORE_COND)
			fprintf (out_file, ",%X",(int)wdata);
		fprintf (out_file, "|%X",(int)be);
		if ((int)eop==1)
			fprintf (out_file, ")\r\n");
		else
			fprintf (out_file, ";\r\n");
	}
};

template <typename vci_param>
struct VciRspFlit{
	typename vci_param::data_t   rdata;
	bool                         reop;
	typename vci_param::rerror_t rerror;
	typename vci_param::srcid_t  rsrcid;
	typename vci_param::trdid_t  rtrdid;
	typename vci_param::pktid_t  rpktid;
	void print(bool visible){
		if (visible){
			int sumnbchar=0;
			printf ("%.2d%d%d.Rsp(%X,%X%n", (int)rsrcid,(int)rtrdid,(int)rpktid,(int)rerror,(int)rdata,&sumnbchar);
			if ((int)reop==1)
				printf(")");
			else
				printf(";");
			sumnbchar+=1;
			if (sumnbchar<16)
				printf("        ");
			printf("\t");
		}else{
			printf("                \t");
		}
	}
	void fprint(FILE* out_file){
		fprintf (out_file,"%.2d%d%d.Rsp(%X,%X", (int)rsrcid,(int)rtrdid,(int)rpktid,(int)rerror,(int)rdata);
		if ((int)reop==1)
			fprintf(out_file,")\r\n");
		else
			fprintf(out_file,";\r\n");
	}
};

}}
#endif /* SOCLIB_CABA_STRUCT_VCI_H */
