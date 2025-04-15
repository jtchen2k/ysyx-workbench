/*
 * difftest.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-11 16:27:41
 * @modified: 2025-04-15 01:19:51
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "core.h"
#include "difftest/difftest-def.h"
#include "macro.h"
#include "mem.h"
#include "utils.h"
#include <dlfcn.h>

#ifdef CONFIG_DIFFTEST

static diffcontext_t *refcontext;

static void *handle = nullptr;
static bool  is_inited = false;

static ref_difftest_memcpy_t     ref_difftest_memcpy = nullptr;
static ref_difftest_regcpy_t     ref_difftest_regcpy = nullptr;
static ref_difftest_exec_t       ref_difftest_exec = nullptr;
static ref_difftest_raise_intr_t ref_difftest_raise_intr = nullptr;
static ref_difftest_init_t       ref_difftest_init = nullptr;

bool g_difftest_failed = false;

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
    ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), CONFIG_MSIZE, DIFFTEST_TO_REF);
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

// memory camparison. could be very slow.
static int     memcmp_size = 32 << 10; // only compare the first 32KB of the memory
static uint8_t pmem_ref[CONFIG_MSIZE] PG_ALIGN = {};
static bool    is_first_cycle = true;

void difftest_step(paddr_t pc) {

    if (!is_inited)
        return;

    // // delay nemu for one instruction
    // if (is_first_cycle) {
    //     is_first_cycle = false;
    //     return;
    // }

    ref_difftest_exec(1);

    // LogTrace("difftest_step: pc=" FMT_ADDR, pc);
    bool fail = false;

    ref_difftest_regcpy(refcontext, DIFFTEST_TO_DUT);
    for (int i = 0; i < 32; i++) {
        if (refcontext->gpr[i] != R(i)) {
            fail = true;
            char rname[10];
            reg_name(i, rname);
            LogError("%s mismatch: ref=" FMT_WORD ", dut=" FMT_WORD, rname, refcontext->gpr[i],
                     R(i));
        }
    }
    if (g_core_context->state != CORE_STATE_TERM && refcontext->pc != pc) {
        fail = true;
        LogError("pc mismatch: ref=" FMT_ADDR ", dut=" FMT_ADDR, refcontext->pc, pc);
    }

    ref_difftest_memcpy(RESET_VECTOR, pmem_ref, memcmp_size, DIFFTEST_TO_DUT);

    for (int i = 0; i < memcmp_size; i += 4) {
        uint32_t dut_data = _pmem_read_word_silent(RESET_VECTOR + i);
        uint32_t ref_data = *(uint32_t *)(pmem_ref + i);
        if (dut_data != ref_data) {
            fail = true;
            LogError("pmem mismatch at " FMT_ADDR ". dut=" FMT_WORD ", ref=" FMT_WORD,
                     RESET_VECTOR + i, dut_data, ref_data);
        }
    }

    // free(pmem_ref);

    if (fail) {
        LogError("difftest failed at pc " FMT_ADDR ".", pc);
        g_core_context->state = CORE_STATE_STOP;
        g_difftest_failed = true;
    }
}

#endif