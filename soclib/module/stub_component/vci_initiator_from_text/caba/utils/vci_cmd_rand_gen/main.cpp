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
 
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
// Support for the long long type. This type is not in the standard
// but is usually supported by compilers.
#ifndef WIN32
    typedef long long          int64;
#else
    typedef __int64            int64;
#endif
int main(int argc, char* argv[]){
	printf("VCI commands file random generator\n");
	if (argc<1){
		printf("Syntax : vci_cmd_rand_gen [f_file [nbcmd]]\n");
	}
	
	char* filename=new char[256];
	FILE* f_file;
	printf("Enter a name for the output VCI commands file :");
	if (argc>1){
		filename=argv[1];
		printf("%s\n",filename);
	}else{
		std::cin >> filename;
	}
	f_file=fopen(filename,"w");
	if (f_file==0){
		printf("Could not create output file\n");
		return 1;
	}
	
	int nbcmd;
	printf("Number of commands to generate : ");
	if (argc>2){
		nbcmd=atoi(argv[2]); 
		printf("%d\n",nbcmd);
	}else{
		std::cin >> nbcmd;
	}
	if (nbcmd<1){
		printf("Incorrect number of commands to generate\n");
		fclose(f_file);
		return 2;
	}
	unsigned int rndinit=(unsigned)time(NULL);
	fprintf(f_file,"' Randomly generated VCI commands file\n");
	fprintf(f_file,"' Random's seed : %u\n",rndinit);
	srand(rndinit);

	int			 srcid,	start_srcid=0,	size_srcid=16;
	int			 trdid,	start_trdid=0,	size_trdid=2;
	int			 pktid,	start_pktid=0,	size_pktid=4;
#ifndef WIN32
	int64		 addr,	start_addr=0,	size_addr= 0xFFFFFFFFFFLL;
#else
	int64		 addr,	start_addr=0,	size_addr= 0xFFFFFFFFFFL;
#endif
	int			 cmd,	start_cmd=1,	size_cmd=2;
	int			 plen,	start_plen= 1,	size_plen=16;
	unsigned int wdata,	start_wdata=0,	size_wdata=0xFFFFFFFF;
	int			 be,	start_be=-10,	size_be=200;

	int i,j;
	for (i=nbcmd;i>0;i--){
		srcid=	rand()%size_srcid + start_srcid;
		trdid=	rand()%size_trdid + start_trdid;
		pktid=	rand()%size_pktid + start_pktid;
		cmd=	rand()%size_cmd +	start_cmd;
		plen=	rand()%size_plen +	start_plen;
		if (cmd==2)
			plen=(plen-1) % 8+1;
		do{
			addr=	((rand()+((int64)rand()*RAND_MAX+rand())*RAND_MAX)%size_addr + start_addr);
			addr=(addr/4)*4;
		// Avoid crossing borders of 8 or 16 words
		}while ((cmd==2 && ((addr/4)%8+plen>8)) || (cmd==1 && ((addr/4)%16+plen>16)));
		wdata=(rand()+rand()*RAND_MAX)%size_wdata + start_wdata;
		be=	rand()%size_be + start_be;
		if (be>15) be=15;
		if (be<0) be=0;	
		
		fprintf(f_file,"%.2d %d %d ",srcid,trdid,pktid);
		switch (cmd){
		case 1:
			fprintf(f_file,"RE(%.6X%.4X,%.2X|%X)",(int)(addr/0x10000),(int)(addr%0x10000),plen*4,be);
			break;
		case 2:
			fprintf(f_file,"WR(%.6X%.4X,%.2X,%.8X|%X",(int)(addr/0x10000),(int)(addr%0x10000),plen*4,wdata,be);
			for (j=plen-1;j>0;j--){
				wdata=(rand()+rand()*RAND_MAX)%size_wdata + start_wdata;
				be=	rand()%size_be + start_be;
				if (be>15) be=15;
				if (be<0) be=0;
				fprintf(f_file,",\n                        %.8X|%X",wdata,be);
			}
			fprintf(f_file,")");
			break;
		}
		fprintf(f_file,"\n");
	}
	printf("Generation terminee avec succes\n");
	fclose(f_file);
	return 0;
}
