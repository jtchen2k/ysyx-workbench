/*
 * IFU.scala
 *
 * Instruction Fetch Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:43:01
 * @modified: 2025-04-13 14:20:00
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._

class IFU extends Module {
  val io = IO(new Bundle {
    val pc   = Input(UInt(32.W))
    // val inst_in = Input(UInt(32.W))
    val inst = Output(UInt(32.W))
  })

  val dpi_ifetch = Module(new DPI_IFU)
  val dpi_mem    = Module(new DPI_MEM)

  val inst = Wire(UInt(32.W))

  dpi_mem.io.valid := 1.B
  dpi_mem.io.raddr := io.pc
  dpi_mem.io.waddr := 0.U
  dpi_mem.io.wdata := 0.U
  dpi_mem.io.wmask := 0.U(8.W)
  dpi_mem.io.wen   := 0.B
  inst             := dpi_mem.io.rdata

  dpi_mem.io.reset := reset
  dpi_mem.io.clock := clock

  dpi_ifetch.io.reset   := reset
  dpi_ifetch.io.clock   := clock
  dpi_ifetch.io.eifetch := 1.B
  dpi_ifetch.io.inst    := inst
  io.inst               := inst
}
