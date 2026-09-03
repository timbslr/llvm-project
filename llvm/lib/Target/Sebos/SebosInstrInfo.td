
def SebosInstrInfo : InstrInfo;

class SebosInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : Instruction {
  let Namespace = "Sebos";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Defs = [PC, MAR, IR];
  let Uses = [PC];

  let Pattern = pattern;
  field bits<8> Inst;
}

class SebosOperandlessInst<string asmstr, bits<8> opcode, list<Register> additionalDefs, list<Register> additionalUses, list<dag> pattern = []> : SebosInst<(outs), (ins), asmstr, pattern> {
  let Inst{7-0} = opcode;
  let Defs = !listconcat(SebosInst.Defs, additionalDefs);
  let Uses = !listconcat(SebosInst.Uses, additionalUses);
}

class SebosSingleRegisterOperandInst<dag outs, dag ins, string asmstr, bits<5> opcode, list<Register> additionalDefs, list<Register> additionalUses, list<dag> pattern> : SebosInst<outs, ins, asmstr, pattern> {
  let Inst{7-3} = opcode;

  let Defs = !listconcat(SebosInst.Defs, additionalDefs);
  let Uses = !listconcat(SebosInst.Uses, additionalUses);
}

class SebosSingleReadRegisterOperandInst<string asmstr, bits<5> opcode, list<Register> additionalDefs, list<Register> additionalUses, list<dag> pattern = []> : SebosSingleRegisterOperandInst<(outs), (ins GPR8:$reg), asmstr, opcode, additionalDefs, additionalUses, pattern> {
  let Inst{2-0} = reg;
}

class SebosSingleWriteRegisterOperandInst<string asmstr, bits<5> opcode, list<Register> additionalDefs, list<Register> additionalUses, list<dag> pattern = []> : SebosSingleRegisterOperandInst<(outs GPR8:$reg), (ins), asmstr, opcode, additionalDefs, additionalUses, pattern> {
  let Inst{2-0} = reg;
}

class SebosSingleImmediateOperandInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs), (ins i8imm:$imm), asmstr, pattern> {
  field bits<16> Inst;
  let Inst{15-8} = opcode;
  let Inst{7-0} = imm;
}

class SebosALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosOperandlessInst<asmstr, opcode, [A, FLAGS], [A], pattern>;

class SebosUnaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern>;

class SebosBinaryALUInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosALUInst<asmstr, opcode, pattern> {
  let Uses = !listconcat(SebosALUInst.Uses, [TMP]);
}

class SebosBranchInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosInst<(outs), (ins i16addr:$addr), asmstr, pattern>{
  let Defs = !listconcat(SebosInst.Defs, [BUF]);

  field bits<24> Inst;
  let Inst{23-16} = opcode;
  let Inst{15-0} = addr;
}

class SebosCompareAndBranchInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosBranchInst<asmstr, opcode, pattern> {
  let Defs = !listconcat(SebosBranchInst.Defs, [FLAGS]);
  let Uses = !listconcat(SebosBranchInst.Uses, [A, TMP]);
}

class SebosFlagTestBranchInst<string asmstr, bits<8> opcode, list<dag> pattern = []> : SebosBranchInst<asmstr, opcode, pattern> {
  let Uses = !listconcat(SebosBranchInst.Uses, [FLAGS]);
}

class SebosMovInst<string asmstr, Register dst, Register src, bits<8> opcode, list<dag> pattern = []> : SebosOperandlessInst<asmstr, opcode, [dst], [src], pattern>;

def i8imm : Operand<i8>;
def i16addr : Operand<i16>;

// ==================================================================================

// TODO: add patterns

def ADD : SebosBinaryALUInst<"add", 0b11000011>;
def ADDC : SebosBinaryALUInst<"addc", 0b00100100> {
  let Uses = !listconcat(SebosBinaryALUInst.Uses, [FLAGS]);
}
def SUB : SebosBinaryALUInst<"sub", 0b11000101>;
def SUBC : SebosBinaryALUInst<"subc", 0b00100101> {
  let Uses = !listconcat(SebosBinaryALUInst.Uses, [FLAGS]);
}
def AND : SebosBinaryALUInst<"and", 0b11100010>;
def OR : SebosBinaryALUInst<"or", 0b11100011>;
def XOR : SebosBinaryALUInst<"xor", 0b11110011>;
def NOT : SebosUnaryALUInst<"not", 0b11100100>;
def SHL : SebosUnaryALUInst<"shl", 0b11110010> {
  // no TMP in Uses as TMP is overwritten in the microinstructions, so the old TMP isn't actually being used
  let Defs = !listconcat(SebosUnaryALUInst.Defs, [TMP]);
}
def SLR : SebosUnaryALUInst<"slr", 0b11000100>;
def SAR : SebosUnaryALUInst<"sar", 0b11110000>;
def ROR : SebosUnaryALUInst<"ror", 0b11110100>;
def ROL : SebosUnaryALUInst<"rol", 0b11110001>;

