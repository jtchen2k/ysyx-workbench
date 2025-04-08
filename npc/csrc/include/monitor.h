/*
 * monitor.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:23:54
 * @modified: 2025-04-09 00:06:04
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#ifndef __INCLUDE_MONITOR__
#define __INCLUDE_MONITOR__

#include "common.h"
#include <argparse/argparse.hpp>
#include <map>
#include <string>

#define EXPR_TOKEN_SIZE 4096
#define EXPR_TOKEN_LENGTH 32
#define EXPR_MAX_LENGTH 1024

class Arguments {
  public:
    std::string image = "";
    std::string elf = "";
    std::string log_dir = "";
    bool        batch = false;
    int         verbosity = 0;

    argparse::ArgumentParser parser;

    void parse_args(int argc, char **argv);

    Arguments() {
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
            .action([this](const auto &) { ++this->verbosity; })
            .append();
        parser.add_argument("-b", "--batch")
            .help("run in batch mode")
            .default_value(false)
            .implicit_value(true)
            .store_into(batch);
    }

    void print_argvals() {
        std::map<std::string, std::string> argvals{
            {"image", image},
            {"elf", elf},
            {"log_dir", log_dir},
            {"batch", std::to_string(batch)},
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

extern Arguments *g_args;

void monitor_init(int argc, char **argv);

void sdb_mainloop();

void sdb_init();

void regex_init();

void wp_init();

word_t expr(char *e, bool *success);

#endif // __INCLUDE_MONITOR__
