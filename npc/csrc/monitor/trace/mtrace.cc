/*
 * mtrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 23:55:10
 * @modified: 2025-04-12 23:58:08
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "monitor.h"

void mtrace_init() {}

void mtrace_read(paddr_t addr, word_t data) {
    LogPrintf("@r " FMT_ADDR " -> " FMT_WORD "\n", addr, data);
}

void mtrace_write(paddr_t addr, word_t data, uint8_t wmask) {
    LogPrintf("@w " FMT_ADDR " <- " FMT_WORD " <0x%x>\n", addr, data, wmask);
}