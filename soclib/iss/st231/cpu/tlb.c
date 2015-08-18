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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "state.h"
#include "tlb.h"
#include "ctrlregdefs.h"
#include "memory.h"


static struct tlb_entry utlb[NUM_UTLB_ENTRIES];
static struct tlb_entry *itlb[NUM_ITLB_ENTRIES];
static struct tlb_entry *dtlb[NUM_DTLB_ENTRIES];



struct tlb_entry *last_dtlb;
struct tlb_entry *last_itlb;

int tlb_index;


void print_tlb_entry(struct tlb_entry *t)
{
  static char *prot_str[]={
    "NONE", 
    "X",
    "R",
    "RX",
    "W",
    "WX",
    "RW",
    "RWX"
  };

  if(!t) return;

  fprintf(stdout,"vaddr 0x%08x paddr 0x%08x page_mask 0x%08x\n",
	  t->vaddr,t->paddr,t->page_mask);
  fprintf(stdout,"asid 0x%x user %s super %s shared %d dirty %d cache %d size %d\n",
	    t->asid,prot_str[t->prot>>3],prot_str[t->prot&0x7],t->shared,
	  t->dirty,t->cache_policy,t->size);
  if ((t >= utlb) && (t<&utlb[NUM_UTLB_ENTRIES])) {
    fprintf(stdout,"UTLB index %d, ", t-utlb);
  } else {
    fprintf(stdout,"Not in UTLB, ");
  }
  fprintf(stdout, "%s ITLB, %s DTLB\n",
	  t->micro_i ? "In" : "Not in",
	  t->micro_d ? "In" : "Not in");
  fprintf(stdout,"TLB0 %08x TLB1 %08x TLB2 %08x TLB3 %08x\n",
	t->entry[0], t->entry[1], t->entry[2], t->entry[3]);
}

/* Blats the ITLB */
void invalidate_itlb(void)
{
  int i;

  last_itlb=NULL;

  for(i=0;i<NUM_ITLB_ENTRIES;i++) {
    if(itlb[i]) {
      itlb[i]->micro_i=NULL;
      itlb[i]=NULL;
    }
  }
}


/* Blats the DTLB */
void invalidate_dtlb(void)
{
  int i;

  last_dtlb=NULL;

  for(i=0;i<NUM_DTLB_ENTRIES;i++) {
    if(dtlb[i]) {
      dtlb[i]->micro_d=NULL;
      dtlb[i]=NULL;
    }
  }
}

void invalidate_last_tlb_cache(void) 
{
  last_itlb=last_dtlb=NULL;
}


/* Both the following functions are inlined, so the code generated
 * for ITLB and DTLB lookup should be optimal, as constants are 
 * passed in for most params 
 */



/* lookup UTLB array. The extype is which exception to throw if 
 * something goes horribly wrong. The perms field has one bit set
 * depending on what permission you are trying to access 
 * (read/write/exec/super/user)
 */

static inline struct tlb_entry *lookup_utlb(unsigned virtual)
{
  unsigned masked_addr;
  struct tlb_entry *t; 
  int i;

  for(i=0;i<NUM_UTLB_ENTRIES;i++) {
    t=utlb+i;

    if(t->page_mask==0) continue; /* no entry */
         
    if(!(t->shared || (t->asid==asid))) continue; /* No match on asid */
  
    masked_addr=virtual&t->page_mask;
    if(masked_addr==t->vaddr) return t;
  }

  return NULL;
}

static inline struct tlb_entry *lookup_micro_tlb(struct tlb_entry **micro,
						 int micro_size,
						 const unsigned virtual)
{
  unsigned masked_addr;
  struct tlb_entry *t;
  int i;
  
  /* Ok ,so our cache of last access doesn't work. Bugger. Try the ITLB */
  /* We do have to check super/user mode for this one, as this is not flushed
   * on super/user transition. We don't have to check ASID, as this is 
   * flushed on ASID change.
   */  
  for(i=0;i<micro_size;i++) {
    t=micro[i];
    if(t==NULL) continue;  /* Nothin in this one */
    masked_addr=virtual&t->page_mask;
    if(masked_addr==t->vaddr) return t;
  }

  return NULL;
}



