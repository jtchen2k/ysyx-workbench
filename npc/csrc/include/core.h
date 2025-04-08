/*
 * core.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:05:30
 * @modified: 2025-04-08 23:03:07
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CORE_H__
#define __INCLUDE_CORE_H__

#include "VTop.h"
#include "utils.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

enum core_state_t {
    CORE_STATE_RUNNING, // running
    CORE_STATE_STOP,    // stopped (but can continue)
    CORE_STATE_TERM     // terminated (cannot continue)
};

struct CoreState {
    core_state_t state;
    time_t       startup_time;
    time_t       running_time;
    uint64_t     exec_cycles;
};

extern VerilatedVcdC    *g_trace;
extern VerilatedContext *g_context;
extern TOP_NAME         *g_core;
extern CoreState        *g_core_state;

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

#endif // __INCLUDE_CORE_H__
