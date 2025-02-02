/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-02-01 21:20:30
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

static TOP_NAME      *dut = new TOP_NAME();
static bool           TRACE_ENABLE = false;
static int            TRACE_DUMP_CYCLE = 1e4;
static int            TRACE_FLUSH_CYCLE = 1e6;
static VerilatedVcdC *g_trace = new VerilatedVcdC();
static int            cur_cycle = 0;

void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle_trace() { g_trace->flush(); }

static void single_cycle() {
    cur_cycle++;
    dut->clock = 0;
    dut->eval();
    dut->clock = 1;
    dut->eval();
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
    dut->reset = 1;
    while (n-- > 0)
        single_cycle();
    dut->reset = 0;
}

static void setup_trace() {
    if (!TRACE_ENABLE)
        return;
    Verilated::traceEverOn(true);
    dut->trace(g_trace, 99);
    g_trace->open("waveform_light.vcd");
}

int main() {
    // setup_trace();
    // nvboard_bind_all_pins(dut);
    // nvboard_init();
    pmem_init();
    reset(10);
    while (1) {
        single_cycle();
    }
    return 0;
}