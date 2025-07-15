#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define N 10000000
static unsigned int is_prime[312500];  //0代表是素数
static int  primes[N];
int *sieve(int n) {
  assert(n + 1 < N);
  for (long long i = 3; i <= n; i+=2) {
    for (long long j = i*i; j <= n; j += (i<<1)) {
      is_prime[j>>5]|=(1<<((j&31)-1));
    }
  }
  int *p = primes;
  *p++=2;
  for (int i = 3; i <= n; i+=2)
    if (!(is_prime[i>>5]>>((i&31)-1)&1)) {
      *p++ = i;
    }
  *p = 0;
  return primes;
}


