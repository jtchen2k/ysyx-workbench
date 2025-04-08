/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-04-08 21:46:51
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
    return R(10);
}