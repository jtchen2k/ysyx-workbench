/*
 * mem.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:18:39
 * @modified: 2025-04-13 16:14:43
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "mem.h"
#include "config.h"
#include "core.h"
#include "monitor.h"
#include "utils.h"
#include <cstring>

static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
static long    img_size = 0;

word_t pmem_read(paddr_t addr, int len) {
    if (!in_pmem(addr)) {
        LogFatal("attempted illegal memory access: " FMT_ADDR, addr);
        return 0;
    }
    uint8_t *base = guest_to_host(addr);
    word_t   data = 0;
    switch (len) {
    case 1:
        data = *(uint8_t *)base;
        break;
    case 2:
        data = *(uint16_t *)base;
        break;
    case 4:
        data = *(uint32_t *)base;
        break;
    default:
        Panic("unsupported memory access length: %d", len);
    }
#ifdef CONFIG_MTRACE
    mtrace_read(addr, data, len);
#endif
    return data;
}

void pmem_write(paddr_t addr, word_t data, uint8_t wmask) {
    if (!in_pmem(addr)) {
        LogFatal("attempted illegal memory access: " FMT_ADDR, addr);
        return;
    }
    uint8_t *base = guest_to_host(addr);
    for (int i = 0; i < 4; i++) {
        base[i] = (base[i] & ~wmask) | (data >> (i * 8) & wmask);
    }
#ifdef CONFIG_MTRACE
    mtrace_write(addr, data, wmask);
#endif
}

static const uint32_t img_addi[] = {
    0x00c00093, // addi x1, x0, 12
    0x01000113, // addi x2, x0, 16
    0x01808193, // addi x3, x1, 24
    0x00010213, // addi x4, x2, 0 (expect: x1 = 12, x2 = 16, x3 = 36, x4 = 16 )
    0x00100073, // ebreak
};

// test for this basic addi image:
// Assert(R(1) == 12, "x1 = %d", R(1));
// Assert(R(2) == 16, "x2 = %d", R(2));
// Assert(R(3) == 36, "x3 = %d", R(3));
// Assert(R(4) == 16, "x4 = %d", R(4));

long pmem_init() {
    memset(pmem, 0, sizeof(pmem));
    memcpy(pmem, img_addi, sizeof(img_addi));
    img_size = sizeof(img_addi);
    LogDebug("physical memory initialized.");
    return img_size;
}

long pmem_init(FILE *fp) {
    memset(pmem, 0, sizeof(pmem));
    fseek(fp, 0, SEEK_END);
    img_size = ftell(fp);

    LogInfo("image loaded. size: %lu", img_size);
    fseek(fp, 0, SEEK_SET);
    int ret = fread(pmem, img_size, 1, fp);

    Assert(ret == 1, "failed to read image file: %s", strerror(errno));
    LogDebug("physical memory initialized.");
    return img_size;
}

paddr_t  host_to_guest(uint8_t *haddr) { return CONFIG_MBASE + (haddr - pmem); }
uint8_t *guest_to_host(paddr_t paddr) { return pmem + (paddr - CONFIG_MBASE); }