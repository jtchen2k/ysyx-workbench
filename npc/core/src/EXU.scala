/*
 * EXU.scala
 *
 * Execution Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:10:34
 * @modified: 2025-02-15 03:52:31
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import top.util._
import chisel3.util.MuxLookup
import chisel3.util.Mux1H

class EXU extends Module {

    val io = IO(new Bundle {
        val rs1 = Input(UInt(5.W))
        val rs2 = Input(UInt(5.W))
        val rd = Input(UInt(5.W))
        val opcode = Input(UInt(7.W))
        val imm = Input(UInt(32.W))
        val func3 = Input(UInt(3.W))
        val func7 = Input(UInt(7.W))
        val itype = Input(InstType())

        // register read data
        val rdata1 = Input(UInt(32.W))
        val rdata2 = Input(UInt(32.W))
        // register write data
        val wen = Output(Bool())
        val wdata = Output(UInt(32.W))
    })

    // io.raddr2 := io.rs1
    // io.raddr1 := io.rs2
    io.wen := false.B
    io.wdata := 0.U

    val rs1r = io.rdata1
    val rs2r = io.rdata2

    val res = Wire(UInt(32.W))
    val dpi = Module(new DPI)

    dpi.io.reset := reset
    dpi.io.clock := clock
    dpi.io.ebreaken := Mux(io.opcode === "b1110011".U && io.imm === 0x1.U, true.B, false.B)
    dpi.io.ecallen := Mux(io.opcode === "b1110011".U && io.imm === 0x0.U, true.B, false.B)

    res := MuxLookup(io.opcode, 0.U)(Seq(
        // I-type, Register-Immediate Arithmetic Instructions
        "b0010011".U -> MuxLookup(io.func3, 0x0.U)(Seq(
            0x0.U -> (rs1r + io.imm), // addi
            0x4.U -> (rs1r ^ io.imm), // xori
            0x6.U -> (rs1r | io.imm), // ori
            0x7.U -> (rs1r & io.imm), // andi
            0x1.U -> (rs1r << io.imm(4, 0)), // slli (logical)
            0x5.U -> MuxLookup(io.imm(11, 5), 0.U)(Seq(
                0x00.U -> (rs1r >> io.imm(4, 0)), // srli (logical)
                0x20.U -> (rs1r.asSInt >> io.imm(4, 0)).asUInt // srai (arithmetic)
            )),
            0x2.U -> Mux(rs1r.asSInt < io.imm.asSInt, 1.U, 0.U), // slti (signed)
            0x3.U -> Mux(rs1r < io.imm, 1.U, 0.U), // sltiu (unsigned)
        )),
        "b1110011".U -> dpi.io.out
    ))

    io.wen := Mux(io.itype === InstType.I, true.B, false.B)
    io.wdata := res
}