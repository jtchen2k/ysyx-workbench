/*
 * dpi.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:27:15
 * @modified: 2025-02-15 16:53:07
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "VTop__Dpi.h"
#include "core.h"
#include "utils.h"

void dpi_ebreak() {
    LogDebug("dpi call to ebreak");
    g_core_state->state = CORE_STATE_QUIT;
}

void dpi_ecall() { LogDebug("dpi call to ecall\n"); }