#include <common.h>
#include "syscall.h"
#include "../include/proc.h"
typedef long time_t;
typedef long suseconds_t;
struct timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* and microseconds */
};
struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};
int mm_brk(uintptr_t brk,uint32_t base);
int sys_execve(const char *fname, char *const argv[], char *const envp[]);
void naive_uload(PCB *pcb, const char *filename);
int fs_open(const char* pathname, int flags, int mode);
size_t fs_read(int fd,void* buf,size_t len);
size_t fs_write(int fd,const void* buf,size_t len);
size_t fs_lseek(int fd,size_t offset, int whence);
int fs_close(int fd);
int mygettimeofday(struct timeval *tv, struct timezone *tz){
  tv->tv_sec=io_read(AM_TIMER_UPTIME).us/1000000;
  tv->tv_usec=io_read(AM_TIMER_UPTIME).us%1000000;
  return 0;
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case -1:c->GPRx=0;break;//yield
    case 0:c->GPRx=0;halt(0);/*naive_uload(NULL,"/bin/nterm");*/break;//exit
    case 1: yield();break;//syscall_yield
    case 2: c->GPRx=fs_open((const char*)a[1],a[2],a[3]);break;       //sys_open
    case 3: c->GPRx=fs_read(a[1],(void*)a[2],a[3]);break;       //sys_read
    case 4: c->GPRx=fs_write(a[1],(void*)a[2],a[3]);break;//sys_write
    case 7: c->GPRx=fs_close(a[1]);break;        //sys_close
    case 8: c->GPRx=fs_lseek(a[1],a[2],a[3]);break;        //sys_lseek
    case 9: c->GPRx=mm_brk(a[1],a[2]);break;//sys_brk
    case 13:c->GPRx=-1;sys_execve((const char*)a[1],(char* const*)a[2],(char* const*)a[3]);break;////sys_exceve
    case 19:c->GPRx=mygettimeofday((struct timeval*)a[1],(struct timezone*)a[2]);break;//sys_gettimeofday
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
