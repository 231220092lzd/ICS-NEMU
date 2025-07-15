#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inl(VGACTL_ADDR)>>16, .height = inl(VGACTL_ADDR)&0xffff,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {//绘制一个矩形
  if(!ctl->sync&&((ctl->w==0)||ctl->h==0)){
    return;
  }
  uint32_t *newfb=(uint32_t*)FB_ADDR;
  uint32_t *mypix=(uint32_t*)ctl->pixels;
  for(int i=0;i<ctl->h;i++){
    for(int j=0;j<ctl->w;j++){
      newfb[(i+ctl->y)*(inl(VGACTL_ADDR)>>16)+j+ctl->x]=mypix[i*ctl->w+j];
    }
  }
  if (ctl->sync==1) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
