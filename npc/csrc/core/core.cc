/*
 * core.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:21:40
 * @modified: 2025-02-15 16:53:28
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "core.h"
#include "config.h"
#include "mem.h"
#include "utils.h"

VerilatedVcdC    *g_trace = new VerilatedVcdC();
VerilatedContext *g_context = new VerilatedContext();
TOP_NAME         *g_core = new TOP_NAME(g_context);
CoreState        *g_core_state;

static void trace_init() {
    if (!TRACE_ENABLE)
        return;
    g_context->traceEverOn(true);
    g_core->trace(g_trace, 99);
    g_trace->open(TRACE_FILENAME);
    LogDebug("trace enabled: %s", TRACE_FILENAME);
}

void core_init() {
    trace_init();
    g_core_state = new CoreState();
    g_core_state->state = CORE_STATE_RUNNING;
    reset(10);
}

void exec(int n) {
    while (n--) {
        word_t pc = g_core->io_pc;
        word_t inst = pmem_read(pc, 4);
        g_core->io_inst = inst;
        // print_register();
        single_cycle();
    }
}

void reset(int n) {
    g_core->reset = 1;
    while (n--) {
        single_cycle();
    }
    g_core->reset = 0;
}

void single_cycle() {
    g_core->clock = 0;
    g_core->eval();
    g_core->clock = 1;
    g_core->eval();

    g_context->timeInc(1);
    if (TRACE_ENABLE) {
        auto t = g_context->time();
        g_trace->dump(t);
        if (t % TRACE_FLUSH_CYCLE == 0)
            g_trace->flush();
    }
}

void core_shutdown() {
    LogTrace("core shutdown.");
    if (TRACE_ENABLE) {
        g_trace->flush();
        g_trace->close();
    }
}
