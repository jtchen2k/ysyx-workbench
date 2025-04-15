/*
 * EXU.scala
 *
 * Execution Unit
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-02-01 20:10:34
 * @modified: 2025-04-15 15:07:15
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
package top

import chisel3._
import top.util._
import chisel3.util.MuxLookup
import chisel3.util.Mux1H
import chisel3.util.MuxCase
import chisel3.util.Cat
import chisel3.util.Fill

class EXU extends Module {

  val io = IO(new Bundle {
    val rs1    = Input(UInt(5.W))
    val rs2    = Input(UInt(5.W))
    val rd     = Input(UInt(5.W))
    val opcode = Input(UInt(7.W))
    val imm    = Input(UInt(32.W))
    val func3  = Input(UInt(3.W))
    val func7  = Input(UInt(7.W))
    val ifmt   = Input(IFmt())
    val pc     = Input(UInt(32.W))

    // Register value read from RF
    val rdata1 = Input(UInt(32.W))
    val rdata2 = Input(UInt(32.W))

    // Register write enable and data
    val wen   = Output(Bool())
    val wdata = Output(UInt(32.W))

    // Static next pc
    val nextpc = Output(UInt(32.W))
  })

  // io.raddr2 := io.rs1
  // io.raddr1 := io.rs2

  val rs1r = io.rdata1
  val rs2r = io.rdata2

  val rdval  = Wire(UInt(32.W))
  val nextpc = Wire(UInt(32.W))
  val aluval = Wire(UInt(32.W))

  val dpi_exu = Module(new DPI_EXU)
  val dpi_mem = Module(new DPI_MEM)
  val alu     = Module(new ALU)

  dpi_exu.io.reset := reset
  dpi_exu.io.clock := clock
  dpi_mem.io.reset := reset
  dpi_mem.io.clock := clock

  alu.io.in1 := rs1r
  alu.io.in2 := Mux(io.ifmt === IFmt.I || io.ifmt === IFmt.S, io.imm, rs2r)

  alu.io.sext := false.B // really needed?
  aluval      := alu.io.out

  dpi_exu.io.ebreaken := Mux(io.opcode === "b1110011".U && io.imm === 0x1.U, 1.B, 0.B)
  dpi_exu.io.ecallen  := Mux(io.opcode === "b1110011".U && io.imm === 0x0.U, 1.B, 0.B)

  val mrdata   = Wire(UInt(32.W))
  val mwdata   = Wire(UInt(32.W))
  val mroffset = aluval & 3.U
  val mwoffset = aluval & 3.U

  // load data dpi
  dpi_mem.io.raddr := aluval & "hfffffffc".U                                   // 4-byte aligned
  dpi_mem.io.valid := io.opcode === "b0000011".U || io.opcode === "b0100011".U // load or store
  dpi_mem.io.wen   := io.opcode === "b0100011".U                               // store

  // S-type, Store instructions
  dpi_mem.io.waddr := aluval & "hfffffffc".U // 4-byte aligned
  dpi_mem.io.wmask := MuxLookup(io.func3, 0.U)(
    Seq(
      0x0.U -> MuxLookup(mwoffset, 0.U)(
        Seq(
          0.U -> "b0001".U,
          1.U -> "b0010".U,
          2.U -> "b0100".U,
          3.U -> "b1000".U
        )
      ), // sb - byte
      0x1.U -> MuxLookup(mwoffset, 0.U)(
        Seq(
          0.U -> "b0011".U,
          2.U -> "b1100".U
        )
      ), // sh - half word
      0x2.U -> "b1111".U // sw - word
    )
  )
  dpi_mem.io.wdata := mwdata

  mwdata := rs2r << ((mwoffset * 8.U)(5, 0))
  mrdata := dpi_mem.io.rdata >> (mroffset * 8.U)(5, 0)

  // new rd value
  rdval := MuxLookup(io.opcode, 0.U)(
    Seq(
      // R-type, Register-Register Arithmetic Instructions
      "b0110011".U -> aluval,

      // I-type, Register-Immediate Arithmetic Instructions
      "b0010011".U -> aluval,

      // I-type, Load instructions
      "b0000011".U -> MuxLookup(io.func3, mrdata)(
        Seq(
          0x0.U -> Cat(Fill(24, mrdata(7)), mrdata(7, 0)),   // lb - sign extend
          0x1.U -> Cat(Fill(16, mrdata(15)), mrdata(15, 0)), // lh - sign extend
          0x2.U -> mrdata,                                   // lw
          0x4.U -> Cat(0.U(24.W), mrdata(7, 0)),             // lbu - sign extend offset, then zero ext
          0x5.U -> Cat(0.U(16.W), mrdata(15, 0))             // lhu - sign extend offset, then zero ext
        )
      ),

      // J/I-type, Jump Instructions
      "b1101111".U -> (io.pc + 4.U), // jal
      "b1100111".U -> (io.pc + 4.U), // jalr

      // U-type, Upper Load Immediate Instructions
      "b0010111".U -> (io.pc + io.imm), // auipc
      "b0110111".U -> io.imm            // lui
    )
  )

  alu.io.aluop := MuxCase(
    AluOp.NOOP,
    Seq(
      (io.opcode === "b0000011".U)               -> AluOp.ADD, // Lod
      (io.opcode === "b0100011".U)               -> AluOp.ADD, // Store
      (io.ifmt === IFmt.I || io.ifmt === IFmt.R) -> MuxLookup(io.func3, AluOp.NOOP)(
        Seq(
          0x0.U -> MuxCase(
            AluOp.ADD,
            Seq(
              (io.ifmt === IFmt.I, AluOp.ADD),
              (io.func7 === 0x20.U, AluOp.SUB)
            )
          ),
          0x4.U -> AluOp.XOR,
          0x6.U -> AluOp.OR,
          0x7.U -> AluOp.AND,
          0x1.U -> AluOp.SLL,
          0x5.U -> MuxLookup(io.func7, AluOp.NOOP)(
            Seq(
              0x00.U -> AluOp.SRL,
              0x20.U -> AluOp.SRA
            )
          ),
          0x2.U -> AluOp.LT,
          0x3.U -> AluOp.LTU
        )
      ),
      (io.ifmt === IFmt.B)                       -> MuxLookup(io.func3, AluOp.NOOP)(
        Seq(
          0x0.U -> AluOp.EQ,
          0x1.U -> AluOp.NE,
          0x4.U -> AluOp.LT,
          0x5.U -> AluOp.GE,
          0x6.U -> AluOp.LTU,
          0x7.U -> AluOp.GEU
        )
      )
    )
  )

  // will update rd when ifmt == I, J, U, R
  io.wen   := (io.ifmt === IFmt.I || io.ifmt === IFmt.J || io.ifmt === IFmt.U || io.ifmt === IFmt.R)
  io.wdata := rdval

  // update pc (defaults to pc + 4)
  nextpc    := MuxLookup(io.opcode, io.pc + 4.U)(
    Seq(
      // J-type, Jump Instructions
      "b1101111".U -> (io.pc + io.imm), // jal
      "b1100111".U -> (rs1r + io.imm),  // jalr

      // B-type Branch Instructions
      "b1100011".U -> Mux(aluval === 0.U, io.pc + 4.U, io.pc + io.imm),

      // ecall / ebreak
      "b1110011".U -> (io.pc)
    )
  )
  io.nextpc := nextpc
}
