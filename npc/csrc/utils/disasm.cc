/*
 * disasm.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-10 21:18:46
 * @modified: 2025-04-10 22:12:45
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"
#include "utils.h"
#include <capstone/capstone.h>
#include <dlfcn.h>

static size_t (*cs_disasm_dl)(csh handle, const uint8_t *code, size_t code_size,
                              uint64_t address, size_t count, cs_insn **insn);
static void (*cs_free_dl)(cs_insn *insn, size_t count);

static csh handle;

void disasm_init() {
    void *dl_handle;
    dl_handle = dlopen("csrc/tools/capstone/repo/libcapstone.so.5", RTLD_LAZY);
    Assert(dl_handle, "failed to open capstone library");

    cs_err (*cs_open_dl)(cs_arch arch, cs_mode mode, csh *handle) = nullptr;
    cs_open_dl = (decltype(cs_open_dl))dlsym(dl_handle, "cs_open");
    Assert(cs_open_dl, "failed to get cs_open");

    cs_disasm_dl = (decltype(cs_disasm_dl))dlsym(dl_handle, "cs_disasm");
    Assert(cs_disasm_dl, "failed to get cs_disasm");

    cs_free_dl = (decltype(cs_free_dl))dlsym(dl_handle, "cs_free");
    Assert(cs_free_dl, "failed to get cs_free");

    cs_arch arch = CS_ARCH_RISCV;
    cs_mode mode = CS_MODE_RISCV32;
    int     ret = cs_open_dl(arch, mode, &handle);

    Assert(ret == CS_ERR_OK, "failed to open capstone");
    LogDebug("capstone initialized");
}

void disasm(char *str, int size, uint64_t pc, uint8_t *code, int nbyte) {
    cs_insn *insn;
    size_t   count = cs_disasm_dl(handle, code, nbyte, pc, 0, &insn);
    Assert(count > 0, "failed to disasm");
    int ret = snprintf(str, size, "%s", insn->mnemonic);
    if (insn->op_str[0] != '\0') {
        ret += snprintf(str + ret, size - ret, " %s", insn->op_str);
    }
    cs_free_dl(insn, count);
}