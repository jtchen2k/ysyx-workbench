/*
 * IFU.scala
 *
 * Instruction Fetch Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 19:43:01
 * @modified: 2025-02-07 21:30:28
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._

class IFU extends Module {
    val io = IO(new Bundle {
        val pc = Input(UInt(32.W))
        val inst = Output(UInt(32.W))
    })

}