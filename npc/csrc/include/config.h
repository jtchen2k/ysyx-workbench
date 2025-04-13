/*
 * config.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:55:48
 * @modified: 2025-04-13 13:57:30
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CONFIG__
#define __INCLUDE_CONFIG__

#include <generated/autoconf.h>

#define CONFIG_MBASE 0x80000000
#define CONFIG_MSIZE 0x8000000 // = 128MB
#define CONFIG_PC_RESET_OFFSET 0x0

// max number of instructions to execute
#define CONFIG_MAX_INST 1000

#define TRACE_FLUSH_CYCLE 1

#endif /* __INCLUDE_CONFIG__ */
