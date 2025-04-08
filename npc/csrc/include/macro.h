/*
 * macro.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:35:07
 * @modified: 2025-04-08 18:24:41
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_MACRO__
#define __INCLUDE_MACRO__

#define PG_ALIGN __attribute((aligned(4096)))
#define NOINLINE __attribute__((noinline))
#define ARRLEN(x) (int)(sizeof(x) / sizeof((x)[0]))

#endif /* __INCLUDE_MACRO__ */
