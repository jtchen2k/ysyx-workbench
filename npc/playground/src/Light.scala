/*
 * Light.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 17:22:05
 * @modified: 2025-02-01 18:03:42
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

package ysyx

import chisel3._
import chisel3.util.Cat

class Light extends Module {
  val io = IO(new Bundle {
    val led = Output(UInt(16.W))
  })

  val count = RegInit(0.U(32.W))
  val led   = RegInit(1.U(16.W))

  when(reset.asBool) {
    count := 0.U
  }.otherwise {
    when(count === 0.U) {
      led := Cat(led(14, 0), led(15))
    }
    count := Mux(count >= 5e6.toLong.U, 0.U, count + 1.U)
  }

  io.led := led
}
