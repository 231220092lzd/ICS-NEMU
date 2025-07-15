#include <stdint.h>
#include <stdio.h>
#include <time.h>
int *sieve(int n);
int main() {
  int start=clock();
  int *primes = sieve(9999998);
  for (int *cur = primes, i = 0; *cur; cur++, i++) {
    printf("%8d", *cur);
    if (i % 8 == 7 || !*(cur + 1)) printf("\n");
  }
  int end=clock();
  int time=(end-start);
  printf("%d",time);
}