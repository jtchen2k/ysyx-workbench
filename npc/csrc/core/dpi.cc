/*
 * dpi.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:27:15
 * @modified: 2025-04-11 15:14:55
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop__Dpi.h"
#include "common.h"
#include "core.h"
#include "monitor.h"
#include "utils.h"

extern "C" void dpi_ebreak() {
    LogDebug("dpi call to ebreak");
    g_core_state->state = CORE_STATE_TERM;
    return;
}

extern "C" void dpi_ecall() {
    LogDebug("dpi call to ecall\n");
    return;
}

/// called by IFU, pass the current instruction to exec.
extern "C" void dpi_ifetch(int inst) {
    // LogTrace("dpi call to ifetch: " FMT_WORD, inst);
    Assert(g_core != nullptr, "core not initialized.");
#ifdef CONFIG_ITRACE
    word_t pc = g_core->io_pc;
    char   inststr[64];
    disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
    LogPrintf("[" FMT_ADDR "]: " FMT_WORD " %s\n", pc, inst, inststr);
    itrace_trace(g_core->io_pc, inst, inststr);
#endif
}