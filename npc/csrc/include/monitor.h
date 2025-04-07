/*
 * monitor.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:23:54
 * @modified: 2025-04-07 16:06:33
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#ifndef __INCLUDE_MONITOR__
#define __INCLUDE_MONITOR__

#include "common.h"
#include <argparse/argparse.hpp>
#include <map>
#include <string>

class Arguments {
  public:
    std::string image = "";
    std::string elf = "";
    int         verbosity = 0;

    argparse::ArgumentParser parser;

    void parse_args(int argc, char **argv);

    void print_argvals() {
        std::map<std::string, std::string> argvals{
            {"image", image},
            {"elf", elf},
            {"verbosity", std::to_string(verbosity)},
        };
        std::string fmt = "arguments: ";
        for (const auto &pair : argvals) {
            fmt += pair.first + "=" + pair.second + ", ";
        }
        fmt.pop_back();
        fmt.pop_back();
        LogDebug("%s", fmt.c_str());
    };
};

void init_monitor(int argc, char **argv);

#endif // __INCLUDE_MONITOR__
