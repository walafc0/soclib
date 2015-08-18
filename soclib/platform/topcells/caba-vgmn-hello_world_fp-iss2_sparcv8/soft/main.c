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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>, 2006-2007
 *
 * Maintainers: Alexis
 */

#include "stdio.h"
#include "system.h"
#include "../segmentation.h"

int print_float(double a)
{
	int int_part = (int) a;
	printf("%d.", int_part);
	a = a - int_part;
	printf("%d\n", (int)(a*1000000));
}

int main(void)
{
	float fa, fb, fc, fd;
	double da, db, dc, dd;
	int ia, ib, ic, id;

	fa = 2;
	printf("2="); print_float(fa);
	printf("sqrt(2)="); print_float(sqrt(fa));
	printf("sqrt(2)/2="); print_float(sqrt(fa)/2.0);
	printf("done\n");
	while(1);
	return 0;
}
