
def SebosInstrInfo : InstrInfo;

class SebosInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : Instruction {
  let Namespace = "Sebos";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Defs = [PC_H, PC_L, MAR_H, MAR_L, IR];
  let Uses = [PC_H, PC_L];

  let Pattern = pattern;
  field bits<8> Inst;
}

class SebosOperandlessInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs), (ins), asmstr, pattern> {
  let Inst{7-0} = opcode;
}

class SebosALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosOperandlessInst<asmstr, opcode, pattern>{
  let Defs = !listconcat(SebosInst.Defs, [A, FLAGS]);
  let Uses = !listconcat(SebosInst.Uses, [A]);
}

class SebosUnaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern>;

class SebosBinaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern> {
  let Uses = !listconcat(SebosALUInst.Uses, [TMP]);
}

class SebosBranchInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs), (ins addr16:$addr), asmstr, pattern>{
  let Defs = !listconcat(SebosInst.Defs, [BUF, FLAGS]);

  field bits<24> Inst;
  let Inst{23-16} = opcode;
  let Inst{15-0} = addr;
}

class SebosMovInst<DIRECTLY_WRITABLE:$dst, READABLE:$src, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs $dst), (ins $src), "mov $dst, $src", pattern> {
  let Defs = !listconcat(SebosInst.Defs, [$dst]);
  let Uses = !listconcat(SebosInst.Uses, [$src]);

  let Inst{7-0} = opcode;
}

def imm8 : Operand<i8>;
def addr16 : Operand<i16>;

// ==================================================================================

// TODO: add patterns

def ADD : SebosBinaryALUInst<"add", 0b11000011>;
def ADDC : SebosBinaryALUInst<"addc", 0b00100100> {
  let Uses = !listconcat(SebosALUInst.Uses, [FLAGS]);
}
def SUB : SebosBinaryALUInst<"sub", 0b11000101>;
def SUBC : SebosBinaryALUInst<"subc", 0b00100101> {
  let Uses = !listconcat(SebosALUInst.Uses, [FLAGS]);
}
def AND : SebosBinaryALUInst<"and", 0b11100010>;
def OR : SebosBinaryALUInst<"or", 0b11100011>;
def XOR : SebosBinaryALUInst<"xor", 0b11110011>;
def NOT : SebosUnaryALUInst<"not", 0b11100100>;
def SHL : SebosUnaryALUInst<"shl", 0b11100100> {
  // no TMP in Uses as TMP is overwritten in the microinstructions, so the old TMP isn't actually being used
  let Defs = !listconcat(SebosALUInst.Defs, [TMP]);
}
def SLR : SebosUnaryALUInst<"slr", 0b11000100>;
def SAR : SebosUnaryALUInst<"sar", 0b11110000>;
def ROR : SebosUnaryALUInst<"ror", 0b11110100>;
def ROL : SebosUnaryALUInst<"rol", 0b11110001>;

def INCX : SebosOperandlessInst<"incx", 0b01100100> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [X]);
  let Uses = !listconcat(SebosOperandlessInst.Uses, [X]);
}
def INCY : SebosOperandlessInst<"incy", 0b01100110> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [Y]);
  let Uses = !listconcat(SebosOperandlessInst.Uses, [Y]);
}
def DECX : SebosOperandlessInst<"decx", 0b01100101> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [X]);
  let Uses = !listconcat(SebosOperandlessInst.Uses, [X]);
}
def DECY : SebosOperandlessInst<"decy", 0b01100111> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [Y]);
  let Uses = !listconcat(SebosOperandlessInst.Uses, [Y]);
}

def S7SDSM : SebosOperandlessInst<"s7sdsm", 0b11000010> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [FLAGS]);
}
def S7SDUM : SebosOperandlessInst<"s7sdum", 0b00000011> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [FLAGS]);
}

def NOP : SebosOperandlessInst<"nop", 0b00000000>;
def HLT : SebosOperandlessInst<"hlt", 0b00000001>;

def RET : SebosOperandlessInst<"ret", 0b00000111> {
  let Defs = !listconcat(SebosOperandlessInst.Defs, [SP_H, SP_L, BUF]);
  let Uses = !listconcat(SebosOperandlessInst.Uses, [SP_H, SP_L]);
}

def JMPR : SebosOperandlessInst<"jmpr", 0b00000110> {
  let Uses = !listconcat(SebosOperandlessInst.Uses, [XY]);
}

def JMP : SebosInst<(outs), (ins addr16:$addr), "jmp $addr"> {
  let Defs = !listconcat(SebosInst.Defs, [BUF]);

  field bits<24> Inst;
  let Inst{23-16} = 0b00000101;
  let Inst{15-0} = addr;
}
def JMPIND : SebosInst<(outs), (ins addr16:$addr), "jmpind $addr"> {
  let Defs = !listconcat(SebosInst.Defs, [A, BUF, TMP, FLAGS]);

  field bits<24> Inst;
  let Inst{23-16} = 0b00000101;
  let Inst{15-0} = addr;
}

