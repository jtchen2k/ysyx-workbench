/*
 * main.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:14:02
 * @modified: 2025-02-15 16:53:31
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
#include "utils.h"

void nvboard_bind_all_pins(TOP_NAME *top);

int main() {
    pmem_init();
    core_init();

    Assert(g_core != nullptr, "core not initialized.");
    Assert(g_core_state != nullptr, "core state not initialized.");
    while (1) {
        switch (g_core_state->state) {
        case CORE_STATE_RUNNING:
            exec(1);
            break;
        case CORE_STATE_QUIT:
            core_shutdown();
            goto end;
        case CORE_STATE_STOP:
            Panic("core stopped.");
        }
    }

end:
    print_register();
    Assert(R(1) == 12, "x1 = %d", R(1));
    Assert(R(2) == 16, "x2 = %d", R(2));
    Assert(R(3) == 36, "x3 = %d", R(3));
    Assert(R(4) == 16, "x4 = %d", R(4));
    LogInfo("test passed.");
    return 0;
}