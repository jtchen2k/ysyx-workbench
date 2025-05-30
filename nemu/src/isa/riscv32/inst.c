/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

// clang-format off
#include "common.h"
#include "isa.h"
#include "local-include/reg.h"
#include "trace.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <stdint.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

#define CSR_MSTATUS 0x0300
#define CSR_MEPC 0x0341
#define CSR_MTVEC 0x0305
#define CSR_MCAUSE 0x0342

static word_t csr_read(word_t csr) {
  switch (csr) {
    case CSR_MSTATUS: return cpu.mstatus;
    case CSR_MEPC: return cpu.mepc;
    case CSR_MTVEC: return cpu.mtvec;
    case CSR_MCAUSE: return cpu.mcause;
    default: panic("unsupported csr read: 0x%x", csr);
  }
}

static void csr_write(word_t csr, word_t value) {
  switch (csr) {
    case CSR_MSTATUS: cpu.mstatus = value; break;
    case CSR_MEPC: cpu.mepc = value; break;
    case CSR_MTVEC: cpu.mtvec = value; break;
    case CSR_MCAUSE: cpu.mcause = value; break;
    default: panic("unsupported csr write: 0x%x", csr);
  }
}

/**
 * I = Immediate, U = Upper Immediate (vairant of I)
 * S = Store, J = Jump, R = Register, B = Branch, N = None
 */
enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_J, TYPE_R, TYPE_B,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = SEXT(BITS(i, 31, 31), 1) << 20 | BITS(i, 30, 21) << 1 | BITS(i, 20, 20) << 11 | BITS(i, 19, 12) << 12; } while(0)
#define immB() do { *imm = SEXT(BITS(i, 31, 31), 1) << 12 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1 | BITS(i, 7, 7) << 11; } while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_N: break;
    default: panic("unsupported type = %d", type);
  }
}

static int decode_exec(Decode *s) {
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  int rd = 0; \
  word_t src1 = 0, src2 = 0, imm = 0; \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);


  /// 2.4.1 Integer Register-Immediate Instructions
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << BITS(imm, 4, 0));
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = src1 >> BITS(imm, 4, 0));
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (sword_t)src1 >> BITS(imm, 4, 0));
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(rd) = (sword_t)src1 < (sword_t)imm ? 1 : 0);
  // the immediate is first sign-extended to XLEN bits then treated as an unsigned number
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = src1 < imm ? 1 : 0);

  /// 2.4.2 Integer Register-Register Operations
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = src1 >> src2);
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (sword_t)src1 >> src2);
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = (sword_t)src1 < (sword_t)src2 ? 1 : 0);  // set less than
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = src1 < src2 ? 1 : 0);  // set less than unsigned

  /// 2.5.1 Uncondictional Branches
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, {
    R(rd) = s->pc + 4;
    s->dnpc = s->pc + (sword_t)imm;
    #ifdef CONFIG_FTRACE
    if (rd != 0) ftrace_call(s);
    #endif
  });
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, {
    R(rd) = s->pc + 4;
    s->dnpc = src1 + (sword_t)imm;
    #ifdef CONFIG_FTRACE
    if (rd == 0 && imm == 0x0 && src1 == R(1)) ftrace_ret(s); // ret: jalr x0, 0(x1), where x1 = return address
    else if (rd != 0) ftrace_call(s);
    #endif
  });

  /// 2.5.2 Conditional Branches
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = src1 == src2 ? s->pc + (sword_t)imm : s->snpc);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = src1 != src2 ? s->pc + (sword_t)imm : s->snpc);
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = (sword_t)src1 < (sword_t)src2 ? s->pc + (sword_t)imm : s->snpc);
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = (sword_t)src1 >= (sword_t)src2 ? s->pc + (sword_t)imm : s->snpc);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = src1 < src2 ? s->pc + (sword_t)imm : s->snpc);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = src1 >= src2 ? s->pc + (sword_t)imm : s->snpc);

  /// 2.6 Load and Store Instructions
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd) = SEXT(Mr(src1 + imm, 1), 8));
  // LH loads a 16-bit value from memory, then sign-extends to 32-bits before storing in rd.
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr(src1 + imm, 4));
  // LHU loads a 16-bit value from memory but then zero extends to 32-bits before storing in rd
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));

  /// 13. RV32M Standard Extension
  /// h = high, s = signed, u = unsigned
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = (sword_t)src1 * (sword_t)src2);
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = ((SEXT(src1, 32) * SEXT(src2, 32)) >> 32)); // upper XLEN for signed x signed
  INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu , R, R(rd) = ((SEXT(src1, 32) * (uint64_t)src2) >> 32)); // upper XLEN for rs1 x unsigned rs2
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(rd) = (((uint64_t)src1 * (uint64_t)src2) >> 32)); // upper XLEN for unsigned x unsigned
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = (sword_t)src1 / (sword_t)src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = src1 / src2);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = (sword_t)src1 % (sword_t)src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = src1 % src2);

  /// CSR Instructions
  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw , I, R(rd) = csr_read(imm), csr_write(imm, src1));
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs , I, R(rd) = csr_read(imm), csr_write(imm, src1 | csr_read(imm)));

  /// 3.3.1 Environment Call and Breakpoints
  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, s->dnpc = isa_raise_intr(MUXDEF(__riscv_e, R(15), R(17)), s->pc));
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0

  /// Trap-Return Instructions
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret  , N, s->dnpc = csr_read(CSR_MEPC));

  /// invalid instruction
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
