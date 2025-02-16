/*
 * core.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:05:30
 * @modified: 2025-02-15 16:48:40
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CORE_H__
#define __INCLUDE_CORE_H__

#include "VTop.h"
#include "VTop__Dpi.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

enum { CORE_STATE_RUNNING, CORE_STATE_STOP, CORE_STATE_QUIT };
struct CoreState {
  int state;
};

extern VerilatedVcdC    *g_trace;
extern VerilatedContext *g_context;
extern TOP_NAME         *g_core;
extern CoreState        *g_core_state;

void core_init();

/// main loop of the core.
void exec(int n);

/// reset the core for n cycles.
void reset(int n);

/// step one single cycle
void single_cycle();

void core_shutdown();

#endif // __INCLUDE_CORE_H__
