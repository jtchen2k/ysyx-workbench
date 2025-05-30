/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define REG_PRINT_LINE(name, reg) printf(ANSI_FG_BLUE "%-12s " ANSI_NONE FMT_PADDR MUXDEF(CONFIG_ISA64, "%24llu\n", "%16u\n"), name, reg, reg)

void isa_reg_display() {
  for (int i = 0; i < ARRLEN(regs); i++) {
    REG_PRINT_LINE(reg_name(i), gpr(i));
  }
  REG_PRINT_LINE("pc", cpu.pc);
  REG_PRINT_LINE("mepc", cpu.mepc);
  REG_PRINT_LINE("mstatus", cpu.mstatus);
  REG_PRINT_LINE("mcause", cpu.mcause);
  REG_PRINT_LINE("mtvec", cpu.mtvec);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  if (!strncmp(s, "pc", strlen(s))) {
    *success = true;
    return cpu.pc;
  }
  for (int i = 0; i < ARRLEN(regs); i++) {
    if (!strncmp(regs[i], s, strlen(regs[i])) && (strlen(regs[i]) == strlen(s))) {
      *success = true;
      return gpr(i);
    }
  }
  *success = false;
  printf("no register named %s\n", s);
  return 0;
}
