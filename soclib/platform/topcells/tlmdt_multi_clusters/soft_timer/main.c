#include "stdio.h"

/////////////
void main()
{
    char	byte;

    if ( timer_set_period(0, 50000) ) 
        tty_printf( "error timer_set_period\n");
    if ( timer_set_mode(0, 3) ) 
        tty_printf( "error timer_set_mode\n");

    while(1)
    {
       tty_printf("Hello World\n");
       tty_getc_irq(&byte);
    }
    exit();
}
