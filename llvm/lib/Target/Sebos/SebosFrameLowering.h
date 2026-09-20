//===-- SebosFrameLowering.h - Frame Info for Sebos ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSFRAMELOWERING_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class SebosSubtarget;

class SebosFrameLowering : public TargetFrameLowering {
public:
  explicit SebosFrameLowering(const SebosSubtarget &STI)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsUp,
                             /*StackAlignment=*/Align(1),
                             /*LocalAreaOffset=*/0) {}

  void emitPrologue(MachineFunction &MF,
                     MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF,
                     MachineBasicBlock &MBB) const override;

                     
                     StackOffset getFrameIndexReference(const MachineFunction &MF, int FI,
                      Register &FrameReg) const override;
                      
                      bool hasReservedCallFrame(const MachineFunction &MF) const override {
                        return true; // no separate ADJCALLSTACKDOWN/UP-driven dynamic
                        // adjustment needed once the call sequence is designed;
                        // revisit alongside SebosISelLowering's LowerCall.
                      }
  protected:                  
    bool hasFPImpl(const MachineFunction &MF) const override { return false; }
};
                    
} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSFRAMELOWERING_H