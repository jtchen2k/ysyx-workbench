/*
 * memory.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:04:22
 * @modified: 2025-04-23 20:45:17
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

void reginfo_init();
void reg_display();
void reg_name(int i, char *name);

/// register read via index or name (x0, x1 / zero, ra)
#define PC 32
#define IFPC 33
word_t R(int i);
word_t R(char *name, bool *success);
word_t R(char *name);

word_t paddr_read(paddr_t addr, int len);
void   paddr_write(paddr_t addr, word_t data, uint8_t wmask);

/// for debugging purpose in difftest.
word_t _pmem_read_word_silent(paddr_t addr);

inline word_t host_read(void *addr, int len) {
    switch (len) {
    case 1:
        return *(uint8_t *)addr;
    case 2:
        return *(uint16_t *)addr;
    case 4:
        return *(uint32_t *)addr;
    default:
        Panic("unsupported host read length: %d", len);
    }
}

inline void host_write(void *addr, word_t data, uint8_t wmask) {
    uint8_t *base = (uint8_t *)addr;
    for (int i = 0; i < 4; i++) {
        if (wmask & (1 << i)) {
            *(uint8_t *)(base + i) = (data >> (i * 8)) & 0xff;
        }
    }
}

long pmem_init();
long pmem_init(FILE *fp);

/// convert the host address (npc simulator) to the guest address (core) as
/// uint32_t
paddr_t host_to_guest(uint8_t *haddr);

/// convert the guest address (core) to the host address (npc simulator) as
/// uint8_t*, as an index of pmem
uint8_t *guest_to_host(paddr_t paddr);

#endif /* __INCLUDE_MEMORY__ */
