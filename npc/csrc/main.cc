/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-02-07 22:46:59
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop.h"
#include <cassert>
#include <cstdio>
#include <nvboard.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#include "common.h"
#include "mem.h"
#include "utils.h"

#ifndef TOP_NAME
#define TOP_NAME VTop
#endif

static TOP_NAME      *core = new TOP_NAME();
static bool           TRACE_ENABLE = false;
static int            TRACE_DUMP_CYCLE = 1e4;
static int            TRACE_FLUSH_CYCLE = 1e6;
static VerilatedVcdC *g_trace = new VerilatedVcdC();
static int            cur_cycle = 0;

void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle_trace() { g_trace->flush(); }

word_t* regs[] = {
    &core->io_regs_0, &core->io_regs_1,  &core->io_regs_2,  &core->io_regs_3,
    &core->io_regs_4, &core->io_regs_5,  &core->io_regs_6,  &core->io_regs_7,
    &core->io_regs_8, &core->io_regs_9,  &core->io_regs_10, &core->io_regs_11,
    &core->io_regs_12, &core->io_regs_13, &core->io_regs_14, &core->io_regs_15,
    &core->io_regs_16, &core->io_regs_17, &core->io_regs_18, &core->io_regs_19,
    &core->io_regs_20, &core->io_regs_21, &core->io_regs_22, &core->io_regs_23,
    &core->io_regs_24, &core->io_regs_25, &core->io_regs_26, &core->io_regs_27,
    &core->io_regs_28, &core->io_regs_29, &core->io_regs_30, &core->io_regs_31,
};
#define R(i) (*regs[i])

static void print_register() {
    printf("[cycle %6d pc = 0x%08x inst = 0x%08x]: \n", cur_cycle, core->io_pc, core->io_inst);
    for (int i = 0; i < 32; i++) {
        printf("x%-2d = 0x%08x ", i, *regs[i]);
        if (i % 8 == 7)
            printf("\n");
    }
    printf("\n");
}

static void clock_step() {
    cur_cycle++;
    core->clock = 0;
    core->eval();
    core->clock = 1;
    core->eval();
    if (TRACE_ENABLE) {
        if (cur_cycle % TRACE_DUMP_CYCLE == 0)
            g_trace->dump(cur_cycle);
        if (cur_cycle % TRACE_FLUSH_CYCLE == 0) {
            single_cycle_trace();
            printf("dumped trace. cycle = %d\n", cur_cycle);
        }
    }
}

static void reset(int n) {
    core->reset = 1;
    while (n-- > 0)
        clock_step();
    core->reset = 0;
}

static void setup_trace() {
    if (!TRACE_ENABLE)
        return;
    Verilated::traceEverOn(true);
    core->trace(g_trace, 99);
    g_trace->open("waveform_light.vcd");
}

static void exec() {
    word_t pc = core->io_pc;
    word_t inst = pmem_read(pc, 4);
    core->io_inst = inst;
    print_register();
    clock_step();
}

int main() {
    pmem_init();
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