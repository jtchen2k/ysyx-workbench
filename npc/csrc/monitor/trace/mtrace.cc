/*
 * mtrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 23:55:10
 * @modified: 2025-04-13 14:41:38
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "monitor.h"

#ifdef CONFIG_MTRACE

#define MTRACE_PRINT_VERBOSITY 1

void mtrace_init() {}

static paddr_t last_r = 0x0;
static paddr_t last_w = 0x0;

void mtrace_read(paddr_t addr, word_t data, int len) {
    if (addr == last_r) {
        return;
    }
    last_r = addr;
    char buf[64];
    snprintf(buf, sizeof(buf), "@r [" FMT_ADDR "] -> " FMT_WORD " <%d>\n", addr, data, len);
    LogPrintf("%s", buf);
    if (g_args->verbosity >= MTRACE_PRINT_VERBOSITY) {
        printf("%s", buf);
    }
}

void mtrace_write(paddr_t addr, word_t data, uint8_t wmask) {
    if (addr == last_w) {
        return;
    }
    last_w = addr;
    char buf[64];
    snprintf(buf, sizeof(buf), "@w " FMT_ADDR " <- " FMT_WORD " <0x%x>\n", addr, data, wmask);
    LogPrintf("%s", buf);
    if (g_args->verbosity >= MTRACE_PRINT_VERBOSITY) {
        printf("%s", buf);
    }
}
#endif
