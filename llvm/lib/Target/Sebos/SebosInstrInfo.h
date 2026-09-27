//===-- SebosInstrInfo.h - Sebos Instruction Information -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSINSTRINFO_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSINSTRINFO_H

#include "SebosRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "SebosSubtarget.h"

#define GET_INSTRINFO_HEADER
#include "SebosGenInstrInfo.inc"

namespace llvm {

class SebosInstrInfo : public SebosGenInstrInfo {
  const SebosRegisterInfo RI;

public:
  SebosInstrInfo(const SebosSubtarget &STI);

  const SebosRegisterInfo &getRegisterInfo() const { return RI; }

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                    const DebugLoc &DL, Register DestReg, Register SrcReg,
                    bool KillSrc, bool RenamableDest = false,
                    bool RenamableSrc = false) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register SrcReg,
                            bool isKill, int FrameIndex,
                            const TargetRegisterClass *RC, Register VReg,
                            MachineInstr::MIFlag Flags =
                                MachineInstr::NoFlags) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            Register VReg, unsigned SubIdx,
                            MachineInstr::MIFlag Flags =
                                MachineInstr::NoFlags) const override;

  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                      MachineBasicBlock *&FBB,
                      SmallVectorImpl<MachineOperand> &Cond,
                      bool AllowModify = false) const override;

  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                         MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                         const DebugLoc &DL,
                         int *BytesAdded = nullptr) const override;

  unsigned removeBranch(MachineBasicBlock &MBB,
                         int *BytesRemoved = nullptr) const override;

  bool reverseBranchCondition(
      SmallVectorImpl<MachineOperand> &Cond) const override;

  bool isAsCheapAsAMove(const MachineInstr &MI) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

private:
  void moveIntoALUOperand(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI,
                           const DebugLoc &DL, MCRegister Dest,
                           MCRegister Src) const;
  bool expandBinaryALUPseudo(MachineInstr &MI, unsigned RealOpc) const;
  bool expandUnaryALUPseudo(MachineInstr &MI, unsigned RealOpc) const;
  bool expandCondBranchPseudo(MachineInstr &MI, unsigned RealOpc) const;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSINSTRINFO_H