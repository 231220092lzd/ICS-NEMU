#include <common.h>
#include <../../navy-apps/libs/libos/src/syscall.h>
void do_syscall(Context *c);
Context* schedule(Context *prev);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {//yield=1,syscall=2
    case 1 : c=schedule(c);break;
    case 2 : do_syscall(c);break;
    case 5 : /*printf("receive time yield\n");*/c=schedule(c);break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