#define NO_MAPPING     TLB_EXCAUSE_CAUSE_NO_MAPPING	     
#define PROT_VIOLATION TLB_EXCAUSE_CAUSE_PROT_VIOLATION
#define WRITE_TO_CLEAN TLB_EXCAUSE_CAUSE_WRITE_TO_CLEAN	


static void throw_iside_exception(unsigned cause,unsigned virtual, struct tlb_entry* t)
{
  struct tlb_entry *u;

  exception|=ITLB_EXCEPTION;

  tlb_cause=cause;
  
  if(cause!=NO_MAPPING) {
    u=lookup_utlb(virtual);    
    if(u) {
      int index=u-utlb;
      /* Set the index bit */
      tlb_cause|=(index|(1<<20));
    }     
  }

  except_addr=virtual;
  except_tlb=t;
}

/* Return the physical address of the virtual */
/* This is only called for lookups from the I side */
/* Does not return tlb entry as we don't care about cache etc */
/* exception will be set on error */
struct tlb_entry * slow_iside_lookup_tlb(unsigned virtual) 
{
  struct tlb_entry *t;
  unsigned perms;

  perms=(psw&PSW_USER_MODE) ? PROT_USER_EXECUTE:PROT_SUPER_EXECUTE;
 
  t=lookup_micro_tlb(itlb,NUM_ITLB_ENTRIES,virtual);
  if(t) {
    if(((t->prot)&perms)==perms) {
      last_itlb=t; /* We will go straight there for next check */
    }else {
      throw_iside_exception(PROT_VIOLATION,virtual,t);
    }
    return t;
  }

  /* Oh well, we have to scan the whole bloody lot. Of course, we have 
   * to check full permissions ASIDs etc.
   */  
  t=lookup_utlb(virtual);

  if(t) {	
    if(((t->prot)&perms)==perms) {
      int itlb_entry=cycle_count&(NUM_ITLB_ENTRIES-1);
      struct tlb_entry *old;
      last_itlb=t; /* We will go straight there for next check */
      old=itlb[itlb_entry];      
      if(old) old->micro_i=NULL; /* No longer in ITLB */
      /* Update new pointer */
      itlb[itlb_entry]=t;
      t->micro_i=itlb+itlb_entry; /* Set new entry */
    }else {
      throw_iside_exception(PROT_VIOLATION,virtual,t);
    }
    return t;
  }

  throw_iside_exception(NO_MAPPING,virtual,NULL);
  return NULL;
}



static void throw_dside_exception(unsigned cause,unsigned virtual,int read,int diss,struct tlb_entry *t)
{
  struct tlb_entry *u;

  exception|=DTLB_EXCEPTION;

  tlb_cause=cause;
  
  tlb_cause|=(diss<<18)|((!read)<<19);

  if(cause!=NO_MAPPING) {
    u=lookup_utlb(virtual);    
    if(u) {
      int index=u-utlb;
      /* Set the index bit */
      tlb_cause|=(index|(1<<20));
    }     
  }

  except_addr=virtual;
  except_tlb=t;
}


/* We need to know if we are doing a read or write. Dissmissable
 * loads will have to check if an exception would have been 
 * thrown and clear the exception down as appropriate
 */

struct tlb_entry dummy; /* A zero entry. Used if CTRL or TLB not enabled */  

struct tlb_entry * dside_lookup_tlb(unsigned virtual,int read,int diss) 
{
  unsigned masked_addr;
  struct tlb_entry *t;
  unsigned perms;


  if(virtual>=CTRL_REG_START || !(psw&PSW_TLB_ENABLE)) return &dummy;

  perms= (read) ? PROT_SUPER_READ : PROT_SUPER_WRITE;

  if(psw&PSW_USER_MODE) perms<<=3; /* Change to user bit ! */

