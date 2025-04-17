/*
 * mtrace.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-14 15:44:39
 * @modified: 2025-04-17 23:44:44
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "trace.h"
#include <stdio.h>

void mtrace_write(paddr_t addr, int len, word_t data) {
#ifdef CONFIG_MTRACE
  char buf[64];
  snprintf(buf, 63, "@w [" FMT_PADDR "] <- " FMT_WORD "\n", addr, data);
#ifdef CONFIG_MTRACE_COND
  if (ITRACE_COND) {
    log_write("%s", buf);
  }
#else
  _Log("%s", buf);
#endif
#endif
}

void mtrace_read(paddr_t addr, int len, word_t data) {
#ifdef CONFIG_MTRACE
  char buf[64];
  snprintf(buf, 63, "@r [" FMT_PADDR "] -> " FMT_WORD "\n", addr, data);
#ifdef CONFIG_MTRACE_COND
  if (ITRACE_COND) {
    log_write("%s", buf);
  }
#else
  _Log("%s", buf);
#endif
#endif
}