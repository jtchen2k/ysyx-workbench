/*
 * device.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 16:06:52
 * @modified: 2025-04-23 21:01:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"
#include "device/map.h"
#include "mem.h"
#include "utils.h"
#include <cstring>
#include <vector>

mmio_map_t *iomaps;
uint8_t    *iospace;

uint8_t *iospace_free;
int      iomap_cnt = 0;

void init_serial();
// void init_timer();
// void init_keyboard();
// void init_gpu();

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
            Panic("mmio map: " FMT_ADDR " overlaps with %s " FMT_ADDR, paddr_base, iomaps[i].name, iomaps[i].low);
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

void device_init() {
    init_map();

    IFDEF(CONFIG_HAS_SERIAL, init_serial());
    // IFDEF(CONFIG_HAS_TIMER, init_timer());
    // IFDEF(CONFIG_HAS_KEYBOARD, init_keyboard());
    // IFDEF(CONFIG_HAS_VGA, init_gpu());
}
