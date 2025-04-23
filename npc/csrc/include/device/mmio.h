/*
 * mmio.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-23 15:53:38
 * @modified: 2025-04-23 16:19:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __DEVICE_MMIO__
#define __DEVICE_MMIO__

#include "common.h"

word_t mmio_read(paddr_t paddr, int len);
void   mmio_write(paddr_t paddr, int len, word_t data);

#endif // __DEVICE_MMIO__
