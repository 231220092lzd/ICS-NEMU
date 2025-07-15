#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  asm("add %%rax,%%rbx"
      :"=b"(b)
      :"a"(a), "b"(b));
  return b;
}

int asm_popcnt(uint64_t x) {
  int s=0;      
  asm(
      "mov $0x0, %%rcx\n\t"          //i放在ecx,初值为0
      "circle:\n\t" 
      "mov %%rax,%%rdx\n\t"
      "shr %%cl,%%rdx\n\t"//x>>i
      "and $0x1,%%rdx\n\t"      //x&1
      "cmp $0x1,%%rdx\n\t"        //if(==1)
      "je equal\n\t"
      "jmp judge\n\t"
    "equal:\n\t"
      "add $0x1,%%rbx\n\t"
    "judge:\n\t"
      "add $0x1,%%rcx\n\t"
      "cmp $0x40,%%rcx\n\t"
      "jl circle\n\t"
      :"=b"(s)
      :"a"(x),"b"(s)
      :"rcx","rdx");
  return s;
}
  /*for (int i = 0; i < 64; i++) {
    if ((x >> i) & 1) s++;
  }*/   

void *asm_memcpy(void *dest, const void *src, size_t n) {
  //return memcpy(dest, src, n);
  asm("mov $0x0,%%rdx\n\t"   //计数器rdx
     "mem_circle:\n\t"
       "cmp %%rcx,%%rdx\n\t"
       "je end\n\t"
       "movb (%%rbx),%%sil\n\t"
       "movb %%sil,(%%rax)\n\t"
       "add $0x1,%%rax\n\t"
       "add $0x1,%%rbx\n\t"
       "add $0x1,%%rdx\n\t"
       "jmp mem_circle\n\t"
     "end:\n\t"  
       :
       :"a"(dest),"b"(src),"c"(n)   //dest在rax,src在rbx,n在rcx
       :"sil","rdx","memory");
  return dest;
}

int asm_setjmp(asm_jmp_buf env) {
  //return setjmp(env);
  int final;
  asm(
      //"mov %%rbx,(%%rax)\n\t"  //rbx后面用到就不保存了
      "mov %%rcx,8(%%rax)\n\t"    //buf[1]=rcx
      "mov %%rdx,16(%%rax)\n\t"   //buf[2]=rdx
      "mov %%rsi,24(%%rax)\n\t"   //buf[3]=rsi
      "mov %%rdi,32(%%rax)\n\t"   //buf[4]=rdi
      "mov %%rbp,40(%%rax)\n\t"    //buf[5]=rbp
      "lea 8(%%rsp),%%rbx\n\t"//buf[6]=rsp+8  //buf[7]=*rsp
      "mov %%rbx,48(%%rax)\n\t"
      "mov (%%rsp),%%rbx\n\t"
      "mov %%rbx,56(%%rax)\n\t"
      "mov $0x0,%%rax\n\t"
      "ret\n\t"
      :"=a"(final)
      :"a"(env)
      :);
  return final;
}

void asm_longjmp(asm_jmp_buf env, int val) {
  //longjmp(env, val);
  asm(
      "cmp $0x0,%%rax\n\t"
      "jne recover\n\t"
      "add $0x1,%%rax\n\t"
    "recover:\n\t"
      "mov 8(%%rbx),%%rcx\n\t"
      "mov 16(%%rbx),%%rdx\n\t"
      "mov 24(%%rbx),%%rsi\n\t"
      "mov 32(%%rbx),%%rdi\n\t"
      "mov 40(%%rbx),%%rbp\n\t"
      "mov 48(%%rbx),%%rsp\n\t"
      "jmp *56(%%rbx)\n\t"
      :
      :"a"(val),"b"(env)
      :);
}
