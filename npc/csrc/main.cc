/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-04-08 20:44:04
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include <cassert>
#include <cstdio>
#include <nvboard.h>

#include "common.h"
#include "config.h"
#include "core.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"

void nvboard_bind_all_pins(TOP_NAME *top);

int main(int argc, char **argv) {
    welcome();
    core_init();
    monitor_init(argc, argv);
    sdb_init();
    core_start();
    //     print_register();
    //     // Assert(R(1) == 12, "x1 = %d", R(1));
    //     // Assert(R(2) == 16, "x2 = %d", R(2));
    //     // Assert(R(3) == 36, "x3 = %d", R(3));
    //     // Assert(R(4) == 16, "x4 = %d", R(4));
    //     // LogInfo("test passed.");
    // a0 (x10)
    return R(10);
}