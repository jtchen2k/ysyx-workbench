/*
 * dpi.cc
 *
 *   Direct Programming Interfaces for the core
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:27:15
 * @modified: 2025-04-26 16:59:12
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop__Dpi.h"
#include "common.h"
#include "config.h"
#include "core.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"

extern "C" void dpi_ebreak() {
    LogDebug("dpi call to ebreak. core will terminate.");
    g_core_context->state = CORE_STATE_TERM;
#ifdef CONFIG_DIFFTEST
    // R10 = a0. use a0 as the return code.
    difftest_raise_intr(R(10));
#endif
}

extern "C" void dpi_ecall() {
    LogDebug("dpi call to ecall\n");
    return;
}

word_t dpi_ifetch_pc = 0;
/// called by IFU, pass the current instruction to exec.
extern "C" void dpi_ifetch(int inst, int dpi_pc) {
    // LogTrace("dpi call to ifetch: " FMT_WORD, inst);
    Assert(g_core != nullptr && g_core_context != nullptr, "core not initialized.");
    char   inststr[64];
    word_t pc = (word_t)dpi_pc;
    inststr[0] = '\0';
    g_core_context->exec_insts++;
    if (g_core_context->cycle_until_stop < CONFIG_MAX_PRINT_INST || g_args->verbosity >= 1) {
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
        printf(">> %lu [" FMT_ADDR "]: " FMT_WORD " %s\n", g_core_context->exec_insts, pc, inst,
               inststr);
    }
    dpi_ifetch_pc = pc;
#ifdef CONFIG_ITRACE
    if (strlen(inststr) == 0)
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
    LogPrintf(">> %9lu [" FMT_ADDR "]: " FMT_WORD " %s\n", g_core_context->exec_insts, pc, inst,
              inststr);
    itrace_trace(R(PC), inst, inststr);
#endif
}

/// This read cache is used to cache the read result of the previous read dpi calls.
/// Current implementation of DPI_MEM uses comb logic. This cache is used to avoid multiple mmio
/// read calls.
#define DPI_RCACHE_SIZE 8
static struct {
    paddr_t raddr;
    word_t  data;
    long    cycle;
} dpi_rcache[DPI_RCACHE_SIZE] = {};
static int dpi_rcache_n = 0;

/// read 4 bytes from pmem at address `raddr & ~0x3u` (4-byte aligned)
extern "C" int dpi_pmem_read(int raddr) {
    if (raddr & 0x3u) {
        LogWarn("attempted unaligned memory read: " FMT_ADDR, raddr);
    }
#ifndef CONFIG_DPIRCACHE
    return paddr_read(raddr & ~0x3u, 4);
#else
    // disable cache.
    paddr_t paddr = raddr & ~0x3u;
    long cur_cycle = g_vcontext->time();
    for (int i = 0; i < DPI_RCACHE_SIZE; i++) {
        auto &cache = dpi_rcache[i];
        if (cache.raddr == paddr && cache.cycle == cur_cycle) {
            return cache.data;
        }
    }
    word_t data = paddr_read(paddr, 4);
    dpi_rcache[dpi_rcache_n] = {paddr, data, cur_cycle};
    dpi_rcache_n = (dpi_rcache_n + 1) % DPI_RCACHE_SIZE;
    return data;
#endif
}

/// write bytes to pmem at address `waddr & ~0x3u` (4-byte aligned)
/// wmask indicates which bytes to write (wmask = 0x3 means the last 2 bytes for each
/// byte) each bit in the mask (b0000 - b1111) indicates whether the corresponding byte in the word
/// should be written
extern "C" void dpi_pmem_write(int waddr, int wdata, char wmask) {
    if (waddr & 0x3u) {
        LogWarn("attempted unaligned memory write: " FMT_ADDR, waddr);
    }
    paddr_write(waddr & ~0x3u, wdata, wmask);
}