def BEQ : SebosBranchInst<"beq $addr", 0b00001010>;
def BNE : SebosBranchInst<"bne $addr", 0b00001011>;
def BLT : SebosBranchInst<"blt $addr", 010010110>;
def BLTU : SebosBranchInst<"bltu $addr", 0b00100010>;
def BLE : SebosBranchInst<"ble $addr", 0b10100110>;
def BLEU : SebosBranchInst<"bleu $addr", 0b11000000>;
def BGE : SebosBranchInst<"bge $addr", 0b10010111>;
def BGEU : SebosBranchInst<"bgeu $addr", 0b00100011>;
def BGT : SebosBranchInst<"bgt $addr", 0b10100111>;
def BGTU : SebosBranchInst<"bgtu $addr", 0b11000001>;
def BZS : SebosBranchInst<"bzs $addr", 0b00001000>;
def BZC : SebosBranchInst<"bzc $addr", 0b00001001>;
def BCS : SebosBranchInst<"bcs $addr", 0b00100000>;
def BCC : SebosBranchInst<"bcc $addr", 0b00100001>;
def BNS : SebosBranchInst<"bns $addr", 0b11110101>;
def BNC : SebosBranchInst<"bnc $addr", 0b11110110>;
def BVS : SebosBranchInst<"bvs $addr", 0b01100000>;
def BVC : SebosBranchInst<"bvc $addr", 0b01100001>;
def BRXRDYS : SebosBranchInst<"brxrdys $addr", 0b11100000>;
def BRXRDYC : SebosBranchInst<"brxrdyc $addr", 0b11100001>;
def BTXRDYS : SebosBranchInst<"btxrdys $addr", 0b11101000>;
def BTXRDYC : SebosBranchInst<"btxrdyc $addr", 0b11101001>;

def MOVATMP : SebosMovInst<ATMP, 0b10011000>;
def MOVAB : SebosMovInst<AB, 0b10011001>;
def MOVAC : SebosMovInst<AC, 0b10011010>;
def MOVAX : SebosMovInst<AX, 0b10011011>;
def MOVAY : SebosMovInst<AY, 0b10011100>;
def MOVTMPA : SebosMovInst<TMPA, 0b10101000>;
def MOVTMPB : SebosMovInst<TMPB, 0b10101001>;
def MOVTMPC : SebosMovInst<TMPC, 0b10101010>;
def MOVTMPX : SebosMovInst<TMPX, 0b10101011>;
def MOVTMPY : SebosMovInst<TMPY, 0b10101100>;
def MOVBA : SebosMovInst<BA, 0b10111000>;
def MOVBTMP : SebosMovInst<BTMP, 0b10111001>;
def MOVBC : SebosMovInst<BC, 0b10111010>;
def MOVBX : SebosMovInst<BX, 0b10111011>;
def MOVBY : SebosMovInst<BY, 0b10111100>;
def MOVCA : SebosMovInst<CA, 0b11001000>;
def MOVCTMP : SebosMovInst<CTMP, 0b11001001>;
def MOVCB : SebosMovInst<CB, 0b11001010>;
def MOVCX : SebosMovInst<CX, 0b11001011>;
def MOVCY : SebosMovInst<CY, 0b11001100>;
def MOVXA : SebosMovInst<XA, 0b11011000>;
def MOVXTMP : SebosMovInst<XTMP, 0b11011001>;
def MOVXB : SebosMovInst<XB, 0b11011010>;
def MOVXC : SebosMovInst<XC, 0b11011011>;
def MOVXY : SebosMovInst<XY, 0b11011100>;
def MOVYA : SebosMovInst<YA, 0b10011101>;
def MOVYTMP : SebosMovInst<YTMP, 0b10101101>;
def MOVYB : SebosMovInst<YB, 0b10111101>;
def MOVYC : SebosMovInst<YC, 0b11001101>;
def MOVYX : SebosMovInst<YX, 0b11011101>;
def MOVABUF : SebosMovInst<ABUF, 0b10101111>;
def MOVB : SebosMovInst<B, 0b10111111>;
def MOVFA : SebosMovInst<FA, 0b10011110>;
def MOVFTMP : SebosMovInst<FTMP, 0b10101110>;
def MOVFB : SebosMovInst<FB, 0b10111110>;
def MOVFC : SebosMovInst<FC, 0b11001110>;
def MOVFX : SebosMovInst<FX, 0b11011110>;
def MOVFY : SebosMovInst<FY, 0b11101110>;