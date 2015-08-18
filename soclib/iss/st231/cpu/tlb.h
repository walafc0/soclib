/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Zheng LI (zheng.x.li@inria.fr)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 - Neither the name of the INRIA nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************** */

#ifndef __TLB_H__
#define __TLB_H__


#define PROT_SUPER_EXECUTE 1
#define PROT_SUPER_READ 2
#define PROT_SUPER_WRITE 4

#define PROT_USER_EXECUTE 8
#define PROT_USER_READ 16
#define PROT_USER_WRITE 32


#define CACHE_POLICY_UNCACHED 1
#define CACHE_POLICY_CACHED 2
#define CACHE_POLICY_WRITE_COMBINE 4

#define PAGE_SIZE_DISABLED 0
#define PAGE_SIZE_8K 1
#define PAGE_SIZE_4MB 2
#define PAGE_SIZE

#define NUM_UTLB_ENTRIES 64
#define NUM_DTLB_ENTRIES 8
#define NUM_ITLB_ENTRIES 8

#define PAGE_MASK_8K   (0xffffe000)
#define PAGE_MASK_4M   (0xffc00000)
#define PAGE_MASK_256M (0xf0000000)

/* Expanded TLB entry */
/* We expand these for speed of access. Question, is it better to use
 * more cache, or unpack on the fly?
 */
struct tlb_entry {
  unsigned vaddr;
  unsigned paddr;  
  unsigned page_mask; /* Mask to get translated bits. Precomputed */
  unsigned asid;
  unsigned prot; /* 6 bits of protection */
  int shared;  
  int dirty;
  struct tlb_entry **micro_i;
  struct tlb_entry **micro_d;
  int size; /* zero means disabled */
  int cache_policy;
  unsigned entry[4]; /* the "unexpanded entry, ie what the machine holds */
};

/* When adding new entries to the utlb, we have to be careful to check if 
 * the entry that was there is held in the micro tlb. If so, we have 
 * to copy it and update the pointer 
 */

/* Gives physical address assuming t points at the correct TLB entry */

#define TRANSLATE(t,virtual) ((t)->paddr+(virtual&(~((t)->page_mask))))

/* Blats the ITLB */
void invalidate_itlb(void);
/* Ditto for DTLB */
void invalidate_dtlb(void);

void invalidate_last_tlb_cache(void);

extern struct tlb_entry *last_itlb;


struct tlb_entry * slow_iside_lookup_tlb(unsigned virtual);

static inline struct tlb_entry * iside_lookup_tlb(unsigned virtual) 
{
  /* last_itlb always points at either a valid shared entry, or 
   * the ASID matches the current ASID. No need to check these 
   * We also don't have to check permissions, as changing from super 
   * to user clears this cache. Also it is never updated if you don't 
   * have execute in the first place
   */
  if(likely(last_itlb!=NULL)) {
    if((virtual & last_itlb->page_mask) == last_itlb->vaddr) return last_itlb;
  }

  /* Damn we are on the slow path, either a micro or a full UTLB lookup */
   
  return slow_iside_lookup_tlb(virtual);

}





struct tlb_entry * dside_lookup_tlb(unsigned virtual,int read,int diss);

/* Future writes to the tlb index register will go here */
void set_tlb_index(int index);

/* Changing any value will cause the TLB's to be updated */
void write_tlb_register(int offset,unsigned value);

void dump_utlb(void);

#endif
