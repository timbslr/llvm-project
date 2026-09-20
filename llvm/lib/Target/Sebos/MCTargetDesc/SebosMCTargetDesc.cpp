//===-- SebosMCTargetDesc.cpp - Sebos Target Descriptions ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosMCTargetDesc.h"
#include "SebosMCAsmInfo.h"
#include "TargetInfo/SebosTargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "SebosGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "SebosGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "SebosGenRegisterInfo.inc"

MCInstrInfo *llvm::createSebosMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitSebosMCInstrInfo(X);
  return X;
}

MCRegisterInfo *llvm::createSebosMCRegisterInfo() {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitSebosMCRegisterInfo(X, /*RA=*/0);
  return X;
}

static MCSubtargetInfo *
createSebosMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createSebosMCSubtargetInfoImpl(TT, CPU, /*TuneCPU=*/CPU, FS);
}

static MCAsmInfo *createSebosMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  return new SebosMCAsmInfo(TT);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSebosTargetMC() {
  Target &T = getTheSebosTarget();
  TargetRegistry::RegisterMCAsmInfo(T, createSebosMCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(T, createSebosMCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createSebosMCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(T, createSebosMCSubtargetInfo);
}