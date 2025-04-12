/*
 * difftest.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-11 16:27:41
 * @modified: 2025-04-12 22:51:52
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "core.h"
#include "difftest/difftest-def.h"
#include "mem.h"
#include "utils.h"
#include <dlfcn.h>

static diffcontext_t *refcontext;

#ifdef CONFIG_DIFFTEST

static void *handle = nullptr;
static bool  is_inited = false;

static ref_difftest_memcpy_t     ref_difftest_memcpy = nullptr;
static ref_difftest_regcpy_t     ref_difftest_regcpy = nullptr;
static ref_difftest_exec_t       ref_difftest_exec = nullptr;
static ref_difftest_raise_intr_t ref_difftest_raise_intr = nullptr;
static ref_difftest_init_t       ref_difftest_init = nullptr;

static void make_dut_context(diffcontext_t *dutcontext) {
    for (int i = 0; i < 32; i++) {
        dutcontext->gpr[i] = R(i);
    }
    dutcontext->pc = R(PC);
}

void difftest_init(long img_size, int port) {
    char ref_so[] = __DIFFTEST_REF_SO__;
    handle = dlopen(ref_so, RTLD_LAZY);
    if (!handle) {
        LogError("Failed to open difftest lib handle: %s\n", dlerror());
        return;
    }
    ref_difftest_regcpy = (ref_difftest_regcpy_t)dlsym(handle, "difftest_regcpy");
    Assert(ref_difftest_regcpy, "Failed to load difftest_regcpy");
    ref_difftest_exec = (ref_difftest_exec_t)dlsym(handle, "difftest_exec");
    Assert(ref_difftest_exec, "Failed to load difftest_exec");
    ref_difftest_raise_intr = (ref_difftest_raise_intr_t)dlsym(handle, "difftest_raise_intr");
    Assert(ref_difftest_raise_intr, "Failed to load difftest_raise_intr");
    ref_difftest_init = (ref_difftest_init_t)dlsym(handle, "difftest_init");
    Assert(ref_difftest_init, "Failed to load difftest_init");
    ref_difftest_memcpy = (ref_difftest_memcpy_t)dlsym(handle, "difftest_memcpy");
    Assert(ref_difftest_memcpy, "Failed to load difftest_memcpy");
    is_inited = true;

    diffcontext_t *dutcontext = (diffcontext_t *)malloc(sizeof(diffcontext_t));
    make_dut_context(dutcontext);

    ref_difftest_init(port);
    ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), img_size, DIFFTEST_TO_REF);
    ref_difftest_regcpy(dutcontext, DIFFTEST_TO_REF);
    free(dutcontext);

    // allocate refcontext
    refcontext = (diffcontext_t *)malloc(sizeof(diffcontext_t));
    LogInfo("difftest enabled (ref_so=%s), performance may degrade.", ref_so);
}

void difftest_raise_intr(uint64_t NO) {
    if (!is_inited)
        return;
    ref_difftest_raise_intr(NO);
}

void difftest_step(paddr_t pc) {
    ref_difftest_exec(1);
    // LogTrace("difftest_step: pc=" FMT_ADDR, pc);
    if (!is_inited)
        return;
    bool fail = false;
    ref_difftest_regcpy(refcontext, DIFFTEST_TO_DUT);
    for (int i = 0; i < 32; i++) {
        if (refcontext->gpr[i] != R(i)) {
            fail = true;
            char rname[10];
            reg_name(i, rname);
            LogError("difftest: %s mismatch: ref=" FMT_WORD ", dut=" FMT_WORD, rname,
                     refcontext->gpr[i], R(i));
        }
    }
    if (refcontext->pc != pc) {
        fail = true;
        LogError("difftest: pc mismatch: ref=" FMT_ADDR ", dut=" FMT_ADDR, refcontext->pc, pc);
    }
    if (fail) {
        LogError("difftest failed. stop the core.");
        g_core_context->state = CORE_STATE_STOP;
    }
}

#endif