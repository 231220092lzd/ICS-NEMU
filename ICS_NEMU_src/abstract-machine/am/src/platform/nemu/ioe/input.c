#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int key=inl(KBD_ADDR);
  if(key&KEYDOWN_MASK){
    kbd->keydown=true;
  }else{
    kbd->keydown=false;
  }
  kbd->keycode = key&~KEYDOWN_MASK;
}
