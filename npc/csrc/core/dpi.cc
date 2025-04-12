/*
 * dpi.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:27:15
 * @modified: 2025-04-12 23:56:17
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
    LogDebug("dpi call to ebreak");
    g_core_context->state = CORE_STATE_TERM;
#ifdef CONFIG_DIFFTEST
    difftest_raise_intr(R(10));
#endif
}

extern "C" void dpi_ecall() {
    LogDebug("dpi call to ecall\n");
    return;
}

/// called by IFU, pass the current instruction to exec.
extern "C" void dpi_ifetch(int inst) {
    // LogTrace("dpi call to ifetch: " FMT_WORD, inst);
    Assert(g_core != nullptr && g_core_context != nullptr, "core not initialized.");
    char   inststr[64];
    word_t pc = R(PC);
    inststr[0] = '\0';
    if (g_core_context->cycle_until_stop < CONFIG_MAX_PRINT_INST) {
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
        printf("[" FMT_ADDR "]: " FMT_WORD " %s\n", pc, inst, inststr);
    }
#ifdef CONFIG_ITRACE
    if (strlen(inststr) == 0)
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
    LogPrintf("[" FMT_ADDR "]: " FMT_WORD " %s\n", pc, inst, inststr);
    itrace_trace(R(PC), inst, inststr);
#endif
}

/// read 4 bytes from pmem at address `raddr & ~0x3u` (4-byte aligned)
extern "C" int dpi_pmem_read(int raddr) {
    return pmem_read(raddr & ~0x3u, 4);
}

/// write 4 bytes to pmem at address `waddr & ~0x3u` (4-byte aligned)
/// wmask indicates which bits to write in a single byte (wmask = 0x3 means the last 2 bytes for each byte)
extern "C" void dpi_pmem_write(int waddr, int wdata, char wmask) {
    pmem_write(waddr & ~0x3u, wdata, wmask);
}
