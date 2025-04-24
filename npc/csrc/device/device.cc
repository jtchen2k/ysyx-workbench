/*
 * device.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 16:06:52
 * @modified: 2025-04-24 16:04:33
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "SDL_events.h"
#include "common.h"
#include "config.h"
#include "core.h"
#include "device/map.h"
#include "mem.h"
#include "utils.h"
#include <SDL2/SDL.h>

mmio_map_t *iomaps;
uint8_t    *iospace;

static uint8_t *iospace_free;
static int      iomap_cnt = 0;

void init_serial();
void init_timer();
void init_keyboard();
void init_gpu();

void send_key(SDL_Scancode, bool);

static void init_map() {
    iospace = (uint8_t *)malloc(IO_SPACE_SIZE);
    Assert(iospace, "mmio space allocation failed");
    iomaps = (mmio_map_t *)malloc(NR_MMIO_MAPS * sizeof(mmio_map_t));
    Assert(iomaps, "mmio map allocation failed");
    iospace_free = iospace;
}

/// add a mmio map to the iospace. the iospace is a linear array of bytes.
mmio_map_t *add_mmio_map(const char *name, paddr_t paddr_base, int len, io_callback_t callback) {
    Assert(iomap_cnt < NR_MMIO_MAPS, "mmio map overflow");
    paddr_t left = paddr_base, right = paddr_base + len - 1;
    if (in_pmem(left) || in_pmem(right)) {
        Panic("mmio map: " FMT_ADDR " is in pmem", paddr_base);
    }
    for (int i = 0; i < iomap_cnt; i++) {
        if (left <= iomaps[i].high && right >= iomaps[i].low) {
            Panic("mmio map: " FMT_ADDR " overlaps with %s " FMT_ADDR, paddr_base, iomaps[i].name,
                  iomaps[i].low);
        }
    }
    auto map = &iomaps[iomap_cnt++];
    strncpy(map->name, name, 16);
    map->low = paddr_base;
    map->high = paddr_base + len;
    map->space = iospace_free;
    map->callback = callback;
    iospace_free += len;
    return map;
}

mmio_map_t *get_mmio_map(paddr_t paddr) {
    for (int i = 0; i < iomap_cnt; i++) {
        if (paddr >= iomaps[i].low && paddr < iomaps[i].high) {
            return &iomaps[i];
        }
    }
    return nullptr;
}

void device_update() {
    static uint64_t last_update = 0;
    uint64_t        now = get_time();
    if (now - last_update < 1000000 / TIMER_HZ)
        return;
    last_update = now;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            g_core_context->state = CORE_STATE_TERM;
            break;
#ifdef CONFIG_HAS_KEYBOARD
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            send_key(e.key.keysym.scancode, e.type == SDL_KEYDOWN);
            break;
#endif
        default:
            break;
        }
    }
}

void device_init() {
    init_map();

    IFDEF(CONFIG_HAS_SERIAL, init_serial());
    IFDEF(CONFIG_HAS_TIMER, init_timer());
    IFDEF(CONFIG_HAS_KEYBOARD, init_keyboard());
    IFDEF(CONFIG_HAS_VGA, init_gpu());
}


