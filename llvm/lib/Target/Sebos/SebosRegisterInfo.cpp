//===-- SebosRegisterInfo.cpp - Sebos Register Information --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosRegisterInfo.h"
#include "SebosSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

#define GET_REGINFO_TARGET_DESC
#include "SebosGenRegisterInfo.inc"

using namespace llvm;

SebosRegisterInfo::SebosRegisterInfo() : SebosGenRegisterInfo(/*RA=*/0) {}

const MCPhysReg *
SebosRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  // No registers preserved across calls. An empty, zero-terminated list means "nothing
  // callee-saved."
  static const MCPhysReg CSR_Sebos_SaveList[] = {0};
  return CSR_Sebos_SaveList;
}

BitVector SebosRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Stack pointer: implicitly written by PUSH/POP, never a compiler-
  // assignable value.
  Reserved.set(Sebos::SP);
  Reserved.set(Sebos::SP_L);
  Reserved.set(Sebos::SP_H);

  // Program counter: control flow only, never a GPR.
  Reserved.set(Sebos::PC);
  Reserved.set(Sebos::PC_L);
  Reserved.set(Sebos::PC_H);

  // Memory address register: internal to load/store microcode.
  Reserved.set(Sebos::MAR);
  Reserved.set(Sebos::MAR_L);
  Reserved.set(Sebos::MAR_H);

  // Fetch/decode plumbing.
  Reserved.set(Sebos::BUF);
  Reserved.set(Sebos::IR);

  // I/O
  Reserved.set(Sebos::SER_RX);
  Reserved.set(Sebos::SEG7);

  // Status register.
  Reserved.set(Sebos::FLAGS);

  return Reserved;
}

bool SebosRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                             int SPAdj,
                                             unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  // Placeholder -- real implementation depends on SebosFrameLowering's
  // stack layout (frame-pointer-relative vs SP-relative addressing),
  // which hasn't been designed yet.
  llvm_unreachable("eliminateFrameIndex not yet implemented");
}

Register SebosRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  // Using SP directly for now; revisit if a dedicated frame pointer
  // register turns out to be needed.
  return Sebos::SP; // TODO edit if a dedicated frame pointer register should be introduced later 
}

const TargetRegisterClass *
SebosRegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                       unsigned Kind) const {
  return &Sebos::PTR16RegClass;
}