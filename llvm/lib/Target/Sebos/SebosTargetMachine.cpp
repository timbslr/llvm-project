//===-- SebosTargetMachine.cpp - Define TargetMachine for Sebos ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosTargetMachine.h"
#include "MCTargetDesc/SebosMCTargetDesc.h"
#include "SebosTargetObjectFile.h"
#include "TargetInfo/SebosTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

// 8-bit ALU/registers, 16-bit pointers/addresses.
//   e         - little endian (pick whichever is true for you)
//   m:e       - ELF-style name mangling (adjust if you're not using ELF)
//   p:16:16   - pointers are 16 bits, 16-bit aligned
//   i8:8      - 8-bit ints are 8-bit aligned (the "natural" register width)
//   i16:16    - 16-bit ints (e.g. pointers-as-integers) are 16-bit aligned
//   a:8       - aggregates are 8-bit aligned
//   n8        - the ALU natively operates on 8-bit values only
static std::string computeDataLayout() {
  return "e-m:e-p:16:16-i8:8-i16:16-a:8-n8";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

SebosTargetMachine::SebosTargetMachine(const Target &T, const Triple &TT,
                                        StringRef CPU, StringRef FS,
                                        const TargetOptions &Options,
                                        std::optional<Reloc::Model> RM,
                                        std::optional<CodeModel::Model> CM,
                                        CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, computeDataLayout(), TT, CPU, FS, Options,
                                getEffectiveRelocModel(RM),
                                CM.value_or(CodeModel::Small), OL),
      TLOF(std::make_unique<SebosTargetObjectFile>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

SebosTargetMachine::~SebosTargetMachine() = default;

MachineFunctionInfo *SebosTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  // Return nullptr (the default) unless you need per-function target state.
  return nullptr;
}

namespace {
class SebosPassConfig : public TargetPassConfig {
public:
  SebosPassConfig(SebosTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  SebosTargetMachine &getSebosTargetMachine() const {
    return getTM<SebosTargetMachine>();
  }

  bool addInstSelector() override;
};
} // namespace

TargetPassConfig *SebosTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new SebosPassConfig(*this, PM);
}

bool SebosPassConfig::addInstSelector() {
  addPass(createSebosISelDag(getSebosTargetMachine(), getOptLevel()));
  return false;
}

// This is what `llc -mtriple=sebos` / `opt` hooks into at startup.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSebosTarget() {
  RegisterTargetMachine<SebosTargetMachine> X(getTheSebosTarget());
}