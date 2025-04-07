/*
 * utils.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:08:42
 * @modified: 2025-04-07 17:20:19
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_UTILS__
#define __INCLUDE_UTILS__

#include <cstdio>
#include <cstdlib>
#include <ctime>

// ----------- log -----------

#define FMT_ADDR "0x%08x"


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

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE "\n"

#define _Log(fmt, ...) \
    printf("< %6s:%-4d:%s> " fmt "\n", __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)

#define _LogColor(color, fmt, ...) \
    printf(ANSI_FMT("[%s:%d.%s] " fmt, color), __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)

#define Log(fmt, ...) _LogColor(ANSI_FG_BLACK, fmt, ##__VA_ARGS__)
#define LogTrace(fmt, ...) _LogColor(ANSI_FG_MAGENTA, "[trace] " fmt, ##__VA_ARGS__)
#define LogDebug(fmt, ...) _LogColor(ANSI_FG_CYAN, "[debug] " fmt, ##__VA_ARGS__)
#define LogInfo(fmt, ...) _LogColor(ANSI_FG_BLUE, "[info] " fmt, ##__VA_ARGS__)
#define LogWarn(fmt, ...) _LogColor(ANSI_FG_YELLOW, "[warn] " fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) _LogColor(ANSI_FG_RED, "[error] " fmt, ##__VA_ARGS__)

#define Panic(fmt, ...) \
    do { \
        _LogColor(ANSI_BG_RED, "[panic] " fmt, ##__VA_ARGS__); \
        exit(1); \
    } while (0)

// ----------- assert -----------
#define Assert(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            _LogColor(ANSI_BG_RED, "Assertion failed: " fmt, ##__VA_ARGS__); \
            exit(1); \
        } \
    } while (0)

#endif /* __INCLUDE_UTILS__ */