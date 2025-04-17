object Elaborate extends App {
  val firtoolOptions = Array(
    "--lowering-options=" + List(
      // make yosys happy
      // see https://github.com/llvm/circt/blob/main/docs/VerilogGeneration.md
      "disallowLocalVariables",
      "disallowPackedArrays",
      "locationInfoStyle=wrapInAtSquareBracket"
    ).reduce(_ + "," + _)
  )
  val arg = Array(
    // to make DPI work
    // see https://github.com/chipsalliance/chisel/issues/3933#issuecomment-2000035088
    "--split-verilog",
  )
  circt.stage.ChiselStage.emitSystemVerilogFile(new top.ALU(dw = 5), args ++ arg, firtoolOptions)
}
