#include <am.h>
#include <nemu.h>
#include <klib.h>

void __am_timer_init() {

}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {

  // important: need to first read the higher 4 bytes for nemu.
  uint32_t high = inl(RTC_ADDR + 4);
  uint32_t low = inl(RTC_ADDR);
  volatile uint64_t us = ((uint64_t)high << 32) | low;
  // printf("<am timer> call timer_uptime: low = %u, high = %u, us = %lu\n", low, high, us);
  uptime->us = us;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
