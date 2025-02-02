/*
 * memory.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:04:22
 * @modified: 2025-02-01 20:42:00
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_MEMORY__
#define __INCLUDE_MEMORY__

#include "common.h"
#include "macro.h"

static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN= {};

static inline bool in_pmem(paddr_t addr) { return (addr - CONFIG_MBASE) < CONFIG_MSIZE; }

word_t pmem_read(paddr_t addr);

void pmem_init();

#endif /* __INCLUDE_MEMORY__ */
