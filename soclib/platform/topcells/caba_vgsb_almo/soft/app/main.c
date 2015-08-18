#include "stdio.h"

__attribute__((constructor)) void main()
{
	int	i;
	char	byte;

        if( timer_set_period(500000) ) 
        {
		tty_puts("echec timer_set_period\n");
		exit();
	}
        if( timer_set_mode(0x3) )
	{
		tty_puts("echec timer_set_mode\n");
		exit();
	}
	for( i=0 ; i<1000 ; i++ )
	{
		if( tty_printf(" hello world : %d\n",i) )
		{
			tty_puts("echec tty_printf\n");
			exit();
		}
                if( tty_getc_irq((void*)&byte) )
                {
			tty_puts("echec tty_getc_irq\n");
			exit();
		} 
                if(byte == 'q') exit();
	}
        exit();

} // end main

