#include <common.h>
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif
#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
 for(int i=0;i<len;i++,buf++){
      putch(*(char*)(buf));
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T mykey=io_read(AM_INPUT_KEYBRD);
  if(mykey.keycode==0){
    *(char*)buf='\0';
    return 0;
  }else if(mykey.keydown){
    *(char*)(buf++)='k';
    *(char*)(buf++)='d';
    *(char*)(buf++)=' ';
  }else{
    *(char*)(buf++)='k';
    *(char*)(buf++)='u';
    *(char*)(buf++)=' ';
  }
  const char* mid=keyname[mykey.keycode];
  for(int i=0;i<strlen(mid);i++){
    *(char*)(buf++)=mid[i];
  }
  *(char*)(buf++)=' ';
  sprintf((char*)buf,"%d",mykey.keycode);
  return len;
}

static int screen_w=0,screen_h=0; 
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T myscreen;
  ioe_read(AM_GPU_CONFIG,&myscreen);
  sprintf((char*)buf,"%d %d\n",myscreen.width,myscreen.height);
  screen_h=myscreen.height;
  screen_w=myscreen.width;
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {//offset=cur_y*screen_w+cur_x
  AM_GPU_FBDRAW_T fbw;
  fbw.pixels=(uint32_t*)buf;
  fbw.x=offset%screen_w;
  fbw.y=offset/screen_w;
  fbw.w=len;
  fbw.h=1;
  fbw.sync=true;
  ioe_write(AM_GPU_FBDRAW,&fbw);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
