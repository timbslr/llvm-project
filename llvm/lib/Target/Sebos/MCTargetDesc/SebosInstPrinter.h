// SebosInstPrinter.h
#ifndef LLVM_LIB_TARGET_SEBOS_MCTARGETDESC_SEBOSINSTPRINTER_H
#define LLVM_LIB_TARGET_SEBOS_MCTARGETDESC_SEBOSINSTPRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class SebosInstPrinter : public MCInstPrinter {
public:
  SebosInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                    const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  void printInstruction(const MCInst *MI, uint64_t Address, raw_ostream &O);
  static const char *getRegisterName(MCRegister Reg);
  std::pair<const char *, uint64_t> getMnemonic(const MCInst &MI);

  void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O);

  void printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                 const MCSubtargetInfo &STI, raw_ostream &O) override;
};
} // namespace llvm


#endif