/*
 * Top.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:15:33
 * @modified: 2025-02-01 22:53:01
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._

class Top extends Module {
    val io = IO(new Bundle {
        val inst = Input(UInt(32.W))
        val pc = Output(UInt(32.W))
    })

    val pc = RegInit("h80000000".U(32.W))
    io.pc := pc

    val rf = Module(new RF)

    rf.io.wdata := 0.U
    rf.io.waddr := 0.U
    rf.io.wen := false.B
    rf.io.raddr1 := 0.U
    rf.io.raddr2 := 0.U
    rf.io.ren1 := false.B
    rf.io.ren2 := false.B
}