#include "../segmentation.h"
#include <stdarg.h>


int putchar(int c)
{
   *(volatile char *)(TTY_BASE+0) = c;
   return (unsigned char)c;
}

int puts(char *s)
{
   char c;
   while ((c = *s++) != 0)
      putchar(c);
   putchar('\n');
   return 0;
}
#define SIZE_OF_BUF 16

int printf(const char *fmt, ...)
{
   register char *tmp;
   int val, i, count = 0;
   char buf[SIZE_OF_BUF];
   va_list ap;
   va_start(ap, fmt);

   while (*fmt) {
      while ((*fmt != '%') && (*fmt)) {
         putchar(*fmt++);
         count++;
      }
      if (*fmt) {
       again:
         fmt++;
         switch (*fmt) {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
         case '.':
         case ' ':
            goto again;
         case '%':
            putchar('%');
            count++;
            goto next;
         case 'c':
            putchar(va_arg(ap, int));
            count++;
            goto next;
         case 'd':
         case 'i':
            val = va_arg(ap, int);
            if (val < 0) {
               val = -val;
               putchar('-');
               count++;
            }
            tmp = buf + SIZE_OF_BUF;
            *--tmp = '\0';
            do {
               *--tmp = val % 10 + '0';
               val /= 10;
            } while (val);
            break;
         case 'u':
            val = va_arg(ap, unsigned int);
            tmp = buf + SIZE_OF_BUF;
            *--tmp = '\0';
            do {
               *--tmp = val % 10 + '0';
               val /= 10;
            } while (val);
            break;
         case 'o':
            val = va_arg(ap, int);
            tmp = buf + SIZE_OF_BUF;
            *--tmp = '\0';
            do {
               *--tmp = (val & 7) + '0';
               val = (unsigned int) val >> 3;
            } while (val);
            break;
         case 's':
            tmp = va_arg(ap, char *);
            if (!tmp)
               tmp = "(null)";
            break;
         case 'p':
         case 'x':
         case 'X':
            val = va_arg(ap, int);
            tmp = buf + SIZE_OF_BUF;
            *--tmp = '\0';
            i = 0;
            do {
               char t = '0' + (val & 0xf);
               if (t > '9')
                  t += 'a' - '9' - 1;
               *--tmp = t;
               val = ((unsigned int) val) >> 4;
               i++;
            } while (val);
            if (*fmt == 'p') {
               while (i < 8) {
                  *--tmp = '0';
                  i++;
               }
               *--tmp = 'x';
               *--tmp = '0';
            }
            break;
         default:
            putchar(*fmt);
            count++;
            goto next;
         }
         while (*tmp) {
            putchar(*tmp++);
            count++;
         }
       next:
         fmt++;
      }
   }
   va_end(ap);
   return count;
}

const char const *s = "Hello World!\n";

unsigned int fact(unsigned int i)
{
   return i <= 1 ? 1 : i * fact(i-1);
}

extern void lee(int);

int _init(void)
{

   char const *t;
   char c;
   int i = 1, n;
   do {
      t = s;
      while ((c = *t++) != 0)
         printf("%c", c);
      printf("First, a computations of n!\n");
      printf("fact %d = %d\n", i, n = fact(i));
      i = (i + 1)%11;
      printf("then, solve a maze using Lee algorithm\n");
      lee(n);
   } while (1);
   return 0;
}

const char const *r = "I have been interrupted!\n";
int handler(void)
{
   char const *t;
   char c;
   t = r;
   while ((c = *t++) != 0)
      *(volatile unsigned int *)(TTY_BASE+0) = c;
   return 0;
}
