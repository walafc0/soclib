#include "stdio.h"
#include "matrice.h"

#define SIZE 		500 
#define SORT_TYPE 	1 

unsigned int SortArray[SIZE+200];

void SORT(unsigned int *base, unsigned int n, int type);
void insertion_sort(unsigned int *base, unsigned int n); // type 2
void selection_sort(unsigned int *base, unsigned int n); // type 1
void bubble_sort(unsigned int *base, unsigned int n);    // type 3
void shellSortPhase(unsigned int a[],unsigned int length, int gap);
void shellSort(unsigned int *base, unsigned int n);     // type 0

int main()
{
    unsigned	int beg_cycle;
    unsigned 	int end_cycle;
    int i;

    beg_cycle = proctime();

    tty_puts("Memory copy... \n");
    for(i=0 ; i<SIZE ; i++) 
    {
        SortArray[i] = gQSortNum0[i];
    }

    tty_puts("Sorting... \n");
    SORT((unsigned int *) (SortArray), (unsigned int) SIZE, SORT_TYPE);

    tty_puts("\nChecking... \n");
    for (i = 1; i < SIZE; i++)
    {
        if (SortArray[i] < SortArray[i-1]) tty_puts("Failure\n");
    }

    tty_puts("Success\n");
    end_cycle = proctime();
    tty_printf( "cycles = %d\n", end_cycle-beg_cycle);

    exit();
    return 0;
}


//---- insertion sort : non adapté pour tableaux de grande taille (> 100) --
void insertion_sort(unsigned int *base, unsigned int n) 
{
    /* Spécifications externes : Tri du tableau base par insertion séquentielle */
    int i,p,j;
    int x;

    tty_puts("Insertion Sort\n");

    for (i = 1; i < n; i++) 
    {
        tty_putc('-'); // added for debug

        /* stockage de la valeur en i */
        x = base[i]; 
 
        /* recherche du plus petit indice p inférieur à i tel que base[p] >= base[i] */
        for(p = 0; base[p] < x; p++);
        /* p pointe une valeur de base supérieure à celle en i */ 
 
        /* décalage avant des valeurs de base entre p et i */         
        for (j = i-1; j >= p; j--) {
            base[j+1] = base[j]; 
        }   
 
        base[p] = x; /* insertion de la valeur stockée à la place vacante */

        tty_putc('+'); // added for debug

    }
}

//------ simple_sort -------------------------------
void selection_sort(unsigned int *base, unsigned int n)
{
     int i, min, j , x;
     tty_puts("Selection Sort\n");

     for(i = 0 ; i < n - 1 ; i++)
     {

         tty_putc('-'); // added for debug

         min = i;

         
         for(j = i+1 ; j < n ; j++)
         {
        
            if(base[j] < base[min])
                  min = j;
            
         }

         if(min != i)
         {
             x = base[i];
             base[i] = base[min];
             base[min] = x;
         }

         tty_putc('+'); // added for debug

     }
}
//-------------------------------
void bubble_sort(unsigned int *base, unsigned int n)
{
        int i   = 0; /* Indice de répétition du tri */
 	int j   = 0; /* Variable de boucle */
 	int tmp = 0; /* Variable de stockage temporaire */
        int en_desordre = 1; /* Booléen marquant l'arrêt du tri si le tableau est ordonné */

        tty_puts("Bubble Sort\n");

	/* Boucle de répétition du tri et le test qui arrête le tri dès que le tableau est ordonné */
	for(i = 0 ; (i < n) && en_desordre; i++)
	{
                tty_putc('-'); // added for debug

		/* Supposons le tableau ordonné */
		en_desordre = 0;
		/* Vérification des éléments des places j et j-1 */
		for(j = 1 ; j < n - i ; j++)
		{
			/* Si les 2 éléments sont mal triés */
			if(base[j] < base[j-1])
			{
				/* Inversion des 2 éléments */
 				tmp = base[j-1];
 				base[j-1] = base[j];
 				base[j] = tmp;
 
 				/* Le tableau n'est toujours pas trié */
				en_desordre = 1;
 			}
		}

                tty_putc('+'); // added for debug
	}

}
//------------------------------------------------------
/*
 * Exécute un tri par insertion avec la séparation donnée
 * If gap == 1, on fait un tri ordinaire.
 * If gap >= length, on ne fait rien.
 */
void shellSortPhase(unsigned int a[],unsigned int length, int gap) {
    int i;
 
    tty_putw(gap);
    for (i = gap; i < length; ++i) {
        unsigned int value = a[i];
        int j;
        for (j = i - gap; j >= 0 && a[j] > value; j -= gap) {
	    tty_putc('+');
            a[j + gap] = a[j];
	    tty_putc('-');
        }
        a[j + gap] = value;
    }
}
 
void shellSort(unsigned int *base, unsigned int n) {
    /*
     * gaps[] doit approximer une Série géométrique.
     * La sequence suivante est la meilleure connue en terme
     * de nombre moyen de comparaisons. voir:
     * http://www.research.att.com/~njas/sequences/A102549
     */
    static const int gaps[] = {
        1, 4, 10, 23, 57, 132, 301, 701
    };
    int sizeIndex;
 
    tty_puts("Shell Sort\n");
    for (sizeIndex = sizeof(gaps)/sizeof(gaps[0]) - 1;
               sizeIndex >= 0;
               --sizeIndex)
        shellSortPhase(base, n, gaps[sizeIndex]);
}

//-------------------------------------*/
void SORT(unsigned int *base, unsigned int n, int type)
{
  switch(type)
  {
  case 0:
    shellSort(base, n);
    break;
  case 1:
    selection_sort(base, n);
    break;
  case 2:
    insertion_sort(base, n);
    break;
  case 3:
    bubble_sort(base, n);
    break;
  default:
    break;
  }
}
