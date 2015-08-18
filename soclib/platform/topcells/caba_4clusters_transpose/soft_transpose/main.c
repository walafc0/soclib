#include "stdio.h"

#define NLINES		128
#define NPIXELS		128
#define NBLOCKS		NLINES*NPIXELS/512
#define NIMAGES		5
#define FALSE		0
#define TRUE		1

volatile int	buf_in_empty	= TRUE;
volatile int	buf_out_empty 	= TRUE;

unsigned char 	buf_out[NLINES][NPIXELS];
unsigned char 	buf_in[NLINES][NPIXELS];

///////////
void load()
{
    int image = 0;

    tty_printf(" !!!  Processor %d running !!!\n", procid());

    while(image < NIMAGES) 
    {
        while ( buf_in_empty == FALSE ) {}	// synchro
        
        tty_printf("\n *** Starting load for image %d *** at cycle %d \n", image, proctime());
        if( ioc_read(image*NBLOCKS, buf_in, NBLOCKS) )
        {
            tty_printf("echec ioc_read \n");
            exit();
        }
        if ( ioc_completed() )
        {
            tty_printf("echec ioc_completed\n");
            exit();
        }

        buf_in_empty = FALSE;

        tty_printf(" *** Completing load for image %d *** at cycle %d \n", image, proctime());
        image++;
    } // end while image      
    exit();
} // end load()

////////////////
void transpose()
{
    int l,p;
    int image = 0;

    tty_printf(" !!!  Processor %d running !!!\n", procid());

    while( image < NIMAGES )
    { 
        while( (buf_in_empty == TRUE) || (buf_out_empty == FALSE) ) {}	// synchro

        tty_printf("\n *** Starting transpose for image %d *** at cycle %d \n", image, proctime());
        for( l=0 ; l<NLINES ; l++)
        {
            for( p=0 ; p<NPIXELS ; p++)
            {
                    buf_out[l][p] = buf_in[p][l];
            } 
        }

        buf_in_empty = TRUE;
        buf_out_empty = FALSE;

        tty_printf(" *** Completing transpose for image %d *** at cycle %d \n", image, proctime());
        image++;
    } // end while image
    exit();
} // end transpose

//////////////
void display()
{
    int	image = 0;

    tty_printf(" !!!  Processor %d running !!!\n", procid());

    while(image < NIMAGES)
    {
        while( buf_out_empty == TRUE ) {} // synchro

        tty_printf("\n *** starting display for image %d at cycle %d\n", image, proctime());
        if ( fb_write(0, buf_out, NLINES*NPIXELS) )
        {
                tty_printf("echec fb_sync_write\n");
                exit();
        }
        else
        {
            tty_printf("*** success fb_write ***\n");
        }

        if ( fb_completed() )
        {
            tty_printf("echec fb_completed\n");
            exit();
        }

        buf_out_empty = TRUE;

        tty_printf(" *** completing display for image %d at cycle %d\n", image, proctime());
        image++;
    } // end while image
    exit();
} // end display

