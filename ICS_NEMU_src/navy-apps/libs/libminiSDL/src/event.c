#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <stdlib.h>
#define keyname(k) #k,
uint8_t pal_key[83]={0};
static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[20];
  if(NDL_PollEvent(buf,sizeof(buf))==0){
    return 0;
  }
  int keycode;
  char type[3];
  char keynumber[15];
  sscanf(buf,"%s %s %d",type,keynumber,&keycode);
  if(strcmp(type,"ku")==0){
    ev->type=SDL_KEYUP;
    pal_key[keycode]=0;
  }else{
    ev->type=SDL_KEYDOWN;
    pal_key[keycode]=1;
  }
    ev->key.keysym.sym=keycode;
    return 1;
}


int SDL_WaitEvent(SDL_Event *event) {
  char buf[20];
  while(NDL_PollEvent(buf,sizeof(buf))==0){}
  int keycode;
  char type[3];
  char keynumber[15];
  sscanf(buf,"%s %s %d",type,keynumber,&keycode);
  if(strcmp(type,"ku")==0){
    event->type=SDL_KEYUP;
    pal_key[keycode]=0;
  }else{
    event->type=SDL_KEYDOWN;
    pal_key[keycode]=1;
  }
    event->key.keysym.sym=keycode;
    return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if(numkeys!=NULL){
    *numkeys=83;//这里numkeys要求返回键盘数组的长度
  }
  return pal_key;
  //用来返回一个数组，表示键盘中每个按键有没有按下

}
