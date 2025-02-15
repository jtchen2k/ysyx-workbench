/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-02-15 03:53:40
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop.h"
#include <cassert>
#include <cstdio>
#include <nvboard.h>

#include "common.h"
#include "config.h"
#include "mem.h"
#include "utils.h"
#include "core.h"

void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle_trace() { g_trace->flush(); }

word_t* regs[] = {
    &g_core->io_regs_0, &g_core->io_regs_1,  &g_core->io_regs_2,  &g_core->io_regs_3,
    &g_core->io_regs_4, &g_core->io_regs_5,  &g_core->io_regs_6,  &g_core->io_regs_7,
    &g_core->io_regs_8, &g_core->io_regs_9,  &g_core->io_regs_10, &g_core->io_regs_11,
    &g_core->io_regs_12, &g_core->io_regs_13, &g_core->io_regs_14, &g_core->io_regs_15,
    &g_core->io_regs_16, &g_core->io_regs_17, &g_core->io_regs_18, &g_core->io_regs_19,
    &g_core->io_regs_20, &g_core->io_regs_21, &g_core->io_regs_22, &g_core->io_regs_23,
    &g_core->io_regs_24, &g_core->io_regs_25, &g_core->io_regs_26, &g_core->io_regs_27,
    &g_core->io_regs_28, &g_core->io_regs_29, &g_core->io_regs_30, &g_core->io_regs_31,
};
#define R(i) (*regs[i])

static void print_register() {
    printf("[cycle %6d pc = 0x%08x inst = 0x%08x]: \n", cur_cycle, g_core->io_pc, g_core->io_inst);
    for (int i = 0; i < 32; i++) {
        printf("x%-2d = 0x%08x ", i, *regs[i]);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");
}

static void clock_step() {
    g_core->clock = 0;
    g_core->eval();
    g_context->timeInc(1);
    g_core->clock = 1;
    g_core->eval();
    g_context->timeInc(1);
    if (TRACE_ENABLE) {
        g_trace->dump(g_context->time());
        if (g_context->time() % TRACE_FLUSH_CYCLE == 0) {
            single_cycle_trace();
        }
    }
}

static void reset(int n) {
    g_core->reset = 1;
    while (n-- > 0)
        clock_step();
    g_core->reset = 0;
}

static void setup_trace() {
    if (!TRACE_ENABLE)
        return;
    g_context->traceEverOn(true);
    g_core->trace(g_trace, 99);
    g_trace->open(TRACE_FILENAME);
    LogDebug("trace enabled: %s", TRACE_FILENAME);
}

static void exec() {
    word_t pc = g_core->io_pc;
    word_t inst = pmem_read(pc, 4);
    g_core->io_inst = inst;
    // print_register();
    clock_step();
}

static void ebreak() {
    LogWarn("EBREAK!");
}

int main() {
    pmem_init();
    setup_trace();
    reset(10);
    int n = 6; // total cycles
    while (n--) {
        exec();
    }
    print_register();
    Assert(R(1) == 12, "x1 = %d", R(1));
    Assert(R(2) == 16, "x2 = %d", R(2));
    Assert(R(3) == 36, "x3 = %d", R(3));
    Assert(R(4) == 16, "x4 = %d", R(4));
    LogInfo("test passed.");
    return 0;
}