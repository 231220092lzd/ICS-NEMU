#include "asm.h"
#include <assert.h>
#include <stdio.h>

int main() {
  asm_jmp_buf buf;
  int r=asm_setjmp(buf);
  if (r == 0) {
   assert(asm_add(114514, 666666) == 781180);
   // printf("%d",asm_popcnt(0x123456789abcdefULL));
   assert(asm_popcnt(0x0123456789abcdefULL) == 32);
   printf("pass");
    // TODO: add more tests here.
    asm_longjmp(buf, 123);
  } else {
    assert(r == 123);
    printf("PASSED.\n");
  }
}
