/*
 * serial.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 16:06:03
 * @modified: 2025-04-23 21:22:26
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "device/map.h"
#include <cstdio>

mmio_map_t *serial_map;

void serial_io_handler(uint32_t offset, int len, bool is_write) {
    Assert(len == 4, "serial io handler: invalid length %d", len);
    Assert(offset == 0, "serial io handler: invalid offset %d", offset);
    // LogTrace("serial io handler: offset %d, len %d, is_write %d", offset, len, is_write);
    if (is_write) {
        putc(serial_map->space[0], stderr);
    } else {
        // serial read is ignored since we are using @(*) in DPI_MEM.scala
        return;
    }
}

void init_serial() {
    serial_map = add_mmio_map("serial", CONFIG_SERIAL_MMIO, 8, serial_io_handler);
}