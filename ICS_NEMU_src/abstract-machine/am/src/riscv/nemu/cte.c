#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0:ev.event = EVENT_SYSCALL;break;//sys_exit
      case -1:case 1:ev.event = EVENT_YIELD; break;//event=1
      case  2:ev.event = EVENT_SYSCALL;break;//sys_open
      case  3:ev.event = EVENT_SYSCALL;break;//sys_read
      case  4:ev.event = EVENT_SYSCALL;break; //sys_write
      case  7:ev.event = EVENT_SYSCALL;break;//sys_close
      case  8:ev.event = EVENT_SYSCALL;break;//sys_flseek
      case  9:ev.event = EVENT_SYSCALL;break;//sys_sbrk
      case  13:ev.event = EVENT_SYSCALL;break;//sys_execve
      case  19:ev.event = EVENT_SYSCALL;break;//timer 
      case  0x80000007:ev.event=EVENT_IRQ_TIMER; break;//时间中断？
      default: printf("WRONG MCAUSE:%d\n",c->mcause);ev.event = EVENT_ERROR; break;
    }
    c = user_handler(ev, c);
    if(c==NULL){
      printf("null context\n");
    }
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile(
    "csrw mtvec, %0\n"
   : 
   : "r"(__am_asm_trap));
  // register event handler
  user_handler = handler;
  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  void* newcontext=kstack.end-sizeof(Context);
  Context* target=(Context*)newcontext;
  target->mepc=(uintptr_t)entry;
  target->gpr[10]=(uintptr_t)arg;
  target->mstatus|=0x00000080;
  target->pdir=NULL;
  return target;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
