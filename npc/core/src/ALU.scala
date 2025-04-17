/*
 * ALU.scala
 *
 * Arithmetic Logic Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:09:51
 * @modified: 2025-04-17 10:32:05
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.MuxLookup
import top.util.AluOp

class ALU(dw: Int = 32) extends Module {

  val io = IO(new Bundle {
    val in1   = Input(UInt(dw.W))
    val in2   = Input(UInt(dw.W))
    val aluop = Input(AluOp())
    val out   = Output(UInt(dw.W))
    val sext  = Input(Bool())
  })

  val in1 = io.in1
  val in2 = io.in2

  io.out := MuxLookup(io.aluop, 0.U)(
    Seq(
      AluOp.ADD -> (in1 + in2),
      AluOp.SUB -> (in1 - in2),
      AluOp.AND -> (in1 & in2),
      AluOp.OR  -> (in1 | in2),
      AluOp.XOR -> (in1 ^ in2),
      AluOp.SLL -> (in1 << in2(4, 0)),
      AluOp.SRL -> (in1 >> in2(4, 0)),
      AluOp.SRA -> ((in1.asSInt >> in2(4, 0)).asUInt),
      AluOp.LT  -> (in1.asSInt < in2.asSInt),
      AluOp.LTU -> (in1 < in2),
      AluOp.EQ  -> (in1 === in2),
      AluOp.NE  -> (in1 =/= in2),
      AluOp.GE  -> (in1.asSInt >= in2.asSInt),
      AluOp.GEU -> (in1 >= in2),
    )
  )
}
