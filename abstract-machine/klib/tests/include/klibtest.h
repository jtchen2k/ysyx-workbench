/*
 * klibtest.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-28 00:23:15
 * @modified: 2025-03-28 12:03:55
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_KLIBTEST__
#define __INCLUDE_KLIBTEST__

#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;45m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"
#define ANSI_CLEAR_LINE "\33[K"

#define START(x) printf(ANSI_FG_YELLOW "[START] " ANSI_NONE "%s...", x)
#define PASS(x) printf("\r" ANSI_CLEAR_LINE ANSI_FG_GREEN " [PASS] " ANSI_NONE "%s\n", x)

#undef assert
#define assert(cond) \
  do { \
    if (!(cond)) { \
    printf(ANSI_FG_RED " [FAIL] " ANSI_NONE "Assertion fail at %s:%d\n", __FILE__, __LINE__); \
    halt(1); \
    } \
  } while (0)

void test_memw();
void test_memr();
void test_strw();
void test_strr();
void test_io();

#endif // __INCLUDE_KLIBTEST__
