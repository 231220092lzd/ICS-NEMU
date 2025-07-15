#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "../../libs/libndl/NDL.c"
#define TIMERTEST
#ifdef TIMERTEST
int main() {
  long sec=500; 
  while (1) {
    while(NDL_GetTicks()<sec){
      NDL_GetTicks();
    }
    printf("第%d个0.5秒\n",(int)(sec/500));
    sec +=500;
  }
  return 0;
}
#else
int main() {
  write(1, "Hello World!\n", 13);
  int i = 1;
  volatile int j = 0;
  while (1){
    j ++;
    if (j ==10000) {
      printf("Hello World from Navy-apps for the fucking %d time!\n",i++);
      j = 0;
    }
  }
  return 0;
}
#endif
