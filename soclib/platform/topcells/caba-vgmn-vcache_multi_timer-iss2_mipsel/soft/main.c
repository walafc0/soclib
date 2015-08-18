/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU GPLv2.
 * 
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo yang
 */

#include "soclib/timer.h"
#include "system.h"
#include "stdio.h"

#include "../segmentation.h"

static const int period[4] = {10000, 11000, 12000, 13000};

static int max_interrupts = 80;

void irq_handler(int irq)
{
	uint32_t ti;
	int left = atomic_add(&max_interrupts, -1);
	
	ti = soclib_io_get(
		base(V_TIMER),
		procnum()*TIMER_SPAN+TIMER_VALUE);
	printf("IRQ %d received at cycle %d on cpu %d %d interrupts to go\n\n", irq, ti, procnum(), left);
	soclib_io_set(
		base(V_TIMER),
		procnum()*TIMER_SPAN+TIMER_RESETIRQ,
		0);
	
	if ( ! left )
		exit(0);
}

int main(void)
{
/*
	// define the page table entry
        uint32_t * ptd_table;
        ptd_table = (uint32_t *)PTD_ADDR;

        ptd_table[0] = 0x00000000; 	// unused
        //ptd_table[1] = 0x40040203; 	// PTD for instruction 	 
        ptd_table[1] = 0x410080C0; 	// PTD for instruction 	 
        //ptd_table[2] = 0x40040202; 	// PTD for tty
        ptd_table[2] = 0x41008080; 	// PTD for tty
        //ptd_table[3] = 0x40040201; 	// PTD for timer
        ptd_table[3] = 0x41008040; 	// PTD for timer
        ptd_table[64] = 0x80001014; 	// PTE for data ram
        ptd_table[128] = 0x80002014; 	// PTE for loc0
        ptd_table[132] = 0x80002114; 	// PTE for loc1
        ptd_table[136] = 0x80002214; 	// PTE for loc2
        ptd_table[140] = 0x80002314; 	// PTE for loc3
        ptd_table[512] = 0x8000802E; 	// PTE for exception
        //ptd_table[712] = 0x40040202; 	// PTD for lock
        ptd_table[712] = 0x41008080; 	// PTD for lock

	// timer pte
	uint32_t * tpte_table;
        tpte_table = (uint32_t *)TPTE_ADDR;
	tpte_table[0] = 0x82C08014;
	
	// tty pte
        uint32_t * pte_table;
        pte_table = (uint32_t *)PTE_ADDR;
        pte_table[0] = 0x83008014;	//tty no global
	pte_table[512] = 0x82C88014; 	//lock 

	// instruction pte
        uint32_t * ipte_table;
        ipte_table = (uint32_t *)IPTE_ADDR;
	ipte_table[0] = 0x8001002C;
*/

	// define the page table entry
        uint32_t * ptd_table;
        ptd_table = (uint32_t *)PTD_ADDR;

	ptd_table[2] = 0x8B000002;	// PTE for instruction
	ptd_table[4] = 0xC0040402;	// PTD for tty

        ptd_table[6] = 0xC0040403; 	// PTD for timer
        ptd_table[128] = 0x8D000080; 	// PTE for data ram
        ptd_table[1024] = 0x8B800400; 	// PTE for exception
        ptd_table[1425] = 0x85000591; 	// PTE for lock

	// timer pte
	uint32_t * tpte_table;
        tpte_table = (uint32_t *)TPTE_ADDR;
	tpte_table[0] = 0x85000000;	//timer no global
        tpte_table[1] = 0x000B0200;	//timer no global
	
	// tty pte
        uint32_t * pte_table;
        pte_table = (uint32_t *)PTE_ADDR;
        pte_table[0] = 0x85000000;	//tty no global
        pte_table[1] = 0x000C0200;	//tty no global

	puts("Page table are defined! \n");

	// context switch and tlb mode change
	//set_cp2(0, 0, 0x04040000);	// context switch
	set_cp2(0, 0, 0x00020200);	// context switch 
	set_cp2(1, 0, 0xf);		// TLB enable
	puts("Context switch(TLB flush) and TLB enable!\n");

	set_cp2(2, 0, 0);
	puts("icache flush done!\n");

	set_cp2(3, 0, 0);
	puts("dcache flush done!\n");

	// cache inval
	//set_cp2(6, 0, 0x004000a4);
	set_cp2(6, 0, 0x004000c4);
	puts("icache invalidation test good :-)\n");
	set_cp2(7, 0, 0x00800000);
	puts("dcache invalidation test good :-) \n");

	set_cp2(10, 0, 0);
	puts("synchronization done!\n");

	// tlb inval
	puts("TLB invalidation test begin: \n");
	set_cp2(4, 0, 0x00400000);
	puts("itlb invalidation test good :-) \n");
	set_cp2(5, 0, 0x00800000);
	puts("dtlb invalidation test good :-) \n");

	uint32_t mmu_paras = get_cp2(15, 0);
	uint32_t mmu_release = get_cp2(16, 0);

	printf(" mmu paras are : %x\n", mmu_paras);
	printf(" mmu release is : %x\n", mmu_release);

	const int cpu = procnum();

	printf("Hello from processor %d\n", procnum());

	set_irq_handler(irq_handler);
	enable_hw_irq(0);
	irq_enable();
	
	soclib_io_set(
		base(V_TIMER),
		procnum()*TIMER_SPAN+TIMER_PERIOD,
		period[cpu]);
	soclib_io_set(
		base(V_TIMER),
		procnum()*TIMER_SPAN+TIMER_MODE,
		TIMER_RUNNING|TIMER_IRQ_ENABLED);

	while (1)
		pause();
	return 0;
}
