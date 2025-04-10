/*
 * itrace.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 23:49:58
 * @modified: 2025-04-11 00:37:35
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"
#include "utils.h"

#define ITRACE_IRB_SIZE 32

struct ITraceNode {
    word_t      pc;
    word_t      inst;
    char        inst_str[64];
    ITraceNode *next;
};

// instruction trace ring buffer
static ITraceNode *irb;
static ITraceNode *cur;

void itrace_init() {
    irb = (ITraceNode *)malloc(ITRACE_IRB_SIZE * sizeof(ITraceNode));
    cur = irb;
    for (int i = 0; i < ITRACE_IRB_SIZE; i++) {
        irb[i].next = (i == ITRACE_IRB_SIZE - 1) ? irb : &irb[i + 1];
        irb[i].pc = 0;
        irb[i].inst = 0;
        irb[i].inst_str[0] = '\0';
    }
};

void itrace_trace(word_t pc, word_t inst, const char *inst_str) {
    cur->pc = pc;
    cur->inst = inst;
    strcpy(cur->inst_str, inst_str);
    cur = cur->next;
};

void itrace_display() {
    for (int i = 0; i < ITRACE_IRB_SIZE; i++) {
        auto p = irb + i;
        printf("%4s [" FMT_WORD "] " FMT_WORD " %s\n", p->next == cur ? "->" : " ",
               p->pc, p->inst, p->inst_str);
    }
}