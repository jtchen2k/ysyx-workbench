/*
 * Top.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:15:33
 * @modified: 2025-04-12 23:16:56
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._

class Top extends Module {
  val io = IO(new Bundle {
    val inst = Input(UInt(32.W))
    val pc   = Output(UInt(32.W))
    val regs = Output(Vec(32, UInt(32.W)))
  })

  val pc   = RegInit("h80000000".U(32.W))
  val snpc = Wire(UInt(32.W))
  pc := Mux(reset.asBool, "h80000000".U, snpc)

  io.pc := pc

  val idu = Module(new IDU)
  val rf  = Module(new RF)
  val exu = Module(new EXU)
  val ifu = Module(new IFU)

  ifu.io.inst_in := io.inst
  ifu.io.pc      := pc

  idu.io.inst := ifu.io.inst

  exu.io.rs1    := idu.io.rs1
  exu.io.rs2    := idu.io.rs2
  exu.io.rd     := idu.io.rd
  exu.io.func3  := idu.io.func3
  exu.io.func7  := idu.io.func7
  exu.io.imm    := idu.io.imm
  exu.io.opcode := idu.io.opcode
  exu.io.itype  := idu.io.itype
  exu.io.pc     := pc
  snpc          := exu.io.snpc

  exu.io.rdata1 := rf.io.rdata1
  exu.io.rdata2 := rf.io.rdata2

  rf.io.wdata := exu.io.wdata
  rf.io.wen   := exu.io.wen

  rf.io.waddr  := idu.io.rd
  rf.io.raddr1 := idu.io.rs1
  rf.io.raddr2 := idu.io.rs2

  io.regs := rf.io._regs
}
