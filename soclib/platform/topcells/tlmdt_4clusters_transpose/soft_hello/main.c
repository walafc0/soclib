#include "stdio.h"

int main()
{
    volatile char	c;

    while(1) 
    {
        tty_puts("hello world\n");
        tty_getc_irq( (void*)&c );
    }
    return 0;            
} // end main
