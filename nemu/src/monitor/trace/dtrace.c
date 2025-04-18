/*
 * dtrace.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-18 11:37:20
 * @modified: 2025-04-18 13:14:05
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "device/map.h"
#include "trace.h"

#ifndef DTRACE_COND
#define DTRACE_COND true
#endif

void dtrace_read(paddr_t addr, IOMap *map, word_t data) {
  if (DTRACE_COND) {
    char buf[64];
    const char *name = map->name;
    snprintf(buf, 63, "#r [" FMT_PADDR "](%s) -> " FMT_WORD "\n", addr, name, data);
    log_write("%s", buf);
  }
}

void dtrace_write(paddr_t addr, IOMap *map, word_t data) {
  if (DTRACE_COND) {
    char buf[64];
    const char *name = map->name;
    snprintf(buf, 63, "#w [" FMT_PADDR "] - %s <- " FMT_WORD "\n", addr, name, data);
    log_write("%s", buf);
  }
}