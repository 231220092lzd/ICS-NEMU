#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  int file_fd=open(filename,0);
  uint32_t file_size=(uint32_t)lseek(file_fd,0,SEEK_END);
  void* buf=(void*)malloc(file_size);
  lseek(file_fd,0,SEEK_SET);
  read(file_fd,buf,file_size);
  SDL_Surface* LOAD=STBIMG_LoadFromMemory((char*)buf,file_size);
  close(file_fd);
  free(buf);
  return LOAD;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