  /* last_dtlb always points at either a valid shared entry, or 
   * the ASID matches the current ASID. No need to check ASID, 
   * but alas we always have to check permission due to read/write
   */
  if(last_dtlb) {
    t=last_dtlb;
    masked_addr=virtual&t->page_mask;
    /* We have to check permissions I'm afraid, due to read/write */
    if(masked_addr==t->vaddr) {
      if( ((t->prot)&perms)==perms) {
      }else {
	throw_dside_exception(PROT_VIOLATION,virtual,read,diss,t);
	return t;
      }
      /* Check for write to clean */
      if(!read && !t->dirty) {
	throw_dside_exception(WRITE_TO_CLEAN,virtual,0,diss,t);
      }
      return t;
    }
  }

#ifndef OLD_TLB_SPEC 
  t=lookup_micro_tlb(dtlb,NUM_DTLB_ENTRIES,virtual);
  if(t) {
    if(((t->prot)&perms)==perms) {
      last_dtlb=t; /* We will go straight there for next check */
    }else {
      throw_dside_exception(PROT_VIOLATION,virtual,read,diss,t);
      /* set tlb cause to protection violation */
      return t;
    }
    /* Check for write to clean */
    if(!read && !t->dirty) {
      throw_dside_exception(WRITE_TO_CLEAN,virtual,0,diss,t);
    }
    return t;
  }
#endif

  t=lookup_utlb(virtual);

  if(t) {	
    if(((t->prot)&perms)==perms) {
      struct tlb_entry *old;
      int dtlb_entry=cycle_count&(NUM_DTLB_ENTRIES-1);

      last_dtlb=t; /* We will go straight there for next check */
#ifndef OLD_TLB_SPEC
      old=dtlb[dtlb_entry];
      if(old) old->micro_d=NULL; /* No longer in DTLB */
      /* Update new pointer */
      dtlb[dtlb_entry]=t;
      t->micro_d=dtlb+dtlb_entry;
#endif

    }else {
      throw_dside_exception(PROT_VIOLATION,virtual,read,diss,t);
      return t;
    }

    /* Check for write to clean */
    if(!read && !t->dirty) {
      throw_dside_exception(WRITE_TO_CLEAN,virtual,0,diss,t);
    }

    return t;
  }

  /* If we get as far as here, we have to give up. It just aint gona happen!! */
  throw_dside_exception(NO_MAPPING,virtual,read,diss,NULL);

  return NULL;

}

/* This data structure holds copies of TLB entries that have to be copied
 * out of the main TLB when a new entry is inserted
 */
#define MICRO_COPY_BUFFER_SIZE (NUM_ITLB_ENTRIES+NUM_DTLB_ENTRIES+1)

static struct tlb_entry micro_copy[MICRO_COPY_BUFFER_SIZE];

/* points to a free entry in above - always valid */
static  int free_micro_index=0;

#ifdef OLD_TLB_SPEC

#define TLB_HI_ASID_MASK 0xff
#define TLB_HI_ASID(x) ((x)& TLB_HI_ASID_MASK)

#define TLB_HI_SHARED_MASK (1<<8)
#define TLB_HI_SHARED(x) (((x)&TLB_HI_SHARED_MASK)>>8)

#define TLB_HI_SIZE_MASK ((0x3)<<9)
#define TLB_HI_SIZE(x) ( ((x)&TLB_HI_SIZE_MASK)>>9)

#define TLB_HI_DIRTY_MASK (1<<11)
#define TLB_HI_DIRTY(x) (((x)&TLB_HI_DIRTY_MASK)>>11)

#define TLB_HI_VADDR_MASK ( ~((1<<13)-1))
#define TLB_HI_VADDR(x) ( ((x)&TLB_HI_VADDR_MASK)  >>13)

#define TLB_LO_PROT_MASK ( 0x3f)
#define TLB_LO_PROT(x)  (((x)&TLB_LO_PROT_MASK))

#define TLB_LO_PADDR_MASK ( ~((1<<9)-1))
#define TLB_LO_PADDR(x) ( (x)>>13 )

#else


#define TLB_ENTRY0_SIZE_MASK ((0x7)<<20)
#define TLB_ENTRY0_SIZE(x) ( ((x)&TLB_ENTRY0_SIZE_MASK)>>20)

#define TLB_ENTRY0_ASID_MASK 0xff
#define TLB_ENTRY0_ASID(x) ((x)& TLB_ENTRY0_ASID_MASK)

#define TLB_ENTRY0_SHARED_MASK (1<<8)
#define TLB_ENTRY0_SHARED(x) (((x)&TLB_ENTRY0_SHARED_MASK)>>8)

#define TLB_ENTRY0_PROT_MASK ( 0x3f<<9)
#define TLB_ENTRY0_PROT(x)  (((x)&TLB_ENTRY0_PROT_MASK)>>9)

