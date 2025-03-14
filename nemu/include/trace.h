/*
 * trace.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-14 14:22:09
 * @modified: 2025-03-14 16:11:07
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "common.h"


// itrace
#define ITRACE_POOL_SIZE 32
#define ITRACE_BUF_SIZE 32

typedef struct ITraceNode {
    vaddr_t pc;
    word_t inst;
    struct ITraceNode *next;
    char instdec[ITRACE_BUF_SIZE];
} ITraceNode;

void irb_trace(word_t inst, char *instdec, vaddr_t pc);
void irb_display();
void init_itrace();

// mtrace

typedef struct MTraceNode {
    paddr_t addr;
    bool is_read;
    word_t data;
} MTraceNode;

void mtrace_write(paddr_t addr, int len, word_t data);
void mtrace_read(paddr_t addr, int len, word_t data);