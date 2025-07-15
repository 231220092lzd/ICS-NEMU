#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
int
open (const char *file,
        int flags, ...);
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int draw_w=0, draw_h=0;
struct timeval mytv;
int gettimeofday (struct timeval *ptimeval,void *ptimezone);
uint32_t NDL_GetTicks() {
  gettimeofday(&mytv,NULL);
  return (uint32_t)(mytv.tv_sec*1000+mytv.tv_usec/1000);
}

int NDL_PollEvent(char *buf, int len) {
  return read(3,buf,len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  int fd_dispinfo=open("/proc/dispinfo",0);
  char* screen_size=(char*)malloc(100*sizeof(char));
  read(fd_dispinfo,screen_size,100);
  sscanf(screen_size,"%d %d\n",&screen_w,&screen_h);
  if(*w>screen_w||*h>screen_h){
    printf("draw map > screen\n");
    assert(0);
  }
  if(*w==0&&*h==0){
    draw_h=screen_h;
    *w=screen_h;
    draw_w=screen_w;
    *h=screen_h;
  }else{
    draw_h=*h;
    draw_w=*w;
  }

}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd_FB=open("/dev/fb",0);
  int offset_x=(screen_w-draw_w)/2;
  int offset_y=(screen_h-draw_h)/2;
  for(int i=0,cur_y=y;i<h;i++,cur_y++){
    lseek(fd_FB,((cur_y+offset_y)*screen_w+x+offset_x),0);
    write(fd_FB,pixels+i*w,w);
  }
  /*for(int i=0,cur_y=y;i<h;i++,cur_y++){  //一次刷新一个像素实在是太慢了！！！改成一行
    for(int j=0,cur_x=x;j<w;j++,cur_x++){
      lseek(fd_FB,((cur_y+offset_y)*screen_w+cur_x+offset_x),0);
      write(fd_FB,(pixels+i*w+j),sizeof(uint32_t));
    }
  }*/
}

void NDL_OpenAudio(int freq, int channels, int samples) {
  
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
