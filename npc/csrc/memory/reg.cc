/*
 * reg.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 20:46:57
 * @modified: 2025-04-09 12:32:48
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "core.h"
#include "mem.h"
#include "utils.h"
#include <string>
#include <vector>

static std::vector<std::vector<std::string>> reg_names = {
    {"x0", "zero"},     // hard wired to 0
    {"x1", "ra"},       // return address
    {"x2", "sp"},       // stack pointer
    {"x3", "gp"},       // global pointer
    {"x4", "tp"},       // thread pointer
    {"x5", "t0"},       // tmp register
    {"x6", "t1"},       // tmp register
    {"x7", "t2"},       // tmp register
    {"x8", "s0", "fp"}, // saved register, frame pointer
    {"x9", "s1"},       // saved register
    {"x10", "a0"},      // func param, return value
    {"x11", "a1"},      // func param, return value
    {"x12", "a2"},      // func param
    {"x13", "a3"},      // func param
    {"x14", "a4"},      // func param
    {"x15", "a5"},      // func param
    {"x16", "a6"},      // func param
    {"x17", "a7"},      // func param
    {"x18", "s2"},      // saved register
    {"x19", "s3"},      // saved register
    {"x20", "s4"},      // saved register
    {"x21", "s5"},      // saved register
    {"x22", "s6"},      // saved register
    {"x23", "s7"},      // saved register
    {"x24", "s8"},      // saved register
    {"x25", "s9"},      // saved register
    {"x26", "s10"},     // saved register
    {"x27", "s11"},     // saved register
    {"x28", "t3"},      // tmp register
    {"x29", "t4"},      // tmp register
    {"x30", "t5"},      // tmp register
    {"x31", "t6"},      // tmp register
    {"pc"},             // program counter
};
static word_t **regs;

void reginfo_init() {
    regs = new word_t *[33]{
        &g_core->io_regs_0,  &g_core->io_regs_1,  &g_core->io_regs_2,
        &g_core->io_regs_3,  &g_core->io_regs_4,  &g_core->io_regs_5,
        &g_core->io_regs_6,  &g_core->io_regs_7,  &g_core->io_regs_8,
        &g_core->io_regs_9,  &g_core->io_regs_10, &g_core->io_regs_11,
        &g_core->io_regs_12, &g_core->io_regs_13, &g_core->io_regs_14,
        &g_core->io_regs_15, &g_core->io_regs_16, &g_core->io_regs_17,
        &g_core->io_regs_18, &g_core->io_regs_19, &g_core->io_regs_20,
        &g_core->io_regs_21, &g_core->io_regs_22, &g_core->io_regs_23,
        &g_core->io_regs_24, &g_core->io_regs_25, &g_core->io_regs_26,
        &g_core->io_regs_27, &g_core->io_regs_28, &g_core->io_regs_29,
        &g_core->io_regs_30, &g_core->io_regs_31, &g_core->io_pc,
    };
}

word_t R(int i) {
    Assert(i >= 0 && i < 32, "invalid register index: %d", i);
    return *regs[i];
}

word_t R(char *name, bool *success) {
    // allow pc
    for (int i = 0; i < 33; i++) {
        for (const auto &n : reg_names[i]) {
            if (strcmp(name, n.c_str()) == 0) {
                *success = true;
                return *regs[i];
            }
        }
    }
    *success = false;
    LogError("invalid register name: %s", name);
    return -1;
}

word_t R(char *name) {
    bool success = false;
    return R(name, &success);
}

void reg_display() {
    printf("┌─────┬────────┬────────────┬──────────────┬──────────────┐\n");
    printf("│ reg │ abi    │ hex        │ uint         │ int          │\n");
    printf("├─────┼────────┼────────────┼──────────────┼──────────────┤\n");
    for (int i = 0; i < 33; i++) {
        word_t      val = *regs[i];
        std::string abi_name = (i < 32) ? reg_names[i][1] : "pc";
        if (reg_names[i].size() > 2) {
            abi_name += "/" + reg_names[i][2]; // Add fp alias for s0
        }
        printf("│ x%-2d │ %-6s │ 0x%08x │ %-12u │ %-12d │\n", i,
               abi_name.c_str(), (uint32_t)val, (uint32_t)val, (int32_t)val);
    }
    printf("└─────┴────────┴────────────┴──────────────┴──────────────┘\n");
}
