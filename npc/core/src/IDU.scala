/*
 * IDU.scala
 *
 * Instruction Decode Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:14:28
 * @modified: 2025-04-13 15:51:05
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util._
import top.util._

class IDU extends Module {

  val io = IO(new Bundle {
    val inst = Input(UInt(32.W))

    val rs1    = Output(UInt(5.W))
    val rs2    = Output(UInt(5.W))
    val rd     = Output(UInt(5.W))
    val imm    = Output(UInt(32.W))
    val opcode = Output(UInt(7.W))
    val func3  = Output(UInt(3.W))
    val func7  = Output(UInt(7.W))
    val valid  = Output(Bool())
    val ifmt  = Output(IFmt())
  })

  val op = io.inst(6, 0)

  io.opcode := op
  io.rs1    := io.inst(19, 15)
  io.rs2    := io.inst(24, 20)
  io.rd     := io.inst(11, 7)
  io.func3  := io.inst(14, 12)
  io.func7  := io.inst(31, 25)

  // io.tp := MuxLookup(op, InstType.X)(Seq(
  //     "b0110011".U -> InstType.R,
  //     "b0010011".U -> InstType.I,
  //     "b0000011".U -> InstType.I,
  //     "b0100011".U -> InstType.S,
  //     "b1100011".U -> InstType.B,
  //     "b1101111".U -> InstType.J,
  //     "b0110111".U -> InstType.U,
  //     "b0010111".U -> InstType.U,
  //     "b1110011".U -> InstType.I, // ecall, ebreak
  // ))

  io.ifmt := Mux1H(
    Seq(
      (op === "b0110011".U) -> IFmt.R,
      (op === "b0010011".U) -> IFmt.I,
      (op === "b0000011".U) -> IFmt.I,
      (op === "b0100011".U) -> IFmt.S,
      (op === "b1100011".U) -> IFmt.B,
      (op === "b1101111".U) -> IFmt.J,
      (op === "b0110111".U) -> IFmt.U,
      (op === "b0010111".U) -> IFmt.U,
      (op === "b1110011".U) -> IFmt.I // ecall, ebreak
    )
  )

  io.valid := io.ifmt =/= IFmt.X

  io.imm := MuxLookup(io.ifmt, 0.U)(
    Seq(
      IFmt.R -> 0.U,
      IFmt.I -> Cat(Fill(20, io.inst(31)), io.inst(31, 20)),

      // these imms are not tested!
      IFmt.U -> Cat(io.inst(31, 12), 0.U(12.W)),
      IFmt.J -> Cat(Fill(12, io.inst(31)), io.inst(19, 12), io.inst(20), io.inst(30, 21), 0.U(1.W)),
      IFmt.S -> Cat(Fill(20, io.inst(31)), io.inst(31, 25), io.inst(11, 7)),
      IFmt.B -> Cat(Fill(19, io.inst(31)), io.inst(31), io.inst(7), io.inst(30, 25), io.inst(11, 8), 0.U(1.W))
    )
  )
}
