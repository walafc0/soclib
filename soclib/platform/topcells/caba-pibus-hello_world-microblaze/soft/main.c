#include "soclib/tty.h"
#include "uputs.h"
#define ttyputs(x) uputs(0x00400000, x)

#define get_fsl(x)                         \
({unsigned int __fsl_x;                    \
  __asm__("get %0, rfsl"#x:"=r"(__fsl_x)); \
  __fsl_x;\
})

static inline int procnum()
{
    return get_fsl(0);
}

#define MAZEX 20
#define MAZEY 40
#define INITSEED 1

static long maze[MAZEX][MAZEY];
/*\
 * Random number generator due to Don Knuth, from the Stanford Graph Base
\*/
#define gb_next_rand()(*gb_fptr>=0?*gb_fptr--:gb_flip_cycle())
#define mod_diff(x,y)(((x)-(y))&0x7fffffff)
#define two_to_the_31 ((unsigned long)0x80000000)
static long A[56]= {-1};
long *gb_fptr= A;

long gb_flip_cycle(void)
{
register long*ii,*jj;

   for(ii= &A[1],jj= &A[32];jj<=&A[55];ii++,jj++)
      *ii= mod_diff(*ii,*jj);
   for(jj= &A[1];ii<=&A[55];ii++,jj++)
      *ii= mod_diff(*ii,*jj);
   gb_fptr= &A[54];
   return A[55];
}

void gb_init_rand(long seed)
{
register long i;
register long prev= seed,next= 1;

   seed= prev= mod_diff(prev,0);
   A[55]= prev;
   for(i= 21;i;i= (i+21)%55){
      A[i]= next;

      next= mod_diff(prev,next);
      if(seed&1)seed= 0x40000000+(seed>>1);
      else seed>>= 1;
      next= mod_diff(next,seed);
      prev= A[i];
   }

   (void)gb_flip_cycle();
   (void)gb_flip_cycle();
   (void)gb_flip_cycle();
   (void)gb_flip_cycle();
   (void)gb_flip_cycle();
}

long gb_unif_rand(long m)
{
register unsigned long t = two_to_the_31-(two_to_the_31%m);
register long r;

   do {
      r = gb_next_rand();
   } while (t<=(unsigned long)r);
   return r%m;
}

#define srandom(x) gb_init_rand(x)
#define random() gb_next_rand()

void phint(int x)
{
   static const char hex[] = "0123456789ABCDEF";
   char res[] = "0x........";
   int i;
   for (i = 0; i < 8; i++) 
      res[2+7-i] = hex[(x>>(i<<2)) & 0xF];
   ttyputs(res);
}

/*\
 * Build a maze and find the shortest path in it using Lee
 * (or Acker's, I really do not remember anymore) algorithm
\*/
void generatemaze(void);
int propagate( int i, int j, long v);
int solvemaze(int ox, int oy, int dx, int dy);
int buildpath(int ox, int oy, int dx, int dy);
void displaymaze(void);

void lee(void)
{
int xd, yd, xa, ya;
int seed;

   srandom(3);
   xd = random()%MAZEX;
   yd = random()%MAZEY;
   while((xa = random()%MAZEX) == xd);
   while((ya = random()%MAZEY) == yd);
   ttyputs("Starting point : ");
   phint(xd); ttyputs(", ");
   phint(yd); ttyputs(")\n");
   ttyputs("Ending point : ");
   phint(xa); ttyputs(", ");
   phint(ya); ttyputs(")\n");
   generatemaze();
   displaymaze();

   if ((seed = solvemaze(xd, yd, xa, ya)) == 0)
      ttyputs("Maze without solution\n");
   else {
      buildpath(xd, yd, xa, ya);
      displaymaze();
   }
}

void generatemaze(void)
{
int i, j;

   for (i = 0; i < MAZEX; i++)
      for (j = 0; j < MAZEY; j++)
         if (random()%3 == 0)
            maze[i][j] = -1;
         else 
            maze[i][j] = 0;
}

/* solve:
   we start from the coordinate ox,oy and want to go up to dx,dy. */

int propagate( int i, int j, long v)
{
   if (i < 0 || i >= MAZEX || j < 0 || j >= MAZEY)
      return 0;
   if (maze[i][j] == 0) {
      maze[i][j] = v;
      return 1;
   } else 
      return 0;
}

int solvemaze(int ox, int oy, int dx, int dy)
{
int i = 0, j = 0;
int seed = INITSEED;
int end;

   maze[ox][oy] = INITSEED;
   maze[dx][dy] = 0;
   while (1) {
      end = 0;
      for (i = 0; i < MAZEX; i++) {
         ttyputs(".");
         for (j = 0; j < MAZEY; j++) {
            if (maze[i][j] == seed) {
               end |= propagate(i - 1, j, seed + 1);
               end |= propagate(i + 1, j, seed + 1);
               end |= propagate(i, j - 1, seed + 1);
               end |= propagate(i, j + 1, seed + 1);
            }
         }
      }
      if (maze[dx][dy] != 0)
         return 1;
      if (end == 0)
         break;
      seed++;
   }
   return 0;
}

int buildpath(int ox, int oy, int dx, int dy)
{
int i = dx, j = dy;
int seed = maze[dx][dy];

   maze[dx][dy] = - 2;
   while (1) {
      seed--;
      if (maze[i - 1][j] == seed)
         i--, maze[i][j] = -2;
      else if (maze[i + 1][j] == seed)
         i++, maze[i][j] = -2;
      else if (maze[i][j - 1] == seed)
         j--, maze[i][j] = -2;
      else if (maze[i][j + 1] == seed)
         j++, maze[i][j] = -2;
      else {
         ttyputs("Unbelievable error...\n");
      }
      if (seed == INITSEED)
         break;
   }
}

void displaymaze(void)
{
int i, j;

   ttyputs("\n");
   for (i = 0; i < MAZEX; i++) {
      for (j = 0; j < MAZEY; j++)
         if (maze[i][j] == -1)
            ttyputs("*");
         else if (maze[i][j] == -2)
            ttyputs("#");
         else
            ttyputs(" ");
      ttyputs("\n");
   }
}

int fact(int n)
{
   int r;

   phint(n); ttyputs("\n");
   if (n <= 1)
      r = 1;
   else
      r = n * fact(n-1);
   phint(n); ttyputs("\n");
   return r;
}

int main(void)
{
   ttyputs("Hello world on processor "); phint(procnum()); ttyputs("\n");
   ttyputs("Computing 7! recursively\n");
   phint(fact(7)); ttyputs("\n");
   ttyputs("Solving a random maze using Lee algorithm\n");
   lee();
   return 0;
}

void __start(void)
{
   extern unsigned int _edata, _end; /* Known 4 words alignment */
   unsigned int *p;
   /* Clear bss just in case some function requires it ! */
   for (p = &_edata; p < &_end; p++)
      *p = 0;
   main();
   while (1);
}
