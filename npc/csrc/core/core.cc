/*
 * core.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:21:40
 * @modified: 2025-04-11 15:11:33
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "core.h"
#include "common.h"
#include "config.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"

#define PRINT_INST 0x01
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
    reginfo_init();
    reset(10);
}

void core_start() {
    Assert(g_core != nullptr, "core not initialized.");
    Assert(g_core_state != nullptr, "core state not initialized.");
    Assert(g_context != nullptr, "context not initialized.");
    LogDebug("core started.");
    sdb_mainloop();
}

/// execute a single cycle
static void exec_once(uint32_t flags) {
    // evaluate watchpoints
    wp_eval();
    word_t pc = g_core->io_pc;
    word_t inst = pmem_read(pc, 4);
    g_core->io_inst = inst;
    single_cycle();
}

/// execute n instructions. early stop if term.
void core_exec(uint64_t n) {
    bool print_stdio = (n <= CONFIG_MAX_PRINT_INST);
    switch (g_core_state->state) {
    case CORE_STATE_TERM:
        printf("core has terminated. to continue, please restart npc.\n");
        return;
    default:
        g_core_state->state = CORE_STATE_RUNNING;
        break;
    }

    while (n--) {
        exec_once(print_stdio ? PRINT_INST : 0);
        if (g_core_state->state != CORE_STATE_RUNNING)
            break;
    }

    switch (g_core_state->state) {
    case CORE_STATE_RUNNING:
        g_core_state->state = CORE_STATE_STOP;
        break;
    case CORE_STATE_STOP:
        break;
    case CORE_STATE_TERM:
        check_trap();
        break;
    }
}

void reset(uint64_t n) {
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

void core_stop() {
    if (TRACE_ENABLE) {
        g_trace->flush();
        g_trace->close();
    }
}

void statistics() {}

int check_trap() {
    int code = R(10);
    if (code == 0) {
        LogSuccess("hit good trap at pc: " FMT_ADDR, g_core->io_pc);
    } else {
        LogError("hit bad trap: %d", code);
    }
    return code;
}