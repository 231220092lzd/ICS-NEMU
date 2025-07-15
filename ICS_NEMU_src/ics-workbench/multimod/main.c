
#include <stdio.h>
#include <inttypes.h>

uint64_t multimod(uint64_t, uint64_t, uint64_t);

void test(uint64_t a, uint64_t b, uint64_t m) {
  #define U64 "%" PRIu64
  printf(U64 " * " U64 " mod " U64 " = " U64 "\n", a, b, m, multimod(a, b, m));
}

int main() {
  test(123, 456, 789);
  test(123, 456, -1ULL);
  test(-1ULL, 2, -1ULL-2); // should be 1
  test(5,5,5);
  test(-1ULL,8,-1ULL-6);
  test(-2ULL,4,-2ULL);
  test(-2ULL,-2ULL,-3ULL);
  test(-4ULL,-1ULL,-3ULL);
  test(-6ULL,-10ULL,-4ULL);
  test(2024,1024,1);
}
