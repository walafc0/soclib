/* Solving a maze using Lee algorithm, early global routing strategy
 * in O(n^2)
 * First version:
 * Frédéric Pétrot at Université Pierre et Marie Curie, 1989
 */
extern int putchar(int);
extern int printf(char *fmt, ...);
#define MAZEX 20
#define MAZEY 40
#define INITSEED 1

static int maze[MAZEX][MAZEY];
/* Random generator:
 * code by Don Knuth, the Stanford Graph Base
 * Thanks! */
#define gb_next_rand()(*gb_fptr>=0?*gb_fptr--:gb_flip_cycle())
#define mod_diff(x,y)(((x)-(y))&0x7fffffff)
#define two_to_the_31 ((unsigned int)0x80000000)
static int A[56]= {-1};
int*gb_fptr= A;

int gb_flip_cycle()
{
register int*ii,*jj;

   for(ii= &A[1],jj= &A[32];jj<=&A[55];ii++,jj++)
      *ii= mod_diff(*ii,*jj);
   for(jj= &A[1];ii<=&A[55];ii++,jj++)
      *ii= mod_diff(*ii,*jj);
   gb_fptr= &A[54];
   return A[55];
}

void gb_init_rand(seed)
int seed;
{
register int i;
register int prev= seed,next= 1;

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

int gb_unif_rand(m)
int m;
{
register unsigned int t= two_to_the_31-(two_to_the_31%m);
register int r;

   do {
      r= gb_next_rand();
   } while (t<=(unsigned int)r);
   return r%m;
}

void generatemaze();
int solvemaze();
void displaymaze();
void buildpath();

#define srandom(x) gb_init_rand(x)
#define urandom(x) gb_unif_rand(x)
void lee(int seed)
{
int xd, yd, xa, ya;

   srandom(seed);
   xd = urandom(MAZEX);
   yd = urandom(MAZEY);
   while((xa = urandom(MAZEX)) == xd);
   while((ya = urandom(MAZEY)) == yd);

   generatemaze(seed);
   displaymaze();

   if (solvemaze(xd, yd, xa, ya) == 0)
      printf("No way to go from %d, %d to %d,%d\n", xd, yd, xa, ya);
   else {
      buildpath(xd, yd, xa, ya);
      displaymaze();
   }
}

void generatemaze(int seed)
{
int i, j;

   srandom(seed);
   for (i = 0; i < MAZEX; i++)
      for (j = 0; j < MAZEY; j++)
         if (urandom(4) == 0)
            maze[i][j] = -1;
         else 
            maze[i][j] = 0;
}

/* solve:
   we start from the coordinate ox,oy and want to go up to dx,dy. */

int propagate(i, j, v)
int i, j;
int v;
{
   if (i < 0 || i >= MAZEX || j < 0 || j >= MAZEY)
      return 0;
   if (maze[i][j] == 0) {
      maze[i][j] = v;
      return 1;
   } else 
      return 0;
}

int solvemaze(ox, oy, dx, dy)
int ox, oy, dx, dy;
{
int i = 0, j = 0;
int seed = INITSEED;
int end;

   maze[ox][oy] = INITSEED;
   maze[dx][dy] = 0;
   while (1) {
      end = 0;
      for (i = 0; i < MAZEX; i++) {
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

void buildpath(ox, oy, dx, dy)
int ox, oy, dx, dy;
{
int i = dx, j = dy;
int seed = maze[dx][dy];

   maze[dx][dy] = - 2;
   while (1) {
      seed--;
      if (maze[i - 1][j] == seed)
         i--, maze[i][j] = - 2;
      else if (maze[i + 1][j] == seed)
         i++, maze[i][j] = - 2;
      else if (maze[i][j - 1] == seed)
         j--, maze[i][j] = - 2;
      else if (maze[i][j + 1] == seed)
         j++, maze[i][j] = - 2;
      else {
         printf("Unbelievable error...\n");
      }
      if (seed == INITSEED)
         break;
   }
}

void displaymaze(void)
{
int i, j;

   for (i = 0; i < MAZEX; i++) {
      for (j = 0; j < MAZEY; j++)
         putchar(maze[i][j] == -1 ? '*' : maze[i][j] == -2 ? '#' : ' ');
      putchar('\n');
   }
}