def INCX : SebosOperandlessInst<"incx", 0b01100100, [X], [X]>;
def INCY : SebosOperandlessInst<"incy", 0b01100110, [Y], [Y]>;
def DECX : SebosOperandlessInst<"decx", 0b01100101, [X], [X]>;
def DECY : SebosOperandlessInst<"decy", 0b01100111, [Y], [Y]>;

def S7SDSM : SebosOperandlessInst<"s7sdsm", 0b11000010, [FLAGS], []>;
def S7SDUM : SebosOperandlessInst<"s7sdum", 0b00000011, [FLAGS], []>;

def NOP : SebosOperandlessInst<"nop", 0b00000000, [], []>;
def HLT : SebosOperandlessInst<"hlt", 0b00000001, [], []>;

def RET : SebosOperandlessInst<"ret", 0b00000111, [SP, BUF], [SP]>;

def JMPR : SebosOperandlessInst<"jmpr", 0b00000110, [], [XY]>;

def JMP : SebosInst<(outs), (ins i16addr:$addr), "jmp $addr"> {
  let Defs = !listconcat(SebosInst.Defs, [BUF]);

  field bits<24> Inst;
  let Inst{23-16} = 0b00000101;
  let Inst{15-0} = addr;
}
def JMPIND : SebosInst<(outs), (ins i16addr:$addr), "jmpind $addr"> {
  let Defs = !listconcat(SebosInst.Defs, [A, BUF, TMP, FLAGS]);

  field bits<24> Inst;
  let Inst{23-16} = 0b00100110;
  let Inst{15-0} = addr;
}

def BEQ     : SebosCompareAndBranchInst<"beq $addr", 0b00001010>;
def BNE     : SebosCompareAndBranchInst<"bne $addr", 0b00001011>;
def BLT     : SebosCompareAndBranchInst<"blt $addr", 0b10010110>;
def BLTU    : SebosCompareAndBranchInst<"bltu $addr", 0b00100010>;
def BLE     : SebosCompareAndBranchInst<"ble $addr", 0b10100110>;
def BLEU    : SebosCompareAndBranchInst<"bleu $addr", 0b11000000>;
def BGE     : SebosCompareAndBranchInst<"bge $addr", 0b10010111>;
def BGEU    : SebosCompareAndBranchInst<"bgeu $addr", 0b00100011>;
def BGT     : SebosCompareAndBranchInst<"bgt $addr", 0b10100111>;
def BGTU    : SebosCompareAndBranchInst<"bgtu $addr", 0b11000001>;
def BZS     : SebosFlagTestBranchInst<"bzs $addr", 0b00001000>;
def BZC     : SebosFlagTestBranchInst<"bzc $addr", 0b00001001>;
def BCS     : SebosFlagTestBranchInst<"bcs $addr", 0b00100000>;
def BCC     : SebosFlagTestBranchInst<"bcc $addr", 0b00100001>;
def BNS     : SebosFlagTestBranchInst<"bns $addr", 0b11110101>;
def BNC     : SebosFlagTestBranchInst<"bnc $addr", 0b11110110>;
def BVS     : SebosFlagTestBranchInst<"bvs $addr", 0b01100000>;
def BVC     : SebosFlagTestBranchInst<"bvc $addr", 0b01100001>;
def BRXRDYS : SebosFlagTestBranchInst<"brxrdys $addr", 0b11100000>;
def BRXRDYC : SebosFlagTestBranchInst<"brxrdyc $addr", 0b11100001>;
def BTXRDYS : SebosFlagTestBranchInst<"btxrdys $addr", 0b11101000>;
def BTXRDYC : SebosFlagTestBranchInst<"btxrdyc $addr", 0b11101001>;

