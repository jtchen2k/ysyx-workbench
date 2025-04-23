/*
 * mem.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:18:39
 * @modified: 2025-04-23 21:12:43
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "mem.h"
#include "config.h"
#include "core.h"
#include "device/mmio.h"
#include "monitor.h"
#include "utils.h"
#include <cstring>

static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
static long    img_size = 0;

static word_t pmem_read(paddr_t addr, int len) {
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
    IFDEF(CONFIG_MTRACE, mtrace_read(addr, data, len));
    return data;
}

static void pmem_write(paddr_t addr, word_t data, uint8_t wmask) {
    uint8_t *base = guest_to_host(addr);
    for (int i = 0; i < 4; i++) {
        if (wmask & (1 << i)) {
            *(uint8_t *)(base + i) = (data >> (i * 8)) & 0xff;
        }
    }
    IFDEF(CONFIG_MTRACE, mtrace_write(addr, data, wmask));
}

word_t _pmem_read_word_silent(paddr_t addr) {
    Assert(in_pmem(addr), "attempted illegal memory access: " FMT_ADDR, addr);
    return *(uint32_t *)guest_to_host(addr);
}

word_t paddr_read(paddr_t addr, int len) {
    if (in_pmem(addr))
        return pmem_read(addr, len);
#ifdef CONFIG_DEVICE
    return mmio_read(addr, len);
#endif
    LogFatal("attempted illegal paddr access: " FMT_ADDR, addr);
    return 0;
}

void paddr_write(paddr_t addr, word_t data, uint8_t wmask) {
    if (in_pmem(addr)) {
        pmem_write(addr, data, wmask);
        return;
    }
    IFDEF(CONFIG_DEVICE, mmio_write(addr, 4, data); return;);
    LogFatal("attempted illegal paddr access: " FMT_ADDR, addr);
    return;
}

static const uint32_t img_addi[] = {
    0x00c00093, // addi x1, x0, 12 / addi ra, zero, 0xc
    0x01000113, // addi x2, x0, 16 / addi sp, zero, 0x10
    0x01808193, // addi x3, x1, 24
    0x00010213, // addi x4, x2, 0 (expect: x1 = 12, x2 = 16, x3 = 36, x4 = 16 )
    0x00100073, // ebreak
};

static const uint32_t img_sw[] = {
    0xfffff0b7, // lui x1, 0xfffff
    0xfff00093, // addi x1, zero, 0xfff, now all bits of x1 are 1
    0x80000337, // lui x6, 0x80000, now x6 = 0x80000000, base pmem address
    0x08132023, // sw x1, 0x80(x6), now pmem[0x80] = 0xffffffff
    0x08131223, // sh x1, 0x84(x6), now pmem[0x84] = 0x0000ffff
    0x08130423, // sb x1, 0x88(x6), now pmem[0x88] = 0x000000ff
    0x08034283, // lbu x5, 0x80(x6), now x5 = 0x000000ff
    0x08035203, // lhu x4, 0x80(x6), now x4 = 0x0000ffff
    0x00100073, // ebreak
};

static const uint32_t *default_image = img_sw;
static const size_t    default_image_size = sizeof(img_sw);
// test for this basic addi image:
// Assert(R(1) == 12, "x1 = %d", R(1));
// Assert(R(2) == 16, "x2 = %d", R(2));
// Assert(R(3) == 36, "x3 = %d", R(3));
// Assert(R(4) == 16, "x4 = %d", R(4));

long pmem_init() {
    memset(pmem, 0, sizeof(pmem));
    memcpy(pmem, default_image, default_image_size);
    img_size = default_image_size;
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