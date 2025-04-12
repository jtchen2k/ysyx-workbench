/*
 * dpi.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:27:15
 * @modified: 2025-04-12 21:21:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop__Dpi.h"
#include "common.h"
#include "config.h"
#include "core.h"
#include "monitor.h"
#include "utils.h"

extern "C" void dpi_ebreak() {
    LogDebug("dpi call to ebreak");
    g_core_context->state = CORE_STATE_TERM;
    g_core_context->cycle_until_stop = 0;
    g_core_context->running_time = 0;
    g_core_context->startup_time = 0;
    g_core_context->exec_cycles = 0;
    return;
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
    word_t pc = g_core->io_pc;
    inststr[0] = '\0';
    if (g_core_context->cycle_until_stop < CONFIG_MAX_PRINT_INST) {
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
        printf("[" FMT_ADDR "]: " FMT_WORD " %s\n", pc, inst, inststr);
    }
#ifdef CONFIG_ITRACE
    if (strlen(inststr) == 0)
        disasm(inststr, sizeof(inststr), pc, (uint8_t *)&inst, 4);
    LogPrintf("[" FMT_ADDR "]: " FMT_WORD " %s\n", pc, inst, inststr);
    itrace_trace(g_core->io_pc, inst, inststr);
#endif
}