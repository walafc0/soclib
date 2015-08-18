#define N 9

#include "system.h"

int fibo(int i)
{ if(i==0) return 1;
  if(i==1) return 1;
  return fibo(i-1)+fibo(i-2);
}

int main()
{ int f = fibo(N);
  print_str("fibo[9] computed as return value should be equal to 55.\n");
  return f;
}
