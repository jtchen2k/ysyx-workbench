/*
 * iringbuf.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-14 14:13:59
 * @modified: 2025-03-14 15:35:34
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "macro.h"
#include "trace.h"
#include <string.h>

static ITraceNode  iringbuf[ITRACE_POOL_SIZE] = {};
static ITraceNode *next = NULL;

static void irb_init() {
  for (int i = 0; i < ITRACE_POOL_SIZE; i++) {
    iringbuf[i].next = (i == ITRACE_POOL_SIZE - 1 ? &iringbuf[0] : &iringbuf[i + 1]);
  }
  next = iringbuf;
}

void init_itrace() {
  irb_init();
}

void irb_trace(word_t inst, char *instdec, vaddr_t pc) {
  ITraceNode *cur = next;
  next = next->next;
  cur->inst = inst;
  cur->pc = pc;
  strncpy(cur->instdec, instdec, ITRACE_BUF_SIZE - 1);
}

void irb_display() {
  Log("Instruction Ring Buffer:");
  for (int i = 0; i < ITRACE_POOL_SIZE; i++) {
    char line[128] = {};
    char* p = line;
    char *prefix = iringbuf[i].next == next ? " ---> " : "      ";
    p += snprintf(p, 7, "%s", prefix);
    p += snprintf(p, 15, FMT_WORD ": ", iringbuf[i].pc);
    uint8_t *instp = (uint8_t *)&iringbuf[i].inst;
    for (int j= MUXDEF(CONFIG_ISA64, 8, 4) - 1; j >=0; --j) {
      p += snprintf(p, 4, " %02x", instp[j]);
    }
    p += snprintf(p, 36, " %-30s", iringbuf[i].instdec);
    printf("%s\n", line);
  }
}
