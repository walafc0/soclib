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
 *         Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>, 2009
 *
 * Maintainers: Dimitri Refauvelet
 */

#include "system.h"
#include "stdio.h"

#include "../segmentation.h"

#define MMU_PADDR_TO_PPN1(x) (uint32_t)( x >> ( 40 - 19 ) )
#define MMU_VADDR_TO_PDE(x) (uint16_t)( ( x & 0xffe00000) >> ( 32 - 11 ) )
#define MMU_PADDR_TO_PDR(x) (uint32_t)( x >> ( 40 - 27 ) )


struct pte1
{
  uint32_t  address:19;
  uint32_t  reserved:3;
  uint32_t  dirty:1;
  uint32_t  global:1;
  uint32_t  user:1;
  uint32_t  executable:1;
  uint32_t  writable:1;
  uint32_t  cacheable:1;
  uint32_t  remote_access:1;
  uint32_t  local_access:1;
  uint32_t  type:1;
  uint32_t  valid:1;
}__attribute__((packed));;

struct directory{
  struct pte1 pte[2048];
};

uint32_t count;
uint32_t *data_w = (uint32_t *)0x20008000;
uint32_t *data_r = (uint32_t *)0x40008000;

static inline
void sleep(uint32_t cycle)
{
  while(cycle--)
    asm volatile("nop");
}

void prod()
{
  printf("Producteur: hello\n");
  *(data_w) = count;
  printf("Producteur: write %d to %p\n",count,data_w);
  data_w++;
}

void cons()
{
  printf("Consommateur: hello\n");
  if(*data_r == count)
    printf("Consommateur: read %d from %p\n",(*data_r),data_r);
  else
    printf("Consommateur: error data read\n");
  data_r++;
}

int main(void)
{
  struct directory *dir = (struct directory *)DATA_BASE;
  struct pte1 *pte_tmp;

  /*Activation des caches*/
  set_cp2(1, 0, 0x3);

  /*Remplissage de la table de pages*/
  /*Adresse correspondant au code:*/
  pte_tmp = &dir->pte[ MMU_VADDR_TO_PDE(TEXT_BASE) ];
  pte_tmp->valid = 1;
  pte_tmp->type = 0;
  pte_tmp->cacheable = 1;
  pte_tmp->writable = 1;/*La page contenant le code contient aussi les variable du code*/
  pte_tmp->executable = 1;
  pte_tmp->user = 0;
  pte_tmp->global = 1;

  pte_tmp->address = MMU_PADDR_TO_PPN1(TEXT_BASE);

  /*Adresse correspondant a la pile et tas*/
  pte_tmp = &dir->pte[ MMU_VADDR_TO_PDE(DATA_BASE) ];
  pte_tmp->valid = 1;
  pte_tmp->type = 0;
  pte_tmp->cacheable = 1;
  pte_tmp->writable = 1;
  pte_tmp->executable = 0;
  pte_tmp->user = 0;
  pte_tmp->global = 1;

  pte_tmp->address = MMU_PADDR_TO_PPN1(DATA_BASE);

  /*Adresse correspondant au TTY*/
  pte_tmp = &dir->pte[ MMU_VADDR_TO_PDE(TTY_BASE) ];
  pte_tmp->valid = 1;
  pte_tmp->type = 0;
  pte_tmp->cacheable = 0;
  pte_tmp->writable = 1;
  pte_tmp->executable = 0;
  pte_tmp->user = 0;
  pte_tmp->global = 1;

  pte_tmp->address = MMU_PADDR_TO_PPN1(TTY_BASE);


  /*Remplir les deux entrées correspondantes aux fifo consomateur et producteur*/
  /*Cons*/
  pte_tmp = &dir->pte[ MMU_VADDR_TO_PDE(0x40000000) ];
  pte_tmp->valid = 1;
  pte_tmp->type = 0;
  pte_tmp->cacheable = 0;
  pte_tmp->writable = 1;
  pte_tmp->executable = 0;
  pte_tmp->user = 0;
  pte_tmp->global = 1;

  pte_tmp->address = MMU_PADDR_TO_PPN1(0x10000000);

  /*Prod*/
  pte_tmp = &dir->pte[ MMU_VADDR_TO_PDE(0x20000000) ];
  pte_tmp->valid = 1;
  pte_tmp->type = 0;
  pte_tmp->cacheable = 0;
  pte_tmp->writable = 1;
  pte_tmp->executable = 0;
  pte_tmp->user = 0;
  pte_tmp->global = 1;

  pte_tmp->address = MMU_PADDR_TO_PPN1(0x10000000);


  /*Activation de la mmu*/
  set_cp2(0, 0, MMU_PADDR_TO_PDR(DATA_BASE));
  set_cp2(1, 0, 0xf);
  /*Producteur/Consomateur*/
  count = 0;
  while(1)
    {
      prod();
      sleep(100000);
      cons();
      sleep(100000);
      count++;
    }
} 
