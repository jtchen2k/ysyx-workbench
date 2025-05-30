/*
 * common.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:36:17
 * @modified: 2025-04-24 15:38:06
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_COMMON__
#define __INCLUDE_COMMON__

#include "config.h"
#include "macro.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <assert.h>
#include <cstdint>

typedef uint32_t word_t;
typedef int32_t  sword_t;
typedef uint32_t paddr_t;

void exit_reporter();

#endif /* __INCLUDE_COMMON__ */
