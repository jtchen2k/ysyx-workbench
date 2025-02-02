/*
 * RF.scala
 *
 * Register File
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:31:05
 * @modified: 2025-02-01 22:32:20
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.Mux1H

/** Register File
  *
  * @param aw:
  *   address width
  * @param dw:
  *   data width
  */
class RF(val aw: Int = 5, val dw: Int = 32) extends Module {
  val io = IO(new Bundle {
    val wdata  = Input(UInt(dw.W))
    val raddr1 = Input(UInt(aw.W))
    val raddr2 = Input(UInt(aw.W))
    val waddr  = Input(UInt(aw.W))
    val wen    = Input(Bool()) // write enable
    val ren1   = Input(Bool()) // read enable 1
    val ren2   = Input(Bool()) // read enable 2

    val rdata1 = Output(UInt(dw.W))
    val rdata2 = Output(UInt(dw.W))
  })

  val regs = Mem(1 << aw, UInt(dw.W))

  when(io.wen) {
    regs(io.waddr) := io.wdata
  }

  io.rdata1 := Mux1H(
    Seq(
      io.ren1 -> regs(io.raddr1),
      io.ren2 -> regs(io.raddr2)
    )
  )
  io.rdata2 := Mux1H(
    Seq(
      io.ren1 -> regs(io.raddr1),
      io.ren2 -> regs(io.raddr2)
    )
  )
}
