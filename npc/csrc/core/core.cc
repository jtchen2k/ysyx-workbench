/*
 * core.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:21:40
 * @modified: 2025-04-13 16:20:19
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "core.h"
#include "common.h"
#include "config.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"
#include <ctime>

#define PRINT_INST 0x01
VerilatedVcdC    *g_trace = new VerilatedVcdC();
VerilatedContext *g_vcontext = new VerilatedContext();
TOP_NAME         *g_core = new TOP_NAME(g_vcontext);
CoreContext      *g_core_context;

static void wtrace_init() {
#ifdef CONFIG_WTRACE
    g_vcontext->traceEverOn(true);
    g_core->trace(g_trace, 99);
    g_trace->open(CONFIG_WAVEFORM_FILE);
    LogDebug("waveform trace enabled: %s", CONFIG_WAVEFORM_FILE);
#endif
}

void core_init() {
    wtrace_init();
    g_core_context = new CoreContext();
    g_core_context->state = CORE_STATE_RUNNING;
    g_core_context->cycle_until_stop = 0;
    g_core_context->running_time = 0;
    g_core_context->startup_time = clock();
    g_core_context->exec_cycles = 0;
    g_core_context->exec_insts = 0;
    reginfo_init();
    reset(10);
}

void core_start() {
    Assert(g_core != nullptr, "core not initialized.");
    Assert(g_core_context != nullptr, "core state not initialized.");
    Assert(g_vcontext != nullptr, "context not initialized.");
    LogDebug("core started.");
    sdb_mainloop();
}

/// execute a single cycle
static void exec_once(uint32_t flags) {
    // word_t pc = R(PC);
    // word_t inst = pmem_read(pc, 4);
    // g_core->io_inst = inst;
    single_cycle();
    wp_eval();
}

/// execute n instructions. early stop if term.
void core_exec(uint64_t n) {
    bool print_stdio = (n <= CONFIG_MAX_PRINT_INST);
    switch (g_core_context->state) {
    case CORE_STATE_TERM:
        printf("core has terminated. to continue, please restart npc.\n");
        return;
    default:
        g_core_context->state = CORE_STATE_RUNNING;
        break;
    }

    while (n--) {
        g_core_context->exec_cycles++;
        g_core_context->cycle_until_stop = n;
        clock_t start_time = clock();
        exec_once(print_stdio ? PRINT_INST : 0);
        clock_t end_time = clock();
        g_core_context->running_time += (end_time - start_time);
#ifdef CONFIG_DIFFTEST
        difftest_step(R(PC));
#endif
        if (g_core_context->state != CORE_STATE_RUNNING)
            break;
    }

    switch (g_core_context->state) {
    case CORE_STATE_RUNNING:
        g_core_context->state = CORE_STATE_STOP;
        break;
    case CORE_STATE_STOP:
        break;
    case CORE_STATE_TERM:
        check_trap();
        statistics();
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
    g_vcontext->timeInc(1);

#ifdef CONFIG_WTRACE
    auto t = g_vcontext->time();
    g_trace->dump(t);
    if (t % TRACE_FLUSH_CYCLE == 0)
        g_trace->flush();
#endif
}

void core_stop() {
#ifdef CONFIG_WTRACE
    g_trace->flush();
    g_trace->close();
#endif
}

void statistics() {
    double inst_pers = (double)g_core_context->exec_cycles /
                       ((double)g_core_context->running_time / (CLOCKS_PER_SEC));
    LogInfo("statistics: %.3f inst/s", inst_pers);
}

int check_trap() {
    int code = R(10);
    if (code == 0) {
        LogSuccess("hit good trap at pc: " FMT_ADDR, R(PC));
    } else {
        LogError("hit bad trap: %d", code);
    }
    return code;
}