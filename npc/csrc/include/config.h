/*
 * config.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:55:48
 * @modified: 2025-04-08 21:47:45
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CONFIG__
#define __INCLUDE_CONFIG__

#define CONFIG_MBASE 0x80000000
#define CONFIG_MSIZE 0x8000000 // = 128MB
#define CONFIG_PC_RESET_OFFSET 0x0
#define CONFIG_MAX_INST 1000
#define CONFIG_MAX_PRINT_INST 10

// runtime configurations
#define TRACE_ENABLE true
#define TRACE_FILENAME "waveform.vcd"
#define TRACE_FLUSH_CYCLE 10

#endif /* __INCLUDE_CONFIG__ */
