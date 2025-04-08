/*
 * itrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 23:49:58
 * @modified: 2025-04-08 23:50:32
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"

struct ITraceNode {
    word_t pc;
    word_t inst;
};