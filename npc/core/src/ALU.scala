/*
 * ALU.scala
 *
 * Arithmetic Logic Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:09:51
 * @modified: 2025-04-13 16:08:33
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.MuxLookup
import top.util.AluOp

class ALU extends Module {

  val io = IO(new Bundle {
    val in1   = Input(UInt(32.W))
    val in2   = Input(UInt(32.W))
    val aluop = Input(AluOp())
    val out   = Output(UInt(32.W))
    val sext  = Input(Bool())
  })

  io.out := MuxLookup(io.aluop, 0.U)(
    Seq(
      AluOp.ADD -> (io.in1 + io.in2),
      AluOp.SUB -> (io.in1 - io.in2),
      AluOp.AND -> (io.in1 & io.in2),
      AluOp.OR  -> (io.in1 | io.in2),
      AluOp.XOR -> (io.in1 ^ io.in2),
      AluOp.SLL -> (io.in1 << io.in2(4, 0)),
      AluOp.SRL -> (io.in1 >> io.in2(4, 0)),
      AluOp.SRA -> (io.in1.asSInt >> io.in2(4, 0)).asUInt,
      AluOp.LT  -> (io.in1.asSInt < io.in2.asSInt),
      AluOp.LTU -> (io.in1 < io.in2),
      AluOp.EQ  -> (io.in1 === io.in2),
      AluOp.NE  -> (io.in1 =/= io.in2),
      AluOp.GE  -> (io.in1.asSInt >= io.in2.asSInt),
      AluOp.GEU -> (io.in1 >= io.in2)
    )
  )
}
