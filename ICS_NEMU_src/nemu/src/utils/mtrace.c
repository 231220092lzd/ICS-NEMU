#include <common.h>
void mtrace_read(paddr_t curaddr,int length){
    printf("read addr:%d %d",curaddr,length);
}

void mtrace_write(paddr_t curaddr,int length,int towrite){
    printf("write addr:%d length:%d write:%d",curaddr,length,towrite);
}