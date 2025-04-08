/*
 * monitor.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:17:58
 * @modified: 2025-04-08 21:03:00
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "monitor.h"
#include "mem.h"
#include "utils.h"
#include <string>

Arguments *g_args;

void Arguments::parse_args(int argc, char **argv) {
    parser.add_argument("-i", "--image")
        .help("the binary image file to load")
        .store_into(image);
    parser.add_argument("-e", "--elf")
        .help("the elf file to load")
        .store_into(elf);
    parser.add_argument("-l", "--log_dir")
        .help("the directory to store log files")
        .store_into(log_dir);
    parser.add_argument("-V", "--verbosity")
        .help("set the verbosity level")
        .default_value(0)
        .implicit_value(true)
        .action([this](const auto &) {++this->verbosity;})
        .append();
    parser.add_argument("-b", "--batch")
        .help("run in batch mode")
        .default_value(false)
        .implicit_value(true)
        .store_into(batch);
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