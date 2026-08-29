//===-- SebosTargetMachine.h - Define TargetMachine for Sebos -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Sebos specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSTARGETMACHINE_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSTARGETMACHINE_H

#include "SebosSubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>
#include <optional>

namespace llvm {

class SebosTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  SebosSubtarget Subtarget;

public:
  SebosTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      std::optional<Reloc::Model> RM,
                      std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                      bool JIT);

  ~SebosTargetMachine() override;

  // We only ever have one subtarget config, but the API still requires
  // this accessor — it's how codegen reaches InstrInfo/RegisterInfo/
  // FrameLowering/TargetLowering.
  const SebosSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                             const TargetSubtargetInfo *STI) const override;

  bool isNoopAddrSpaceCast(unsigned, unsigned) const override { return true; }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSTARGETMACHINE_H