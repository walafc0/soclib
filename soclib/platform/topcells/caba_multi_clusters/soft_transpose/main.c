#include "stdio.h"

#define NL		128
#define NP		128
#define NMAX		20

///////////////////////////////////////////
// tricks to read parameters from ldscript
///////////////////////////////////////////

struct plaf;

extern struct plaf seg_heap_base;
extern struct plaf segment_increment;
extern struct plaf NB_PROCS;

/////////////
void main()
{
    unsigned int	id = procid();
    unsigned int 	image = 0;

    unsigned int	c;	// cluster index for loops
    unsigned int	l;	// line index for loops
    unsigned int	p;	// pixel index for loops

    // base address for the shared buffers
    unsigned int	BASE      = (unsigned int)&seg_heap_base;
    // adress increment between two clusters
    unsigned int	INCREMENT = (unsigned int)&segment_increment;
    // number of parallel tasks (one task per processor / one processor per cluster)
    unsigned int	NC = (unsigned int)&NB_PROCS;
    // number of blocks per image (one block is 128 pixels
    unsigned int	NBLOCKS   = (NP*NL) / 128;
    // number of pixels in a local buffer (A or B)
    unsigned int	NSTRIPE    = (NP*NL) / NC;
    // number of lines in A buffer
    unsigned int	NLR        = NL / NC;
    // number of pixels in B buffer
    unsigned int	NPR        = NP / NC;

    // NC parameter checking
    if( NC > 128 )
    tty_printf("NC must be a power of 2 between 1 & 128");
    if( NL % NC != 0 )
    tty_printf("NC must be a power of 2 divider of NL");
    if( NP % NC != 0 )
    tty_printf("NC must be a power of 2 divider of NP");

    // Arrays of pointers on the shared, distributed buffers  
    // containing the images (sized for the worst case : 128 clusters)
    unsigned char*	A[128];
    unsigned char*	B[128];
    
    // shared variables address definition 
    // from the seg_heap_base and segment_increment 
    // values defined in the ldscript file.
    // These arrays of pointers are identical and
    // replicated in the stack of each task 
    for( c=0 ; c<NC ; c++)
    {
        A[c] = (unsigned char*)(BASE + INCREMENT*c);
        B[c] = (unsigned char*)(BASE + NL*NP + INCREMENT*c);
    }

    tty_printf("\n *** Processor %d running ***\n\n", id);

    tty_printf("NCLUSTER  = %d\n", NC); 
    tty_printf("NLINES    = %d\n", NL); 
    tty_printf("NPIXELS   = %d\n", NP); 

    //  barriers initialization
    barrier_init(0,NC);
    barrier_init(1,NC);
    barrier_init(2,NC);

    // Main loop (on images)
    while(image < NMAX) 
    {
        // parallel load from disk to A[c] buffers
        tty_printf("\n *** Starting load for image %d at cycle %d \n", image, proctime());
        if( ioc_read(image*NBLOCKS + NBLOCKS*id/NC , A[id], NBLOCKS/NC) )
        {
            tty_printf("echec ioc_read\n");
            exit();
        }
        if ( ioc_completed() )
        {
            tty_printf("echec ioc_completed\n");
            exit();
        }
        tty_printf(" *** Completing load for image %d at cycle %d \n", image, proctime());

        barrier_wait(0);

        // pseudo parallel transpose from A[c] to B[c] buffers
        tty_printf("\n *** starting transpose for image %d at cycle %d\n", image, proctime());
        for ( l=0 ; l<NLR ; l++)
        {
            for ( p=0 ; p<NP ; p++)
            {
/*
tty_printf("A[%d][%d,%d] => ", id, p, l);
tty_printf("B[%d][%d,%d]\n", p/NPR, l+id*NLR, p%NPR);
*/
                B[p/NPR][((p%NPR)*NL) + l + (id*NLR)] = A[id][l*NP + p];
            }
        }
        tty_printf(" *** Completing transpose for image %d at cycle %d \n", image, proctime());

        barrier_wait(1);

        // parallel display from B[c] to frame buffer using DMA
        tty_printf("\n *** starting display for image %d at cycle %d\n", image, proctime());
        if ( fb_write(id*NSTRIPE, B[id], NSTRIPE) )
        {
            tty_printf("echec fb_sync_write\n");
            exit();
        }
        if ( fb_completed() )
        {
            tty_printf("echec fb_completed\n");
            exit();
        }
        tty_printf(" *** completing display for image %d at cycle %d\n", image, proctime());

        barrier_wait(2);

        // next image
        image++;
    } // end while image      
    exit();
} // end main()

