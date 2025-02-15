/*
 * config.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:55:48
 * @modified: 2025-02-14 17:44:21
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_CONFIG__
#define __INCLUDE_CONFIG__

#define CONFIG_MBASE 0x80000000
#define CONFIG_MSIZE 0x8000000

// runtime configurations
#define TRACE_ENABLE true
#define TRACE_FILENAME "waveform.vcd"
#define TRACE_FLUSH_CYCLE 10

#endif /* __INCLUDE_CONFIG__ */
