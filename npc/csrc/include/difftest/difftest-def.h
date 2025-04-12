/*
 * difftest-def.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 17:03:32
 * @modified: 2025-04-12 17:20:18
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_DIFFTEST_DEF__
#define __INCLUDE_DIFFTEST_DEF__

#include "common.h"

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

using ref_difftest_memcpy_t = void (*)(paddr_t addr, void *buf, size_t n, bool direction);
using ref_difftest_regcpy_t = void (*)(void *dut, bool direction);
using ref_difftest_exec_t = void (*)(uint64_t n);
using ref_difftest_raise_intr_t = void (*)(word_t NO);
using ref_difftest_init_t = void (*)(int port);

struct diffcontext_t {
    word_t gpr[32];
    word_t pc;
};

#endif // __INCLUDE_DIFFTEST_DEF__
