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

#include "utils.h"
#include <isa.h>

/*
  * Trigger an interrupt/exception with ``NO''.
  * Then return the address of the interrupt/exception vector.
  */
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  cpu.mepc = epc;
  switch (NO) {
    case -1:
      cpu.mcause = 0x0000000b;
      break;
    default:
      cpu.mcause = NO;
      break;
  }
#ifdef CONFIG_ETRACE
  log_write("itr [" FMT_WORD "]: " FMT_WORD "\n", epc, NO);
#endif
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