#define TLB_ENTRY0_DIRTY_MASK (1<<15)
#define TLB_ENTRY0_DIRTY(x) (((x)&TLB_ENTRY0_DIRTY_MASK)>>15)

#endif



static void update_tlb_entry(int index)
{
  struct tlb_entry *t=utlb+index;
  int i;
 
  /* Do we have to make a copy of the tlb entry ??? */
  if(t->micro_i||t->micro_d) {
    struct tlb_entry *micro_free=micro_copy+free_micro_index;
    /* Copy over struct */
    memcpy(micro_free,t,sizeof(struct tlb_entry));
    
    /* Bugger. We do - curses */
    if(t->micro_i) {
      /* Change new struct to point at entry in micro tlb */
      *(t->micro_i)=micro_free;
      t->micro_i=NULL;
    }
    if(t->micro_d) {
      *(t->micro_d)=micro_free;
      t->micro_d=NULL;
    }
    /* We can probably optimise this I think - too hard for now */
    for(i=0;i<MICRO_COPY_BUFFER_SIZE;i++) {
      if((micro_copy[i].micro_i==NULL) && (micro_copy[i].micro_d==NULL)) {
	free_micro_index=i;
	break;
      }
    }	 
    if(i==MICRO_COPY_BUFFER_SIZE) {
      printf("AAAAAKKK - run out of room in micro copy buffer\n");
      exit(1);
    }
  }

#ifdef OLD_TLB_SPEC
  switch(TLB_HI_SIZE(t->entry[0])) {
#else
  switch(TLB_ENTRY0_SIZE(t->entry[0])) {
#endif
  case 0:      
    t->page_mask=0;    
    /* Disable this page */
    break;
  case 1:
    t->page_mask=PAGE_MASK_8K;
    /* 8KB PAGE */
    break;
  case 2:
    t->page_mask=PAGE_MASK_4M;
    /* 4MB page */
    break;
  case 3:
    t->page_mask=PAGE_MASK_256M;
    /* 256Mb page */
  }

  /* We should really mask out irrelevant bits here */

#ifdef OLD_TLB_SPEC
  t->paddr=TLB_LO_PADDR(t->entry[1])<<13;
  t->vaddr=TLB_HI_VADDR(t->entry[0])<<13;

  t->asid=TLB_HI_ASID(t->entry[0]);
  t->shared=TLB_HI_SHARED(t->entry[0]);
  t->prot=TLB_LO_PROT(t->entry[1]);
  t->dirty=TLB_HI_DIRTY(t->entry[0]);
  t->size=TLB_HI_SIZE(t->entry[0]);
#else
  t->paddr=t->entry[2]<<13;
  t->vaddr=t->entry[1]<<13;

  t->asid=TLB_ENTRY0_ASID(t->entry[0]);
  t->shared=TLB_ENTRY0_SHARED(t->entry[0]);
  t->prot=TLB_ENTRY0_PROT(t->entry[0]);
  t->dirty=TLB_ENTRY0_DIRTY(t->entry[0]);
  t->size=TLB_ENTRY0_SIZE(t->entry[0]);
#endif


  t->micro_i=NULL;
  t->micro_d=NULL;

  //  printf("ADDED NEW TLB ENTRY!! at index %d\n",index);
  //  print_tlb_entry(t);

  
  /* We might have changed it !! 
   * I'm not sure we need to do this - doesn't the fact that last
   * must be in the micro TLB mean this is not needed?
   */
  invalidate_last_tlb_cache();

}



void dump_utlb(void)
{
  int i;
  struct tlb_entry *t;

  for(i=0;i<NUM_UTLB_ENTRIES;i++) {
    t=utlb+i;
    if(t->page_mask==0) continue;
    printf("-------Entry %d\n",i);
    print_tlb_entry(t);
  }

}


void set_tlb_index(int index)
{
  if(index>=NUM_UTLB_ENTRIES) {
    printf("out of range write to TLB_INDEX reg\n");
    tlb_index&=(NUM_UTLB_ENTRIES-1);
  }

  tlb_index=index;
}

void write_tlb_register(int offset,unsigned value)
{
  utlb[tlb_index].entry[offset]=value;

    update_tlb_entry(tlb_index);
  
}

unsigned read_tlb_register(int offset)
{
  return   utlb[tlb_index].entry[offset];
}

