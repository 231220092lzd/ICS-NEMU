#include <proc.h>
#include <elf.h>
#include "fs.h"
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;
void init_fs();
int fs_open(const char* pathname, int flags, int mode);
size_t fs_read(int fd,void* buf,size_t len);
size_t fs_write(int fd,const void* buf,size_t len);
size_t fs_lseek(int fd,size_t offset, int whence);
int fs_close(int fd);
void map(AddrSpace *as, void *va, void *pa, int prot);
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
uintptr_t loader(PCB *pcb, const char *filename){
  init_fs();
  pcb->max_brk=0;
  int file=fs_open(filename,0,0);
  Elf_Ehdr myehpr;
  fs_read(file,&myehpr,sizeof(Elf_Ehdr));
  //ramdisk_read(&myehpr,0,sizeof(Elf_Ehdr));
  for(int i=0;i<myehpr.e_phnum;i++){
    fs_lseek(file,myehpr.e_phoff+i*myehpr.e_phentsize,SEEK_SET);
    Elf_Phdr myphdr;
    fs_read(file,&myphdr,myehpr.e_phentsize);
    if(myphdr.p_type!=PT_LOAD){
      continue;
    }
    fs_lseek(file,myphdr.p_offset,SEEK_SET);
    //PA4虚拟内存后，下面要修改为加载到虚拟内存中
    
    //fs_read(file,(void*)myphdr.p_vaddr,myphdr.p_memsz);
    //memset((void*)(myphdr.p_vaddr+myphdr.p_filesz),0,myphdr.p_memsz-myphdr.p_filesz);
    
    uint32_t start=ROUNDDOWN((uintptr_t)myphdr.p_vaddr,4096);
    uint32_t end=ROUNDDOWN((uintptr_t)myphdr.p_vaddr+myphdr.p_memsz,4096);//
    uint32_t page_number=((end-start)/4096)+1;//计算要几个新的物理页面
    if(((myphdr.p_vaddr+myphdr.p_memsz)&0xFFF)==0){
      page_number--;
    }
    uint32_t align_set=(myphdr.p_vaddr&0xFFF);//第一个页面偏移这么多字节
    void* pa_ptr=new_page(page_number)-page_number*4096;// 申请新的物理页,其中pa_ptr是起始位置
    for(int j=0;j<page_number;j++){
      map(&pcb->as,(void*)start+j*4096,pa_ptr+j*4096,1);
    }
    fs_read(file,pa_ptr+align_set,myphdr.p_memsz);
    memset((void*)pa_ptr+align_set+myphdr.p_filesz,0,myphdr.p_memsz-myphdr.p_filesz);
    if(pcb->max_brk<start+page_number*4096-1){
      pcb->max_brk=start+page_number*4096-1;
    }

    //assert(((pcb->max_brk+1)&0xFFF)==0);
    //加载结束
  }
  return myehpr.e_entry;
}


void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

