/*
 * RF.scala
 *
 * Register File
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:31:05
 * @modified: 2025-04-14 12:39:17
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.Mux1H

/** Register File. x0 is hardwired to 0.
  *
  * @param aw: address width
  * @param dw: data width
  */
class RF(val aw: Int = 5, val dw: Int = 32) extends Module {
  val io = IO(new Bundle {
    val wdata  = Input(UInt(dw.W))
    val raddr1 = Input(UInt(aw.W))
    val raddr2 = Input(UInt(aw.W))
    val waddr  = Input(UInt(aw.W))
    val wen    = Input(Bool()) // write enable for registers

    val rdata1 = Output(UInt(dw.W))
    val rdata2 = Output(UInt(dw.W))

    // wired to the top for debugging purposes
    val _regs = Output(Vec(1 << aw, UInt(dw.W)))
  })

  val regs = RegInit(VecInit(Seq.fill(1 << aw)(0.U(dw.W))))
  io._regs := regs

  regs(io.waddr) := Mux(io.wen && io.waddr =/= 0.U, io.wdata, regs(io.waddr))

  io.rdata1 := Mux(io.raddr1 === 0.U, 0.U, regs(io.raddr1))
  io.rdata2 := Mux(io.raddr2 === 0.U, 0.U, regs(io.raddr2))
}
