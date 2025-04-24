/*
 * timer.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 21:37:38
 * @modified: 2025-04-24 15:17:32
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "device/map.h"
#include <cstdint>
#include <ctime>
#include <sys/time.h>

static mmio_map_t *rtc_map;
static uint64_t    boot_time = 0;

/// get the current time. If it is the first call, set the boot time.
uint64_t get_time() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t us = tv.tv_usec + tv.tv_sec * 1000000;
    if (boot_time == 0)
        boot_time = us;
    return us - boot_time;
}

void rtc_io_handler(uint32_t offset, int len, bool is_write) {
    Assert(len == 4, "serial io handler: invalid length %d", len);
    Assert(offset == 0 || offset == 4, "serial io handler: invalid offset %d", offset);
    if (!is_write && offset == 0) {
        uint64_t us = get_time();
        uint32_t high = (us >> 32) & 0xffffffff;
        uint32_t low = us & 0xffffffff;
        ((uint32_t *)rtc_map->space)[1] = high;
        ((uint32_t *)rtc_map->space)[0] = low;
    }
}

void init_timer() {
    get_time();
    rtc_map = add_mmio_map("timer", CONFIG_RTC_MMIO, 8, rtc_io_handler);
    LogInfo("timer device initialized");
}