/*
 * mmio.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 17:05:03
 * @modified: 2025-04-24 15:14:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "device/mmio.h"
#include "device/map.h"
#include "mem.h"

word_t mmio_read(paddr_t paddr, int len) {
    auto map = get_mmio_map(paddr);
    Assert(map, "mmio read: no map found at " FMT_ADDR, paddr);
    if (map->callback) {
        map->callback(paddr - map->low, len, false);
    }
    word_t ret = host_read(map->space + (paddr - map->low), len);
    return ret;
}

void mmio_write(paddr_t paddr, int len, word_t data) {
    auto map = get_mmio_map(paddr);
    Assert(map, "mmio write: no map found at " FMT_ADDR, paddr);
    host_write(map->space + (paddr - map->low), data, 0xf);
    if (map->callback) {
        map->callback(paddr - map->low, len, true);
    }
}
