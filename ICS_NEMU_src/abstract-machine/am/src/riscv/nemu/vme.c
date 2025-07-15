#include <am.h>
#include <nemu.h>
#include <klib.h>
#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog
static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  //页目录基地址as->ptr
  //printf("in loader va_ptr:%d %d pa_ptr:%d %d\n",va,BITS((uintptr_t)va,11,0),pa,BITS((uintptr_t)pa,11,0));
  assert(BITS((uintptr_t)va,11,0)==0&&BITS((uintptr_t)pa,11,0)==0);
  uint32_t virtual_page_numberone=BITS((uintptr_t)va,21,12);//二级页号
  uint32_t virtual_page_numbertwo=BITS((uintptr_t)va,31,22);//一级页号
  uint32_t physic_page_number=BITS((uintptr_t)pa,31,12);//物理页号
  uint32_t* first_page_addr=((uint32_t*)as->ptr)+virtual_page_numbertwo;
  //考虑下一级页表空不空
  if((*first_page_addr&0x1)==0){
    uint32_t* second_page_addr=(uint32_t*)pgalloc_usr(4096);//设置有效
    *first_page_addr=(uint32_t)second_page_addr|0x1;//设置有效
    *(second_page_addr+virtual_page_numberone)=(physic_page_number<<12)|0xF;
  }else{//下一级页表不空
    uint32_t* second_page_addr=(uint32_t*)(uintptr_t)(BITS(*first_page_addr,31,12)<<12);
    *(second_page_addr+virtual_page_numberone)=(physic_page_number<<12)|0xF;
  }
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context* newcontext=(Context*)(kstack.end-sizeof(Context));
  newcontext->mepc=(uintptr_t)entry;
  newcontext->mstatus|=0x80;
  newcontext->pdir=as->ptr;
  return newcontext;
}
