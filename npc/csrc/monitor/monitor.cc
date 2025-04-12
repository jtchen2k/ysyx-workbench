/*
 * monitor.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:17:58
 * @modified: 2025-04-12 17:46:22
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "monitor.h"
#include "core.h"
#include "mem.h"
#include "utils.h"
#include <string>

Arguments *g_args;
FILE      *g_log_file;
int        g_verbosity = 2;

static int difftest_port = 1234;

void Arguments::parse_args(int argc, char **argv) {
    try {
        parser.parse_args(argc, argv);
        g_verbosity = parser.get<int>("--verbosity");
    } catch (const std::exception &err) {
        LogError("failed to parse argument: %s", err.what());
        exit(-1);
    }
    if (parser.get<bool>("--help")) {
        std::cout << parser;
        exit(0);
    }
}

long load_image() {
    if (g_args->image.empty()) {
        LogInfo("no image file specified, using default image.");
        return pmem_init();
    }
    FILE *fp = fopen(g_args->image.c_str(), "rb");
    if (fp == nullptr) {
        LogError("failed to open image file: %s", g_args->image.c_str());
        exit(-1);
    }
    LogInfo("loading image file: %s", g_args->image.c_str());
    long size = pmem_init(fp);
    fclose(fp);
    return size;
}

void monitor_init(int argc, char **argv) {
    g_args = new Arguments();
    g_args->parse_args(argc, argv);
    g_args->print_argvals();

    g_log_file = fopen(g_args->log_file.c_str(), "w");
    Assert(g_log_file, "failed to open log file: %s", g_args->log_file.c_str());
    long size = load_image();

#ifdef CONFIG_ITRACE
    itrace_init();
    LogInfo("instruction trace enabled.");
#endif

#ifdef CONFIG_DIFFTEST
    difftest_init(size, difftest_port);
#endif
}

int monitor_exit() {
    fclose(g_log_file);
    core_stop();
    return R(10);
}