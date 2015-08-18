/********************************************************************************/
/*	File : mips32_registers.h						*/
/*	Author : Alain Greiner							*/
/*	Date : 26/03/2012							*/
/********************************************************************************/
/* 	We define mnemonics for MIPS32 registers             	 		*/
/********************************************************************************/
		
#ifndef _MIPS32_REGISTER_H
#define _MIPS32_REGISTER_H

/* processor registers */

#define zero			$0
#define at			$1
#define v0			$2
#define v1			$3
#define a0			$4
#define a1			$5
#define a2			$6
#define a3			$7
#define t0			$8
#define t1			$9
#define t2			$10
#define t3			$11
#define t4			$12
#define t5			$13
#define t6			$14
#define t7			$15
#define s0			$16
#define s1			$17
#define s2			$18
#define s3			$19
#define s4			$20
#define s5			$21
#define s6			$22
#define s7			$23
#define t8			$24
#define t9			$25
#define k0			$26
#define k1			$27
#define gp			$28
#define sp			$29
#define fp			$30
#define ra			$31

/* CP0 registers */

#define CP0_BVAR		$8
#define CP0_TIME 		$9
#define	CP0_SR 			$12
#define CP0_CR 			$13
#define CP0_EPC			$14
#define CP0_PROCID 		$15,1
#define CP0_SCHED 		$22

/* CP2 registers */

#define CP2_PTPR            	$0 
#define CP2_MODE            	$1
#define CP2_ICACHE_FLUSH    	$2
#define CP2_DCACHE_FLUSH    	$3
#define CP2_ITLB_INVAL      	$4 
#define CP2_DTLB_INVAL      	$5 
#define CP2_ICACHE_INVAL    	$6 
#define CP2_DCACHE_INVAL    	$7
#define CP2_ICACHE_PREFETCH 	$8 
#define CP2_DCACHE_PREFETCH 	$9 
#define CP2_SYNC            	$10 
#define CP2_IETR            	$11 
#define CP2_DETR            	$12 
#define CP2_IBVAR           	$13
#define CP2_DBVAR           	$14
#define CP2_PARAMS          	$15 
#define CP2_RELEASE         	$16 
#define CP2_DATA_LO				$17 	
#define CP2_DATA_HI				$18 		
#define CP2_ICACHE_INVAL_PA 	$19 		
#define CP2_DCACHE_INVAL_PA 	$20 

#endif
