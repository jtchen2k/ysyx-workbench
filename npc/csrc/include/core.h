/*
 * core.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:05:30
 * @modified: 2025-02-15 03:49:06
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CORE__
#define __INCLUDE_CORE__

#include "svdpi.h"
#include "VTop.h"
#include "VTop__Dpi.h"
#include <verilated.h>
#include <verilated_vcd_c.h>

static int               cur_cycle = 0;
static VerilatedVcdC    *g_trace = new VerilatedVcdC();
static VerilatedContext *g_context = new VerilatedContext();
static TOP_NAME         *g_core = new TOP_NAME(g_context);


#ifndef TOP_NAME
#define TOP_NAME VTop
#endif

enum { CORE_RUNNING, CORE_STOP, CORE_QUIT };

typedef struct {
  int state;
} g_core_state;

void core_init();

/// main loop of the core.
void exec(int n);

// /// reset the core for n cycles.
// void reset(int n);

void core_shutdown();

#endif /* __INCLUDE_CORE__ */
