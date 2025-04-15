/*
 * DPI_IFU.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-11 14:43:11
 * @modified: 2025-04-15 00:46:19
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

package top
import chisel3._
import chisel3.util.HasBlackBoxInline

class DPI_IFU extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val eifetch = Input(Bool())
    val inst    = Input(UInt(32.W))
    val pc      = Input(UInt(32.W))
    val reset   = Input(Reset())
    val clock   = Input(Clock())
  })

  setInline(
    "DPI_IFU.sv",
    """

module DPI_IFU(
    input eifetch,
    input [31:0] inst,
    input [31:0] pc,
    input reset,
    input clock
);

import "DPI-C" context function void dpi_ifetch(input int inst, input int pc);

always @(negedge clock) begin
    if (!reset) begin
        if (eifetch) begin
            dpi_ifetch(inst, pc);
        end
    end
end
endmodule
    """.stripMargin
  )
}
