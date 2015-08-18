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
 * Author: Frédéric Pétrot <Frederic.Petrot@imag.fr>
 *
 * Boot:
 * Just what is strictly necessary to do some testing.
 * The .org values are specified in the MicroBlaze data-sheet.
\*/
                .section .boot,"ax",@progbits
	.extern __start
	.extern _stack_top
	.globl  __boot
	.ent    __boot

                .org 0x00
__boot:
                bri .Lgo

                .org 0x08
_exception_vector:
                bri _exception_vector

                .org 0x10
_interrupt_vector:
                bri __interrupt_handler

                .org 0x18
_break_vector:
                bri _break_vector

                .org 0x20
_hw_exception_vector:
                bri _hw_exception_vector

                .text
                .org 0x50
.Lgo:
                /*\
                 * Enables interrupts 
                \*/
                ori     r3, r0, 2
                mts     rmsr, r3
	addik   r1, r0, _stack_top
	bri     __start
_boot_end:
	bri 	_boot_end
	.end	_boot
