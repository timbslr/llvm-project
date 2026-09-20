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

SebosMCAsmInfo::SebosMCAsmInfo(const Triple &TT) {
  CodePointerSize = 2;       // 16-bit addresses
  CalleeSaveStackSlotSize = 1;

  CommentString = ";";
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";

  UsesELFSectionDirectiveForBSS = true;
  ZeroDirective = "\t.zero\t";

  Data8bitsDirective = "\t.byte\t";
  Data16bitsDirective = "\t.word\t";
  // No 32/64-bit data directives should ever be emitted -- there's
  // nothing on this target that would need them, so deliberately not
  // set (left at MCAsmInfo's defaults, which just won't be exercised).

  SupportsDebugInformation = false; // revisit once/if debug info matters
  ExceptionsType = ExceptionHandling::None;
}