/*
 * core.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:05:30
 * @modified: 2025-04-12 23:01:48
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CORE_H__
#define __INCLUDE_CORE_H__

#include "VTop.h"
#include "VTop___024root.h"
#include "common.h"
#include "utils.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

enum core_state_t {
    CORE_STATE_RUNNING, // running
    CORE_STATE_STOP,    // stopped (but can continue)
    CORE_STATE_TERM     // terminated (cannot continue)
};

struct CoreContext {
    core_state_t state;
    clock_t      startup_time;
    clock_t      running_time;
    uint64_t     cycle_until_stop;
    uint64_t     exec_cycles;
};

extern VerilatedVcdC    *g_trace;
extern VerilatedContext *g_vcontext;
extern TOP_NAME         *g_core;
extern CoreContext      *g_core_context;

// core and trace initialization
void core_init();

/// start the core
void core_start();

/// execute n cycles
void core_exec(uint64_t n);

/// reset the core for n cycles.
void reset(uint64_t n);

/// step one single cycle
void single_cycle();

/// stop the core
void core_stop();

/// statistics
void statistics();

int check_trap();

/// difftest
void difftest_init(long img_size, int port);
void difftest_step(paddr_t pc);
void difftest_raise_intr(uint64_t NO);

#endif // __INCLUDE_CORE_H__
