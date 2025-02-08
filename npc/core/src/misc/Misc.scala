/*
 * Misc.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-06 15:26:47
 * @modified: 2025-02-06 21:01:15
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top.misc

import chisel3._
import chisel3.util.Decoupled
import chisel3.util.Queue

class Misc extends Module {
  val io = IO(new Bundle {
    val in  = Flipped(Decoupled(UInt(8.W)))
    val out = Decoupled(UInt(8.W))
  })

  val queue = Queue(io.in, 2)
  io.out <> queue
}