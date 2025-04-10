/*
 * main.cc
 *
 * @project: npc
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-04-09 11:30:08
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include <nvboard.h>

#include "common.h"
#include "core.h"
#include "mem.h"
#include "monitor.h"

void nvboard_bind_all_pins(TOP_NAME *top);

int main(int argc, char **argv) {

    // display welcome message
    welcome();

    // init core, verilator context, and trace
    core_init();

    // init monitor and parse arguments
    monitor_init(argc, argv);

    // init sdb, expression evaluator
    sdb_init();

    // start the core and enter main loop
    core_start();

    return monitor_exit();
}