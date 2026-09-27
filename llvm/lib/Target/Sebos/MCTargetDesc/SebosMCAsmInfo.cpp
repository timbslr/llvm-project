//===-- SebosMCAsmInfo.cpp - Sebos Asm Properties ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosMCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

SebosMCAsmInfo::SebosMCAsmInfo(const Triple &TT, const MCTargetOptions &Options)
    : MCAsmInfoELF(Options) {
  CodePointerSize = 2;
  CalleeSaveStackSlotSize = 1;
  CommentString = ";";
  UsesELFSectionDirectiveForBSS = true;
  ZeroDirective = "\t.zero\t";
  Data8bitsDirective = "\t.byte\t";
  Data16bitsDirective = "\t.word\t";
  SupportsDebugInformation = false;
  ExceptionsType = ExceptionHandling::None;
}