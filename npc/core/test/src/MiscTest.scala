/*
 * MiscTest.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-06 15:27:28
 * @modified: 2025-02-06 21:00:45
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

package top.misc

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

import org.scalatest._

class BasicTest extends AnyFlatSpec with ChiselScalatestTester {
  behavior of "Misc"

  it should "add two numbers" in {

    test(new Misc) { c =>

      def stat = {
        val inReady = c.io.in.ready.peek().litValue
        val inValid = c.io.in.valid.peek().litValue
        val inBits = c.io.in.bits.peek().litValue
        val outReady = c.io.out.ready.peek().litValue
        val outValid = c.io.out.valid.peek().litValue
        val outBits = c.io.out.bits.peek().litValue
        info(s"in: in.ready=$inReady, in.valid=$inValid, in.bits=$inBits")
        info(s"out: out.ready=$outReady, out.valid=$outValid, out.bits=$outBits")
      }

      c.io.in.valid.poke(true.B)
      c.io.out.ready.poke(false.B)
      c.io.in.bits.poke(40.U)
      stat
      c.clock.step(1)

      c.io.in.valid.poke(true.B)
      c.io.in.bits.poke(41.U)
      stat
      c.clock.step(1)

      c.io.in.valid.poke(true.B)
      c.io.in.bits.poke(42.U)
      c.io.out.ready.poke(true.B)
      stat
      c.clock.step(1)
      stat
      c.clock.step(1)
      stat

    }
  }

}