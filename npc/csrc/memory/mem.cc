/*
 * mem.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:18:39
 * @modified: 2025-02-07 22:45:10
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "mem.h"
#include "config.h"
#include "utils.h"
#include <cstring>

word_t pmem_read(paddr_t addr, int len) {
    if (!in_pmem(addr)) {
        Panic("illegal memory access: " FMT_ADDR, addr);
    }
    uint8_t* base = pmem + addr - CONFIG_MBASE;
    switch (len) {
        case 1:
            return *(uint8_t*)base;
        case 2:
            return *(uint16_t*)base;
        case 4:
            return *(uint32_t*)base;
        default:
            Panic("unsupported memory access length: %d", len);
    }
}

static const uint32_t img_addi[] = {
    0x00c00093, // addi x1, x0, 12
    0x01000113, // addi x2, x0, 16
    0x01808193, // addi x3, x1, 24
    0x00010213, // addi x4, x2, 0   (expect: x1 = 12, x2 = 16, x3 = 36, x4 = 16 )
    0x00100073, // ebreak
};

void pmem_init() {
    memset(pmem, 0, sizeof(pmem));
    memcpy(pmem, img_addi, sizeof(img_addi));
    LogInfo("physical memory initialized.");
}