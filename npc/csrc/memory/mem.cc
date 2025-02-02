/*
 * mem.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:18:39
 * @modified: 2025-02-01 21:22:42
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "mem.h"
#include "config.h"
#include "utils.h"
#include <cstring>

word_t pmem_read(paddr_t addr) {
    if (!in_pmem(addr)) {
        return pmem[addr - CONFIG_MBASE];
    } else {
        Panic("illegal memory access!");
    }
}

static const uint32_t img_addi[] = {
    0x00c00093, // addi x1, x0, 12
    0x01000113, // addi x2, x0, 16
    0x00008193, // addi x3, x1, 24
    0x00010193, // addi x4, x2, 0   (expect: x1 = 12, x2 = 16, x3 = 36, x4 = 16 )
    0x00100073, // ebreak
};

void pmem_init() {
    memset(pmem, 0, sizeof(pmem));
    memcpy(pmem, img_addi, sizeof(img_addi));
    LogInfo("physical memory initialized.");
}