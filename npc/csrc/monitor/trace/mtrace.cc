/*
 * mtrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 23:55:10
 * @modified: 2025-04-15 14:36:52
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "mem.h"
#include "monitor.h"

#ifdef CONFIG_MTRACE

#define MTRACE_PRINT_VERBOSITY 1

void mtrace_init() {}

// Used for supressing duplicate traces
static paddr_t last_r = 0x0;    // last read for data
static paddr_t last_r_if = 0x0; // last read for instruction fetch to pc
static paddr_t last_w = 0x0;    // last write

void mtrace_read(paddr_t addr, word_t data, int len) {
    if (addr == last_r || addr == last_r_if) {
        // return;
    }
    last_r = addr;
    char ifstr[8] = "";
    if (addr == R(PC)) {
        last_r_if = addr;
        strcpy(ifstr, " <i>");
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "@r [" FMT_ADDR "] -> " FMT_WORD " <%d>%s\n", addr, data, len, ifstr);
    LogPrintf("%s", buf);
    if (g_args->verbosity >= MTRACE_PRINT_VERBOSITY) {
        printf("%s", buf);
    }
}

void mtrace_write(paddr_t addr, word_t data, uint8_t wmask) {
    if (addr == last_w) {
        // return;
    }
    last_w = addr;
    char buf[64];
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 35
    snprintf(buf, sizeof(buf), "@w [" FMT_ADDR "] <- " FMT_WORD " <%b>\n", addr, data, wmask);
#else
    snprintf(buf, sizeof(buf), "@w [" FMT_ADDR "] <- " FMT_WORD " <0x%x>\n", addr, data, wmask);
#endif
    LogPrintf("%s", buf);
    if (g_args->verbosity >= MTRACE_PRINT_VERBOSITY) {
        printf("%s", buf);
    }
}
#endif
