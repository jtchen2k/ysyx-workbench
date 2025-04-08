/*
 * memory.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:04:22
 * @modified: 2025-04-08 22:12:27
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

static inline bool in_pmem(paddr_t addr) {
    return (addr >= PMEM_LEFT) && (addr < PMEM_RIGHT);
}

void reginfo_init();
void print_registers();

/// register read via index or name (x0, x1 / zero, ra)
word_t R(int i);
word_t R(char *name, bool *success);
word_t R(char *name);

word_t pmem_read(paddr_t addr, int len);

void pmem_init();
void pmem_init(FILE *fp);

#endif /* __INCLUDE_MEMORY__ */
