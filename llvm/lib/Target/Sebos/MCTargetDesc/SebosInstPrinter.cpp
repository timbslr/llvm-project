// SebosInstPrinter.cpp
#include "SebosInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void SebosInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                  StringRef Annot, const MCSubtargetInfo &STI,
                                  raw_ostream &O) {
  printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void SebosInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg())
    O << getRegisterName(Op.getReg());
  else if (Op.isImm())
    O << Op.getImm();
  else if(Op.isExpr())
    O << Op.getExpr();
}