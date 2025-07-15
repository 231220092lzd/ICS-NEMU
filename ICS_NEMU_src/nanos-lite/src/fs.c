#include <fs.h>
#include "proc.h"
typedef uint32_t word_t;
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR,FD_EVENT,FD_FB,FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("invalid_read,should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("invalid_write,should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read,invalid_write},//invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0,invalid_read,serial_write},//invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0,invalid_read,serial_write},//invalid_read, invalid_write},
  [FD_FB]={"/dev/fb",0,0,invalid_read,fb_write},
  [FD_EVENT]={"/dev/events",0,0,events_read,invalid_write},
  [FD_DISPINFO]={"/proc/dispinfo",0,0,dispinfo_read,invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  word_t width=io_read(AM_GPU_CONFIG).width;
  word_t height=io_read(AM_GPU_CONFIG).height;
  file_table[FD_FB].size=width*height*4;
  for(int i=3;i<sizeof(file_table)/sizeof(Finfo);i++){//初始化open_offset
    file_table[i].open_offset=0;
  }
}

int fs_open(const char* pathname, int flags, int mode){
  for(int i=3;i<sizeof(file_table)/sizeof(Finfo);i++){
    if(strcmp(pathname,file_table[i].name)==0){
      file_table[i].open_offset=0;
      return i;
    }
  }
  printf("no matching pathname when open file:%s\n",pathname);
  assert(0);
  return 0;
}

size_t fs_read(int fd,void* buf,size_t len){
  if(file_table[fd].read!=NULL){
    return file_table[fd].read(buf,fd,len);
  }
  if(file_table[fd].open_offset+len>file_table[fd].size){
    len=file_table[fd].size-file_table[fd].open_offset;
  }
  ramdisk_read(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
  file_table[fd].open_offset+=len;
  return len;
}

size_t fs_write(int fd,const void* buf,size_t len){
  if(file_table[fd].write!=NULL){
    return file_table[fd].write(buf,file_table[fd].open_offset,len);
  }
  if(file_table[fd].open_offset+len>file_table[fd].size){
    len=file_table[fd].size-file_table[fd].open_offset;
  }
  ramdisk_write(buf,file_table[fd].open_offset+file_table[fd].disk_offset,len);
  file_table[fd].open_offset+=len;
  return len;
}

size_t fs_lseek(int fd,size_t offset, int whence){
  if(fd<=2){
    //printf("wrong fd file name when lseek\n");
    //assert(0);
  }
  switch(whence){
    case SEEK_SET:
      file_table[fd].open_offset=offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset+=offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset=file_table[fd].size+offset;
  }
  if(file_table[fd].open_offset>file_table[fd].size){
    printf("out of bound when lseek,whence:%d,open_offset:%d,size:%d,offset:%d\n",whence,(int)file_table[fd].open_offset,(int)file_table[fd].size,(int)offset);
  }
  return file_table[fd].open_offset;//open_offset好像实际上是文件内部偏移量

}

int fs_close(int fd){

  return 0;
}