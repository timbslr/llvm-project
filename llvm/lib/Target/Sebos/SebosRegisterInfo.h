//===-- SebosRegisterInfo.h - Sebos Register Information -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSREGISTERINFO_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "SebosGenRegisterInfo.inc"

namespace llvm {

class TargetInstrInfo;

struct SebosRegisterInfo : public SebosGenRegisterInfo {
  SebosRegisterInfo();

  const MCPhysReg *
  getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                            unsigned FIOperandNum,
                            RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;

  const TargetRegisterClass *
  getPointerRegClass(const MachineFunction &MF,
                      unsigned Kind = 0) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSREGISTERINFO_H