/*
 * monitor.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:17:58
 * @modified: 2025-04-09 11:39:32
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

void Arguments::parse_args(int argc, char **argv) {
    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception &err) {
        LogError("failed to parse argument: %s", err.what());
        exit(-1);
    }
    if (parser.get<bool>("--help")) {
        std::cout << parser;
        exit(0);
    }
}

void load_image() {
    if (g_args->image.empty()) {
        LogInfo("no image file specified, using default image.");
        pmem_init();
        return;
    }
    FILE *fp = fopen(g_args->image.c_str(), "rb");
    if (fp == nullptr) {
        LogError("failed to open image file: %s", g_args->image.c_str());
        exit(-1);
    }
    LogInfo("loading image file: %s", g_args->image.c_str());
    pmem_init(fp);
    fclose(fp);
}

void monitor_init(int argc, char **argv) {
    g_args = new Arguments();
    g_args->parse_args(argc, argv);
    g_args->print_argvals();
    load_image();
}

int monitor_exit() {
    core_stop();
    return R(10);
}