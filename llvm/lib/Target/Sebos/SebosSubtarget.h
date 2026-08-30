//===-- SebosSubtarget.h - Define Subtarget for the Sebos target -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Sebos specific subclass of TargetSubtargetInfo.
// Sebos has exactly one hardware configuration -- this class exists
// because TargetSubtargetInfo is the required owner of RegisterInfo,
// InstrInfo, FrameLowering, and TargetLowering, not because Sebos has
// multiple variants to select between.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSSUBTARGET_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSSUBTARGET_H

#include "SebosRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "SebosGenSubtargetInfo.inc"

namespace llvm {

class StringRef;

class SebosSubtarget : public SebosGenSubtargetInfo {
  SebosRegisterInfo RegInfo;

  void initSubtargetFeatures(StringRef CPU, StringRef FS);

public:
  SebosSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                 const TargetMachine &TM);

  const SebosRegisterInfo *getRegisterInfo() const override {
    return &RegInfo;
  }

  // TargetInstrInfo, TargetFrameLowering, and TargetLowering accessors
  // are added here once those classes exist -- deliberately omitted for
  // now rather than stubbed, so the compiler flags every remaining
  // dependent as unimplemented instead of silently returning null.
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSSUBTARGET_H