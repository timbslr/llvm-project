
def SebosInstrInfo : InstrInfo;

class SebosInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : Instruction {
  let Namespace = "Sebos";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Defs = [IR, PC_H, PC_L];
  let Uses = [PC_H, PC_L];

  let Pattern = pattern;
  field bits<8> Inst;
}

class SebosALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs), (ins), asmstr, pattern>{
  let Defs = !listconcat(SebosInst.Defs, [A, FLAGS]);
  let Uses = !listconcat(SebosInst.Uses, [A]);

  let Inst{7-0} = opcode;
}

class SebosUnaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern>;

class SebosBinaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern> {
  let Uses = !listconcat(SebosALUInst.Uses, [TMP]);
};

def imm8 : Operand<i8>;
def addr16 : Operand<i16>;

// ==================================================================================

// TODO: add patterns

def ADD : SebosBinaryALUInst<"add", 0b11000011>;
def ADDC : SebosBinaryALUInst<"addc", 0b00100100> {
  let Uses = !listconcat(SebosALUInst.Uses, [FLAGS]);
};
def SUB : SebosBinaryALUInst<"sub", 0b11000101>;
def SUBC : SebosBinaryALUInst<"subc", 0b00100101> {
  let Uses = !listconcat(SebosALUInst.Uses, [FLAGS]);
};
def AND : SebosBinaryALUInst<"and", 0b11100010>;
def OR : SebosBinaryALUInst<"or", 0b11100011>;
def XOR : SebosBinaryALUInst<"xor", 0b11110011>;
def NOT : SebosUnaryALUInst<"not", 0b11100100>;
def SHL : SebosUnaryALUInst<"shl", 0b11100100> {
  // no TMP in Uses as TMP is overwritten in the microinstructions, so the old TMP isn't actually being used
  let Defs = !listconcat(SebosALUInst.Defs, [TMP]);
};
def SLR : SebosUnaryALUInst<"slr", 0b11000100>;
def SAR : SebosUnaryALUInst<"sar", 0b11110000>;
def ROR : SebosUnaryALUInst<"ror", 0b11110100>;
def ROL : SebosUnaryALUInst<"rol", 0b11110001>;

