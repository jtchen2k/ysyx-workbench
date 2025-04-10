/*
 * monitor.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-07 14:23:54
 * @modified: 2025-04-10 22:02:11
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
    std::string log = "npc.log";
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
        parser.add_argument("-l", "--log")
            .help("the log file to write")
            .default_value("npc.log")
            .store_into(log);
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
            {"log", log},
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
int monitor_exit();

/// sdb
void sdb_mainloop();
void sdb_init();

// expr
void regex_init();
word_t expr_eval(char *e, bool *success);

/// wp
void wp_init();
void wp_add(char *expr, bool *success);
void wp_remove(int id, bool *success);
void wp_display();
void wp_eval();

/// trace & disasm
void disasm_init();
void disasm(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);

#endif // __INCLUDE_MONITOR__
