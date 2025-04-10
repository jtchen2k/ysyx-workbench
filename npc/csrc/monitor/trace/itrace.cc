/*
 * itrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 23:49:58
 * @modified: 2025-04-10 22:17:02
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"

#define ITRACE_IRB_SIZE 32

struct ITraceNode {
    word_t pc;
    word_t inst;
};

static

void itrace_init() {
};

void itrace_trace(word_t pc, word_t inst) {
};