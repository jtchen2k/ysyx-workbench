/*
 * utils.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:08:42
 * @modified: 2025-04-11 00:45:22
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#ifndef __INCLUDE_UTILS__
#define __INCLUDE_UTILS__

#include <cstdio>
#include <cstdlib>
#include <ctime>


#define FMT_ADDR "0x%08x"
#define FMT_WORD "0x%08x"

// ----------- log -----------

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_LG_BLACK   "\33[30m"
#define ANSI_LG_RED     "\33[31m"
#define ANSI_LG_GREEN   "\33[32m"
#define ANSI_LG_YELLOW  "\33[33m"
#define ANSI_LG_BLUE    "\33[34m"
#define ANSI_LG_MAGENTA "\33[35m"
#define ANSI_LG_CYAN    "\33[36m"
#define ANSI_LG_WHITE   "\33[37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;45m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"
#define ANSI_BOLD       "\33[1m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE "\n"

extern FILE *g_log_file;
extern int g_verbosity;

/// print to log file only
#define LogPrintf(fmt, ...) do { \
    if (g_log_file) { \
        fprintf(g_log_file, fmt, ##__VA_ARGS__); \
        fflush(g_log_file); \
    } \
} while (0)

/// print together with log file
#define Printf(fmt, ...) do { \
    LogPrintf(fmt, ##__VA_ARGS__); \
    printf(fmt, ##__VA_ARGS__); \
} while (0)


#define _LogColor(color, fmt, ...) do { \
    printf(ANSI_FMT("[%s:%d.%s] " fmt, color), __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__); \
    if (g_log_file) { \
        fprintf(g_log_file, "[%s:%d.%s] " fmt "\n", __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__); \
        fflush(g_log_file); \
    } \
} while (0)


/// log to console and file (with ansi color and prefix)
#define Log(fmt, ...) _LogColor(ANSI_FG_BLACK, fmt, ##__VA_ARGS__)
#define LogTrace(fmt, ...) _LogColor(ANSI_LG_MAGENTA, "[trace] " fmt, ##__VA_ARGS__)
#define LogDebug(fmt, ...) _LogColor(ANSI_FG_CYAN, "[debug] " fmt, ##__VA_ARGS__)
#define LogInfo(fmt, ...) _LogColor(ANSI_FG_BLUE, "[info] " fmt, ##__VA_ARGS__)
#define LogSuccess(fmt, ...) _LogColor(ANSI_FG_GREEN, "[success] " fmt, ##__VA_ARGS__)
#define LogWarn(fmt, ...) _LogColor(ANSI_FG_YELLOW, "[warn] " fmt, ##__VA_ARGS__)
#define LogError(fmt, ...) _LogColor(ANSI_FG_RED, "[error] " fmt, ##__VA_ARGS__)
#define LogPlain(fmt, ...) _LogColor(ANSI_NONE, fmt, ##__VA_ARGS__)

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

inline void welcome() {
    Log("npc build time: %s %s", __DATE__, __TIME__);
}

#endif /* __INCLUDE_UTILS__ */