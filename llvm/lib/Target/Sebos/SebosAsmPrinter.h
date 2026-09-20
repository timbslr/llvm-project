//===-- SebosAsmPrinter.h - Sebos LLVM Assembly Printer ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSASMPRINTER_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSASMPRINTER_H

#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {

class SebosAsmPrinter : public AsmPrinter {
public:
  explicit SebosAsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Sebos Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSASMPRINTER_H