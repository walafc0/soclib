#include "stdio.h"

int main()
{
	int		opa;
	int		opb;
	int		res;
	int		iter=0;

	volatile char	c;
	volatile int	s;

	while(1) {
	iter++;
	// génération opérandes
	opa = rand();
	opb = rand();

	// écriture opérandes
	gcd_set_opa(opa);
	gcd_set_opb(opb);

	// démarrage
	gcd_start();

	// test status
	s = 1;
	while( s!=0 ) { gcd_get_status((int*)&s); }

	// lecture résultat
	gcd_get_result(&res);

	//affichage
	tty_printf("############### iter = %d ###############\n",iter);
	tty_printf("   - cycle = %d\n",proctime());
	tty_printf("   - opa   = %d\n",opa);
	tty_printf("   - opb   = %d\n",opb);
	tty_printf("   - pgcd  = %d\n",res);

	// next computation
	tty_getc((char*)&c);

	}

} // end main
