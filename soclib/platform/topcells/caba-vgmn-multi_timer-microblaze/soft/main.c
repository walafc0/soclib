/*\
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MutekH; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author: Dominique Houzet <houzet@lis.inpg.fr>
\*/

#include "stdint.h"
#include "soclib/pci.h"
#include "system.h"

static const int period[4] = {10000, 11000, 12000, 13000};

int main(void)
{
   const int cpu = procnum();
   int cpt;

   uputs("Salut du processeur ");
   puti(cpu);
   putc('\n');
   

if (cpu==0) { soclib_io_set(base(PCI),PCI_NB,0);  
 soclib_io_set(base(PCI),PCI_ADR,23);
   soclib_io_set(base(PCI),PCI_VALUE,8);
   soclib_io_set(base(PCI),PCI_VALUE,12);
   soclib_io_set(base(PCI),PCI_VALUE,14);
     soclib_io_set(base(PCI),PCI_VALUE,16);    
   soclib_io_set( base(PCI),PCI_MODE,PCI_DMA_READ_NO_IRQ ); 
   soclib_io_set(base(PCI),PCI_NB,16);  
   

  
   while (1) {
      putc('.');  
      cpt=soclib_io_get(base(PCI), PCI_VALUE);  puti(cpt); putc(' ');
} }
   return 0;
}


/*\
 * All processors share the same stack until here
\*/
void __start(void)
{
   extern unsigned int _edata, _end; /* Known 4 words alignment */
   unsigned int *p;
#if 0
   static volatile int wait = 1;
   /* Clear bss just in case some function requires it ! */
   if (procnum() == 0) {
      for (p = &_edata; p < &_end; p++)
         *p = 0;
      wait = 0;
   } else {
      while (wait) {
         puti(procnum());
         putc(' ');
      }
   }
#endif

   /* Setting a different stack per processor */
   set_r1(-procnum() * 1024);
   main();
   while (1);
}
