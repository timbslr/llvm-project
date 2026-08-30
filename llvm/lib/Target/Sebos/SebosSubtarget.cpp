//===-- SebosSubtarget.cpp - Sebos Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosSubtarget.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "SebosGenSubtargetInfo.inc"

using namespace llvm;

SebosSubtarget::SebosSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                                const TargetMachine &TM)
    : SebosGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS), RegInfo() {
  initSubtargetFeatures(CPU, FS);
}

void SebosSubtarget::initSubtargetFeatures(StringRef CPU, StringRef FS) {
  // No feature-bit variation exists yet -- single hardware target.
  // ParseSubtargetFeatures is still called because it's what the
  // TableGen-generated SebosGenSubtargetInfo base expects to run,
  // even with an empty feature set.
  ParseSubtargetFeatures(CPU, /*TuneCPU=*/CPU, FS);
}