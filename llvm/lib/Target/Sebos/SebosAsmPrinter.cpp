//===-- SebosAsmPrinter.cpp - Sebos LLVM Assembly Printer ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosAsmPrinter.h"
#include "MCTargetDesc/SebosMCTargetDesc.h"
#include "llvm/CodeGen/AsmPrinterHandler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "SebosGenInstrInfo.inc"