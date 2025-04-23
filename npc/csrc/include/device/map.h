/*
 * map.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 16:04:56
 * @modified: 2025-04-23 20:41:09
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __DEVICE_MAP__
#define __DEVICE_MAP__

#define IO_SPACE_SIZE 0x10000
#define NR_MMIO_MAPS 8

#include "common.h"
#include <vector>

/// callback args:
using io_callback_t = void (*)(uint32_t offset, int len, bool is_write);

struct mmio_map_t {
    char          name[16];
    paddr_t       low;      /// low guest address
    paddr_t       high;     /// high guest address
    uint8_t      *space;    /// pointer to the mmio space in the host
    io_callback_t callback; /// callback function
};

extern mmio_map_t *iomaps;
extern uint8_t    *iospace;

void device_init();

mmio_map_t* add_mmio_map(const char *name, paddr_t paddr_base, int len, io_callback_t callback);
mmio_map_t* get_mmio_map(paddr_t paddr);

#endif // __DEVICE_MAP__
