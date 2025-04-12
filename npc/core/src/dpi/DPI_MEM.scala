/*
 * DPI_MEM.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 23:18:36
 * @modified: 2025-04-12 23:19:49
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.HasBlackBoxInline

class DPI_MEM extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val ebreaken = Input(Bool())
    val ecallen  = Input(Bool())
    val out      = Output(UInt(32.W))
    val reset    = Input(Reset())
    val clock    = Input(Clock())
  })

  setInline(
    "DPI_MEM.sv",
    """
import "DPI-C" context function int dpi_pmem_read(input int raddr);
import "DPI-C" context function void dpi_pmem_write(input int waddr, input int wdata, input byte wmask);
module DPI_MEM(
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
