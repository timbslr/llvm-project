//===-- SebosSubtarget.cpp - Sebos Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosSubtarget.h"

#define DEBUG_TYPE "sebos-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "SebosGenSubtargetInfo.inc"


using namespace llvm;

SebosSubtarget::SebosSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                                const TargetMachine &TM)
    : SebosGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS), RegInfo() {
  ParseSubtargetFeatures(CPU, /*TuneCPU=*/CPU, FS);
}