/*
 * memory.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:04:22
 * @modified: 2025-04-07 15:32:01
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_MEMORY__
#define __INCLUDE_MEMORY__

#define PMEM_LEFT ((paddr_t)CONFIG_MBASE)
#define PMEM_RIGHT ((paddr_t)CONFIG_MBASE + (paddr_t)CONFIG_MSIZE)
#define PMEM_SIZE ((paddr_t)CONFIG_MSIZE)
#define RESET_VECTOR (PMEM_LEFT + CONFIG_PC_RESET_OFFSET)

#include "common.h"
#include "core.h"
#include "macro.h"

static inline bool in_pmem(paddr_t addr) { return (addr >= PMEM_LEFT) && (addr < PMEM_RIGHT); }

static word_t *regs[] = {
    &g_core->io_regs_0,  &g_core->io_regs_1,  &g_core->io_regs_2,  &g_core->io_regs_3,  &g_core->io_regs_4,
    &g_core->io_regs_5,  &g_core->io_regs_6,  &g_core->io_regs_7,  &g_core->io_regs_8,  &g_core->io_regs_9,
    &g_core->io_regs_10, &g_core->io_regs_11, &g_core->io_regs_12, &g_core->io_regs_13, &g_core->io_regs_14,
    &g_core->io_regs_15, &g_core->io_regs_16, &g_core->io_regs_17, &g_core->io_regs_18, &g_core->io_regs_19,
    &g_core->io_regs_20, &g_core->io_regs_21, &g_core->io_regs_22, &g_core->io_regs_23, &g_core->io_regs_24,
    &g_core->io_regs_25, &g_core->io_regs_26, &g_core->io_regs_27, &g_core->io_regs_28, &g_core->io_regs_29,
    &g_core->io_regs_30, &g_core->io_regs_31,
};

#define R(i) (*regs[i])

void print_register();

word_t pmem_read(paddr_t addr, int len);

void pmem_init();
void pmem_init(FILE *fp);

#endif /* __INCLUDE_MEMORY__ */
