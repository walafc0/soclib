#include "stdio.h"
#include "userio.h"

int main()
{
	volatile char	c;
	char		s[] = "\n Hello World! \n";

	icu_set_mask(0x3);
	timer_set_period(0, 500000);
	timer_set_mode(0, 0x3);
	while(1) {
	tty_printf(s);
	user_getc((char*)&c);
	}

} // end main
