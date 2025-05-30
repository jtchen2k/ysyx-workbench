/*
 * InstType.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-06 22:05:45
 * @modified: 2025-04-13 15:50:30
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top.util

import chisel3._

object IFmt extends ChiselEnum {
    // X stands for invalid instruction.
    val R, I, S, B, U, J, X = Value
}