#include <memory.h>
#include <proc.h>
extern PCB* current;
static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf+=nr_page*4096;
  return pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  void* new_addr=new_page(n/4096)-n;
  memset(new_addr,0,n);
  return new_addr;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
  //printf("start mm_brk\n");
  //printf("%d %d %d\n",brk,base,current->max_brk);

int mm_brk(uintptr_t brk,uint32_t base) {         //current->max_brk是当前页面最大地址
  //assert(((current->max_brk+1)&0xFFF)==0);
  uint32_t cur_brk=((current->max_brk)+1);       //cur_brk是当前所在页下一页的首地址
  if(brk>current->max_brk){//超出，新分配页面并且映射,考虑对齐
    //传入的brk是pro_break+increment
    int page_number=((ROUNDDOWN(brk,4096)-ROUNDDOWN(current->max_brk,4096)));//新需要的页面数量
    if((brk&0xFFF)==0){
      page_number--;
    }
    for(int i=0;i<page_number;i++){
      void* new_page_ptr=new_page(1)-4096;
      //printf("cur_brk:%d offset:%d new_ptr:%d offset:%d\n",cur_brk,cur_brk&0xFFF,(uint32_t)new_page_ptr,(uint32_t)new_page_ptr&0xFFF);
      map(&current->as,(void*)cur_brk,new_page_ptr,0);
      cur_brk+=4096;
      current->max_brk+=4096;
    }
    assert(((current->max_brk+1)&0xFFF)==0);
  }
  //printf("end mm_brk\n");
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