def MOVATMP   : SebosMovInst<"mov A, TMP", A, TMP, 0b10011000>;
def MOVAB     : SebosMovInst<"mov A, B", A, B, 0b10011001>;
def MOVAC     : SebosMovInst<"mov A, C", A, C, 0b10011010>;
def MOVAX     : SebosMovInst<"mov A, X", A, X, 0b10011011>;
def MOVAY     : SebosMovInst<"mov A, Y", A, Y, 0b10011100>;
def MOVTMPA   : SebosMovInst<"mov TMP, A", TMP, A, 0b10101000>;
def MOVTMPB   : SebosMovInst<"mov TMP, B", TMP, B, 0b10101001>;
def MOVTMPC   : SebosMovInst<"mov TMP, C", TMP, C, 0b10101010>;
def MOVTMPX   : SebosMovInst<"mov TMP, X", TMP, X, 0b10101011>;
def MOVTMPY   : SebosMovInst<"mov TMP, Y", TMP, Y, 0b10101100>;
def MOVBA     : SebosMovInst<"mov B, A", B, A, 0b10111000>;
def MOVBTMP   : SebosMovInst<"mov B, TMP", B, TMP, 0b10111001>;
def MOVBC     : SebosMovInst<"mov B, C", B, C, 0b10111010>;
def MOVBX     : SebosMovInst<"mov B, X", B, X, 0b10111011>;
def MOVBY     : SebosMovInst<"mov B, Y", B, Y, 0b10111100>;
def MOVCA     : SebosMovInst<"mov C, A", C, A, 0b11001000>;
def MOVCTMP   : SebosMovInst<"mov C, TMP", C, TMP, 0b11001001>;
def MOVCB     : SebosMovInst<"mov C, B", C, B, 0b11001010>;
def MOVCX     : SebosMovInst<"mov C, X", C, X, 0b11001011>;
def MOVCY     : SebosMovInst<"mov C, Y", C, Y, 0b11001100>;
def MOVXA     : SebosMovInst<"mov X, A", X, A, 0b11011000>;
def MOVXTMP   : SebosMovInst<"mov X, TMP", X, TMP, 0b11011001>;
def MOVXB     : SebosMovInst<"mov X, B", X, B, 0b11011010>;
def MOVXC     : SebosMovInst<"mov X, C", X, C, 0b11011011>;
def MOVXY     : SebosMovInst<"mov X, Y", X, Y, 0b11011100>;
def MOVYA     : SebosMovInst<"mov Y, A", Y, A, 0b10011101>;
def MOVYTMP   : SebosMovInst<"mov Y, TMP", Y, TMP, 0b10101101>;
def MOVYB     : SebosMovInst<"mov Y, B", Y, B, 0b10111101>;
def MOVYC     : SebosMovInst<"mov Y, C", Y, C, 0b11001101>;
def MOVYX     : SebosMovInst<"mov Y, X", Y, X, 0b11011101>;
def MOVABUF   : SebosMovInst<"mov A, BUF", A, BUF, 0b10101111>;
def MOVBUFTMP : SebosMovInst<"mov BUF, TMP", BUF, TMP, 0b10111111>;
def MOVFA     : SebosMovInst<"mov F, A", FLAGS, A, 0b10011110>;
def MOVFTMP   : SebosMovInst<"mov F, TMP", FLAGS, TMP, 0b10101110>;
def MOVFB     : SebosMovInst<"mov F, B,", FLAGS, B, 0b10111110>;
def MOVFC     : SebosMovInst<"mov F, C,", FLAGS, C, 0b11001110>;
def MOVFX     : SebosMovInst<"mov F, X,", FLAGS, X, 0b11011110>;
def MOVFY     : SebosMovInst<"mov F, Y,", FLAGS, Y, 0b11101110>;

def LDINDR : SebosSingleReadRegisterOperandInst<"ldindr $reg", 0b11010, [], [XY]>;
def STINDR : SebosSingleWriteRegisterOperandInst<"stindr $reg", 0b11111, [], [XY]>;

def PUSH : SebosSingleReadRegisterOperandInst<"push $reg", 0b10010, [SP], [SP]>;
def PUSHI : SebosSingleImmediateOperandInst<"pushi $imm", 0b10110111, [SP, BUF], [SP]>
def POP : SebosSingleWriteRegisterOperandInst<"pop $reg", 0b10100, [SP], [SP]>;
def PEEK : SebosSingleWriteRegisterOperandInst<"peek $reg", 0b10110, [], [SP]>;

def OUT7SD : SebosSingleReadRegisterOperandInst<"out7sd $reg", 0b00101, [SEG7], []>;
def RXRD : SebosSingleWriteRegisterOperandInst<"rxrd $reg", 0b01110, [FLAGS], [SER_RX]>;
def TXSEND : SebosSingleReadRegisterOperandInst<"txsend $reg", 0b01111, [FLAGS, SER_TX], []>;

