/*
 * mtrace.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-14 15:44:39
 * @modified: 2025-03-14 16:21:49
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "trace.h"

void mtrace_write(paddr_t addr, int len, word_t data) {
#ifdef CONFIG_MTRACE
#ifdef CONFIG_MTRACE_COND
  if (ITRACE_COND) {
    _Log("@ " FMT_PADDR " <- " FMT_WORD "\n", addr, data);
  }
#else
  _Log("[mtrace] " FMT_PADDR " <- " FMT_WORD "\n", addr, data);
#endif
#endif
}

void mtrace_read(paddr_t addr, int len, word_t data) {
#ifdef CONFIG_MTRACE
#ifdef CONFIG_MTRACE_COND
  if (ITRACE_COND) {
    _Log("@ " FMT_PADDR " -> " FMT_WORD "\n", addr, data);
  }
#else
  _Log("[mtrace] " FMT_PADDR " <- " FMT_WORD "\n", addr, data);
#endif
#endif
}