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
	printf("HHT commands file random generator\n");
	if (argc<1){
		printf("Syntax : hht_cmd_rand_gen [f_file [nbcmd]]\n");
	}
	
	char* filename=new char[256];
	FILE* f_file;
	printf("Enter a name for the output HHT commands file : ");
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
	fprintf(f_file,"' Randomly generated HHT commands file\n");
	fprintf(f_file,"' Random's seed : %u\n",rndinit);
	srand(rndinit);

	int			 unitid,start_unitid=0,	size_unitid=16;
	int			 srctag,start_srctag=0,	size_srctag=10;
#ifndef WIN32
	int64		 addr,	start_addr=0,	size_addr= 0xFFFFFFFFFFLL;
#else
	int64		 addr,	start_addr=0,	size_addr= 0xFFFFFFFFFFL;
#endif
	int			 cmd,	start_cmd=0,	size_cmd=2;
	int			 count;
	int			 be;
	unsigned int wdata,	start_wdata=0,	size_wdata=0xFFFFFFFF;
	int			 posted;
	int			 dword;
	
	int i,j;
	for (i=nbcmd;i>0;i--){
		unitid=	rand()%size_unitid + start_unitid;
		srctag=	rand()%size_srctag + start_srctag;
		fprintf(f_file,"%.2d %.2d ",unitid,srctag);
		
		dword=rand()%2;
		posted=rand()%2;
		cmd=	rand()%size_cmd +	 start_cmd;
		if (cmd==0){
			if (dword==0){	// Masked Read
				be=rand()%16;
				count=0;
			}else{			// Non-masked read
				count=rand()%16;
			}
		}else if (cmd==1){
			if (dword==0){	// Masked write
				count=rand()%8;
			}else{			// Non-masked write
				count=rand()%16;
			}
		}else if (cmd==2){ // Atomic operation
			count=(rand()%2)*2+1;
		}

		do{
			addr=	((rand()+((int64)rand()*RAND_MAX+rand())*RAND_MAX)%size_addr + start_addr);
			addr=(addr/4)*4;
		// Avoid crossing borders of 8 or 16 words
		}while ((((addr/4)%8+count>7) && (cmd==1 && dword==0)) || ((addr/4)%16+count>15));
		
		if (cmd==0){
			if (dword==0){	// Masked read
				fprintf(f_file,"RB(%.6X%.4X,%X)",(int)(addr/0x10000),(int)(addr%0x10000),be);
			}else{			// Non-masqued read
				fprintf(f_file,"RD(%.6X%.4X,%X)",(int)(addr/0x10000),(int)(addr%0x10000),count);
			}
		}else if (cmd==1){
			fprintf(f_file,(posted==1)?"P":"N"); // Posted/Non posted
			fprintf(f_file,(dword==1)?"D":"B");  // Double/Byte
			if (dword!=1)
				count++;
			wdata=(rand()+rand()*RAND_MAX)%size_wdata + start_wdata;
			fprintf(f_file,"(%.6X%.4X,%X,%X",(int)(addr/0x10000),(int)(addr%0x10000),count,wdata);
			for (j=count;j>0;j--){
				wdata=(rand()+rand()*RAND_MAX)%size_wdata + start_wdata;
				fprintf(f_file,",\n                      %X",wdata);
			}
			fprintf(f_file,")");
		}
		fprintf(f_file,"\n");
	}
	printf("Generation terminee avec succes\n");
	fclose(f_file);
	return 0;
}
