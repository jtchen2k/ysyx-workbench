/*
 * DPI_EXU.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-11 14:42:12
 * @modified: 2025-04-11 15:09:50
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

package top

import chisel3._
import chisel3.util.HasBlackBoxInline

class DPI_EXU extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val ebreaken = Input(Bool())
    val ecallen  = Input(Bool())
    val out      = Output(UInt(32.W))
    val reset    = Input(Reset())
    val clock    = Input(Clock())
  })

  setInline(
    "DPI_EXU.sv",
    """
import "DPI-C" context function void dpi_ebreak();
import "DPI-C" context function void dpi_ecall();
module DPI_EXU(
    input ebreaken,
    input ecallen,
    output reg [31:0] out,
    input reset,
    input clock
);

always @(posedge clock) begin
    if (reset) begin
        out <= 0;
    end else begin
        if (ebreaken) begin
            dpi_ebreak();
            out <= 1;
        end
        if (ecallen) begin
            dpi_ecall();
            out <= 1;
        end
    end
end
endmodule
    """.stripMargin
  )
}
