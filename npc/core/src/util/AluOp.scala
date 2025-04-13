package top.util

import chisel3._

object AluOp extends ChiselEnum {
  val ADD  = Value(0.U)
  val SUB  = Value(1.U)
  val XOR  = Value(2.U)
  val OR   = Value(3.U)
  val AND  = Value(4.U)
  val SLL  = Value(5.U)  // shift left logical
  val SRL  = Value(6.U)  // shift right logical
  val SRA  = Value(7.U)  // shift right arithmetic
  val LT   = Value(8.U)  // set less than
  val LTU  = Value(9.U)  // set less than unsigned
  val EQ   = Value(10.U) // equal
  val NE   = Value(11.U) // not equal
  val GE   = Value(12.U) // greater than or equal
  val GEU  = Value(13.U) // greater than or equal unsigned
  val NOOP = Value(0xf.U) // no op, returning 0.
}
