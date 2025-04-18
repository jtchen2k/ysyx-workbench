#include <am.h>
#include <klib.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  return;
  uint32_t  wh = inl(VGACTL_ADDR);
  volatile int       i;
  volatile int       w = (wh >> 16) & 0xffff;
  volatile int       h = wh & 0xffff;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++)
    fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t wh = inl(VGACTL_ADDR);
  volatile int      w = (wh >> 16) & 0xffff;
  volatile int      h = wh & 0xffff;
  // printf("w = %d h = %d\n", w, h);
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = w,
                           .height = h,
                           .vmemsz = w * h};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t  screenwh = inl(VGACTL_ADDR);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = (uint32_t *)(uintptr_t)ctl->pixels;
  int sw = (screenwh >> 16) & 0xffff;
  int w = ctl->w;
  int h = ctl->h;
  int x = ctl->x;
  int y = ctl->y;
  for (int j = y; j < y + h; j++) {
    for (int i = x; i < x + w; i++) {
      fb[sw * j + i] = pixels[w * (j - y) + (i - x)];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
