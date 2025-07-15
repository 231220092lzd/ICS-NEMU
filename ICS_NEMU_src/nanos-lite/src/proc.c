#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);
uintptr_t loader(PCB *pcb, const char *filename);
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void protect(AddrSpace *as);
void map(AddrSpace *as, void *va, void *pa, int prot);
void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    //Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    //printf("%s ",(char*)arg);
    j ++;
    yield();
  }
}

void context_kload(PCB* kstack, void (*entry)(void *), void* arg){
  kstack->cp=kcontext((Area){kstack->stack,kstack->stack+STACK_SIZE},entry,arg);
}

void context_uload(PCB* ustack,const char* filename,char* const argv[],char* const envp[]){
  envp=NULL;
  protect(&ustack->as);//虚拟内存进程新增加
  uintptr_t stack_top=(uintptr_t)new_page(8);//申请新的空间 
  //进行用户栈的映射  //考虑对齐？ 应该不需要，这个都是对齐的
  void* va_ptr=((void*)ustack->as.area.end)-4096*8;
  void* pa_ptr=(void*)stack_top-8*4096;
  for(int i=0;i<8;i++){
   //printf("%d %d\n",(va_ptr+i*4096),(pa_ptr+i*4096));
    map(&ustack->as,va_ptr+i*4096,pa_ptr+i*4096,0);
  }

  //保存参数
  int argc=0,envc=0;
  char *pointer=(char*)stack_top;
  for(int i=0;argv!=NULL&&argv[i]!=NULL;i++){
    argc++;
  }
  for(int i=0;envp!=NULL&&envp[i]!=NULL;i++){
    envc++;
  }
  for(int i=0;i<argc;i++){
    pointer-=strlen(argv[i])+1;//+1是有结束符
    strcpy(pointer,argv[i]);
  }
  for(int i=0;i<envc;i++){
    pointer-=strlen(envp[i])+1;
    strcpy(pointer,envp[i]);
  }
  uintptr_t* cur_p=(uintptr_t*)(pointer);
  cur_p-=envc+argc+4;
  char* position=(char*)stack_top;
  uintptr_t final_position=(uintptr_t)cur_p;
  *(cur_p++)=argc;
  for(int i=0;i<argc;i++){
    position-=strlen(argv[i])+1;
    *(cur_p++)=(uintptr_t)position;
  }

  *cur_p=0;//留出NULL
  cur_p++;

  for(int i=0;i<envc;i++){
    position-=strlen(envp[i])+1;
    *(cur_p++)=(uintptr_t)position;
  }
  *cur_p=0;
  //参数记录完毕


  ustack->cp=ucontext(&ustack->as,(Area){ustack->stack,ustack->stack+STACK_SIZE},(void*)loader(ustack,filename));
  ustack->cp->GPRx=final_position;
}


void init_proc() {
  context_kload(&pcb[0],hello_fun,"Fuck");
  //context_kload(&pcb[1],hello_fun,"You");
  //char a[15]="--skip";
  //char* arr[2]={a,NULL};                        //BUG:1.在参数为NULL的时候pal会有问题 2.nterm加载不了bird/pal 3在解析命令的时候出错,传进context_uload的时候莫名其妙出问题
  context_uload(&pcb[1],"/bin/nterm",NULL,NULL);        //solution:1.参数为NULL的时候argv不为NULL，argv[0]==NULL
  switch_boot_pcb();                                   //solution:2.加载pal的时候环境变量长度居然是48413，解决暂时将其设置为NULL,而且不能刷新页面？
  Log("Initializing processes...");                     //solution:3. 应该先保存参数再申请新的空间
  //naive_uload(NULL,"/bin/nterm");
  // load program here

}

Context* schedule(Context *prev) {
  current->cp=prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  /*if(current==&pcb[1]){
    printf("0->1\n");
  }else{
    printf("1->0\n");
  }*/
  return current->cp;
}

int sys_execve(const char *fname, char *const argv[], char *const envp[]){
  context_uload(current,fname,argv,envp);
  switch_boot_pcb();
  yield();
  return -1;
}