def OUT7SDI : SebosSingleImmediateOperandInst<"out7sdi $imm", 0b00101111, [SEG7], []>;
def TXSENDI : SebosSingleImmediateOperandInst<"txsendi $imm", 0b01111111, [FLAGS, SER_TX], []>;

def LDSPREL : SebosInst<(outs GPR8:$reg), (ins i8imm:$imm), "ldsprel $reg, $imm"> {
  field bits<16> Inst;
  let Inst{15-11} = 0b01010;
  let Inst{10-8} = reg;
  let Inst{7-0} = imm;

  let Defs = !listconcat(SebosInst.Defs, [FLAGS, A, TMP, BUF]);
  let Uses = !listconcat(SebosInst.Uses, [SP]);
}
def STSPREL : SebosInst<(outs), (ins GPR8:$reg, i8imm:$imm), "stsprel $reg, $imm"> {
  field bits<16> Inst;
  let Inst{15-11} = 0b01011;
  let Inst{10-8} = reg;
  let Inst{7-0} = imm;

  let Defs = !listconcat(SebosInst.Defs, [FLAGS, A, TMP, C, BUF]);
  let Uses = !listconcat(SebosInst.Uses, [SP]);
}

def LI : SebosInst<(outs GPR8:$reg), (ins i8imm:$imm), "li $reg, $imm"> {
  field bits<16> Inst;
  let Inst{15-11} = 0b01101;
  let Inst{10-8} = reg;
  let Inst{7-0} = imm;
}
def LD : SebosInst<(outs GPR8:$reg), (ins i16addr:$addr), "ld $reg, $addr"> {
  field bits<24> Inst;
  let Inst{23-19} = 0b00010;
  let Inst{18-16} = reg;
  let Inst{15-0} = addr;

  let Defs = !listconcat(SebosInst.Defs, [BUF]);
}
def ST : SebosInst<(outs), (ins GPR8:$reg, i16addr:$addr), "st $reg, $addr"> {
  field bits<24> Inst;
  let Inst{23-19} = 0b00011;
  let Inst{18-16} = reg;
  let Inst{15-0} = addr;

  let Defs = !listconcat(SebosInst.Defs, [BUF]);
}

def LDO : SebosInst<(outs GPR8:$reg), (ins OFFSET:$offsetreg, i16addr:$addr), "ldo $reg, $offsetreg, $addr"> {
  field bits<24> Inst;
  let Inst{23-20} = 0b0011;
  let Inst{19-17} = reg;
  let Inst{16} = offsetreg{0}; // last bit in the register encodes the bit for encoding the offset register 
  let Inst{15-0} = addr;

  let Defs = !listconcat(SebosInst.Defs, [A, TMP, BUF, FLAGS]);
}
def STO : SebosInst<(outs), (ins GPR8:$reg, OFFSET:$offsetreg, i16addr:$addr), "sto $reg, $offsetreg, $addr"> {
  field bits<24> Inst;
  let Inst{23-20} = 0b0100;
  let Inst{19-17} = reg;
  let Inst{16} = offsetreg{0}; // last bit in the register encodes the bit for encoding the offset register 
  let Inst{15-0} = addr;

  let Defs = !listconcat(SebosInst.Defs, [A, TMP, C, BUF, FLAGS]);
}

def LCDRDA : SebosInst<(outs), (ins LCD:$lcdreg), "lcdrda $lcdreg"> {
  let Inst{7-4} = 0b1000;
  let Inst{3} = lcdreg{0}; // last bit in the lcd register encodes the bit for encoding it
  let Inst{2-0} = 0b110;

  let Defs = !listconcat(SebosInst.Defs, [A]);
}
def OUTLCD : SebosInst<(outs), (ins LCD:$lcdreg, GPR8:$reg), "outlcd $lcdreg, $reg"> {
  let Inst{7-4} = 0b1000;
  let Inst{3} = lcdreg{0}; // last bit in the lcd register encodes the bit for encoding it
  let Inst{2-0} = reg;
}
def OUTLCDI : SebosInst<(outs), (ins LCD:$lcdreg, i8imm:$imm), "outlcdi $lcdreg, $imm"> {
  field bits<16> Inst;
  let Inst{15-12} = 0b1000;
  let Inst{11} = lcdreg{0}; // last bit in the lcd register encodes the bit for encoding it
  let Inst{10-8} = 0b111;
  let Inst{7-0} = imm;
}