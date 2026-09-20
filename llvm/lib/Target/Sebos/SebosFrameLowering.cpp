//===-- SebosFrameLowering.cpp - Frame Info for Sebos --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosFrameLowering.h"
#include "SebosInstrInfo.h"
#include "SebosSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

// ADDISPU/SUBISPU take an 8-bit immediate; a frame larger than 255 bytes
// needs the adjustment split into 255-byte chunks. On this target that's
// an extreme case, but handled correctly rather than silently truncated.
static void emitSPAdjust(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MBBI,
                          const DebugLoc &DL, const TargetInstrInfo &TII,
                          unsigned Opc, uint64_t Amount) {
  while (Amount > 0) {
    uint8_t Chunk = static_cast<uint8_t>(std::min<uint64_t>(Amount, 255));
    BuildMI(MBB, MBBI, DL, TII.get(Opc)).addImm(Chunk);
    Amount -= Chunk;
  }
}

void SebosFrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL;

  uint64_t FrameSize = MFI.getStackSize();
  if (FrameSize == 0)
    return;

  // Stack grows upward; allocating room for locals means moving SP
  // forward, the same direction PUSH does.
  emitSPAdjust(MBB, MBBI, DL, TII, Sebos::ADDISPU, FrameSize);
}

void SebosFrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.getFirstTerminator();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t FrameSize = MFI.getStackSize();
  if (FrameSize == 0)
    return;

  emitSPAdjust(MBB, MBBI, DL, TII, Sebos::SUBISPU, FrameSize);
}

StackOffset SebosFrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI, Register &FrameReg) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  FrameReg = Sebos::SP;

  // MFI assigns each object a signed offset from a conceptual frame
  // origin, conventionally negative as objects are allocated (matching
  // the usual downward-growing-stack assumption PEI's default layout
  // logic uses). Our addressing is "SP - distance", so the distance to
  // subtract is the negation of that offset -- i.e. more negative MFI
  // offset means further below SP, hence a larger positive subtraction.
  int64_t ObjectOffset = MFI.getObjectOffset(FI);
  int64_t Distance = -ObjectOffset;

  return StackOffset::getFixed(Distance);
}