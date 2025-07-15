#include <common.h>
#ifdef DTRACE
void dtrace_visit(IOMap* device,paddr_t addr){
    printf("visit %s at addr:%x",device->name,addr);
}
void dtrace_write(IOMap* device,paddr_t addr,int towrite){
    printf("write %d to %s at addr:%x",towrite,device->name,addr);
}
#endif