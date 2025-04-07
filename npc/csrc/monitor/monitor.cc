/*
 * monitor.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:17:58
 * @modified: 2025-04-07 16:07:20
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "monitor.h"
#include "mem.h"
#include "utils.h"
#include <string>

static Arguments *args;

void Arguments::parse_args(int argc, char **argv) {
    parser.add_argument("-i", "--image")
        .help("the binary image file to load")
        .store_into(image);
    parser.add_argument("-e", "--elf")
        .help("the elf file to load")
        .store_into(elf);
    parser.add_argument("-V", "--verbosity")
        .help("set the verbosity level")
        .default_value(0)
        .implicit_value(true)
        .action([this](const auto &) {++this->verbosity;})
        .append();
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
    if (args->image.empty()) {
        LogInfo("no image file specified, using default image.");
        pmem_init();
        return;
    }
    FILE *fp = fopen(args->image.c_str(), "rb");
    if (fp == nullptr) {
        LogError("failed to open image file: %s", args->image.c_str());
        exit(-1);
    }
    LogInfo("loading image file: %s", args->image.c_str());
    pmem_init(fp);
    fclose(fp);
}

void init_monitor(int argc, char **argv) {
    args = new Arguments();
    args->parse_args(argc, argv);
    args->print_argvals();
    load_image();
}