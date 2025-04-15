/*
 * DPI.scala (deprecated)
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-14 17:49:32
 * @modified: 2025-04-14 00:10:35
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.HasBlackBoxInline

class DPI extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val ebreaken = Input(Bool())
    val ecallen  = Input(Bool())
    val eifetch  = Input(Bool())
    val inst     = Input(UInt(32.W))
    val out      = Output(UInt(32.W))
    val reset    = Input(Reset())
    val clock    = Input(Clock())
  })

  setInline(
    "DPI.sv",
    """
import "DPI-C" context function void dpi_ebreak();
import "DPI-C" context function void dpi_ecall();
import "DPI-C" context function void dpi_ifetch(word_t inst);

module DPI(
    input ebreaken,
    input ecallen,
    input eifetch,
    input [31:0] inst,
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
        if (eifetch) begin
            dpi_ifetch(inst);
            out <= 1;
        end
    end
end
endmodule
    """.stripMargin
  )
}
