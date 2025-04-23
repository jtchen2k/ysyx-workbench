/*
 * DPI_MEM.scala
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-12 23:18:36
 * @modified: 2025-04-23 21:20:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import chisel3.util.HasBlackBoxInline

class DPI_MEM extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val valid    = Input(Bool())
    val wen      = Input(Bool())
    val waddr    = Input(UInt(32.W))
    val wdata    = Input(UInt(32.W))
    val wmask    = Input(UInt(8.W))
    val raddr    = Input(UInt(32.W))
    val rdata    = Output(UInt(32.W))

    val reset    = Input(Reset())
    val clock    = Input(Clock())
  })

  setInline(
    "DPI_MEM.sv",
    """
module DPI_MEM(
    input valid,
    input wen,
    input [31:0] waddr,
    input [31:0] wdata,
    input [7:0] wmask,
    input [31:0] raddr,
    output reg [31:0] rdata,

    input reset,
    input clock
);
import "DPI-C" context function int dpi_pmem_read(input int raddr);
import "DPI-C" context function void dpi_pmem_write(input int waddr, input int wdata, input byte wmask);

always @(*) begin
    if (valid && !reset) begin
        rdata = dpi_pmem_read(raddr);
    end else begin
        rdata = 0;
    end
end
always @(posedge clock) begin
    if (valid && !reset && wen) begin
        dpi_pmem_write(waddr, wdata, wmask);
    end
end
endmodule
    """.stripMargin
  )
}
