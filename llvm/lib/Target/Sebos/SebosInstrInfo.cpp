//===-- SebosInstrInfo.cpp - Sebos Instruction Information ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosInstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_TARGET_DESC
#include "SebosGenInstrInfo.inc"

using namespace llvm;

SebosInstrInfo::SebosInstrInfo()
    : SebosGenInstrInfo(Sebos::ADJCALLSTACKDOWN, Sebos::ADJCALLSTACKUP), RI() {}
// NOTE: ADJCALLSTACKDOWN/UP don't exist in SebosInstrInfo.td yet -- these
// are the standard pseudo-instructions bracketing outgoing-argument stack
// adjustment around a call. Needed once the calling convention's call
// lowering is written.

//===----------------------------------------------------------------------===//
// Register copies -- built from the MOV* table (movData.json).
//===----------------------------------------------------------------------===//

void SebosInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                  const DebugLoc &DL, Register DestReg, Register SrcReg,
                  bool KillSrc, bool RenamableDest = false,
                  bool RenamableSrc = false) const {

  unsigned Opc = 0;

  // clang-format off
  switch (DestReg) {
  case Sebos::TMP:
    switch (SrcReg) {
    case Sebos::A: Opc = Sebos::MOVATMP; break;
    case Sebos::B: Opc = Sebos::MOVBTMP; break;
    case Sebos::C: Opc = Sebos::MOVCTMP; break;
    case Sebos::X: Opc = Sebos::MOVXTMP; break;
    case Sebos::Y: Opc = Sebos::MOVYTMP; break;
    case Sebos::BUF: Opc = Sebos::MOVBUFTMP; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFTMP; break;
    default: break;
    }
    break;
  case Sebos::A:
    switch (SrcReg) {
    case Sebos::TMP: Opc = Sebos::MOVTMPA; break;
    case Sebos::B: Opc = Sebos::MOVBA; break;
    case Sebos::C: Opc = Sebos::MOVCA; break;
    case Sebos::X: Opc = Sebos::MOVXA; break;
    case Sebos::Y: Opc = Sebos::MOVYA; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFA; break;
    default: break;
    }
    break;
  case Sebos::B:
    switch (SrcReg) {
    case Sebos::A: Opc = Sebos::MOVAB; break;
    case Sebos::TMP: Opc = Sebos::MOVTMPB; break;
    case Sebos::C: Opc = Sebos::MOVCB; break;
    case Sebos::X: Opc = Sebos::MOVXB; break;
    case Sebos::Y: Opc = Sebos::MOVYB; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFB; break;
    default: break;
    }
    break;
  case Sebos::C:
    switch (SrcReg) {
    case Sebos::A: Opc = Sebos::MOVAC; break;
    case Sebos::TMP: Opc = Sebos::MOVTMPC; break;
    case Sebos::B: Opc = Sebos::MOVBC; break;
    case Sebos::X: Opc = Sebos::MOVXC; break;
    case Sebos::Y: Opc = Sebos::MOVYC; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFC; break;
    default: break;
    }
    break;
  case Sebos::X:
    switch (SrcReg) {
    case Sebos::A: Opc = Sebos::MOVAX; break;
    case Sebos::TMP: Opc = Sebos::MOVTMPX; break;
    case Sebos::B: Opc = Sebos::MOVBX; break;
    case Sebos::C: Opc = Sebos::MOVCX; break;
    case Sebos::Y: Opc = Sebos::MOVYX; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFX; break;
    default: break;
    }
    break;
  case Sebos::Y:
    switch (SrcReg) {
    case Sebos::A: Opc = Sebos::MOVAY; break;
    case Sebos::TMP: Opc = Sebos::MOVTMPY; break;
    case Sebos::B: Opc = Sebos::MOVBY; break;
    case Sebos::C: Opc = Sebos::MOVCY; break;
    case Sebos::X: Opc = Sebos::MOVXY; break;
    case Sebos::FLAGS: Opc = Sebos::MOVFY; break;
    default: break;
    }
    break;
  default:
    break;
  }
  // clang-format on

  assert(Opc != 0 && "Sebos: no direct MOV between these two registers -- "
                      "may need a two-hop sequence through A/TMP");

  BuildMI(MBB, MI, DL, get(Opc));
}

//===----------------------------------------------------------------------===//
// Spill / reload -- via SP-relative addressing (no frame pointer).
// LDSPRELU/STSPRELU only reach non-negative offsets from SP (see the
// open ldsprel/stsprel sign issue) -- fine for now since spill slots are
// always allocated above the point SP sits at when these run.
//===----------------------------------------------------------------------===//

void SebosInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MI, Register SrcReg,
                          bool isKill, int FrameIndex,
                          const TargetRegisterClass *RC, Register VReg,
                          MachineInstr::MIFlag Flags =
                              MachineInstr::NoFlags) const {

  DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();
  BuildMI(MBB, MI, DL, get(Sebos::STSPRELU))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex)
      .addImm(0);
}

void SebosInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register DestReg,
                           int FrameIndex, const TargetRegisterClass *RC,
                           Register VReg, unsigned SubIdx,
                           MachineInstr::MIFlag Flags =
                               MachineInstr::NoFlags) const {
  DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();
  BuildMI(MBB, MI, DL, get(Sebos::LDSPRELU), DestReg)
      .addFrameIndex(FrameIndex)
      .addImm(0);
}

//===----------------------------------------------------------------------===//
// Branch analysis.
//===----------------------------------------------------------------------===//

static bool isCondBranchOpcode(unsigned Opc) {
  switch (Opc) {
  case Sebos::BEQ: case Sebos::BNE:
  case Sebos::BLT: case Sebos::BLTU:
  case Sebos::BLE: case Sebos::BLEU:
  case Sebos::BGE: case Sebos::BGEU:
  case Sebos::BGT: case Sebos::BGTU:
  case Sebos::BZS: case Sebos::BZC:
  case Sebos::BCS: case Sebos::BCC:
  case Sebos::BNS: case Sebos::BNC:
  case Sebos::BVS: case Sebos::BVC:
  case Sebos::BRXRDYS: case Sebos::BRXRDYC:
  case Sebos::BTXRDYS: case Sebos::BTXRDYC:
    return true;
  default:
    return false;
  }
}

static unsigned getOppositeBranchOpcode(unsigned Opc) {
  switch (Opc) {
  case Sebos::BEQ: return Sebos::BNE;
  case Sebos::BNE: return Sebos::BEQ;
  case Sebos::BLT: return Sebos::BGE;
  case Sebos::BGE: return Sebos::BLT;
  case Sebos::BLTU: return Sebos::BGEU;
  case Sebos::BGEU: return Sebos::BLTU;
  case Sebos::BLE: return Sebos::BGT;
  case Sebos::BGT: return Sebos::BLE;
  case Sebos::BLEU: return Sebos::BGTU;
  case Sebos::BGTU: return Sebos::BLEU;
  case Sebos::BZS: return Sebos::BZC;
  case Sebos::BZC: return Sebos::BZS;
  case Sebos::BCS: return Sebos::BCC;
  case Sebos::BCC: return Sebos::BCS;
  case Sebos::BNS: return Sebos::BNC;
  case Sebos::BNC: return Sebos::BNS;
  case Sebos::BVS: return Sebos::BVC;
  case Sebos::BVC: return Sebos::BVS;
  case Sebos::BRXRDYS: return Sebos::BRXRDYC;
  case Sebos::BRXRDYC: return Sebos::BRXRDYS;
  case Sebos::BTXRDYS: return Sebos::BTXRDYC;
  case Sebos::BTXRDYC: return Sebos::BTXRDYS;
  default:
    llvm_unreachable("Sebos: not a conditional branch opcode");
  }
}

bool SebosInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  auto I = MBB.instr_end();
  if (I == MBB.instr_begin())
    return false;
  --I;

  if (!I->isTerminator())
    return false;

  if (I->getOpcode() == Sebos::JMP) {
    TBB = I->getOperand(0).getMBB();
    if (I == MBB.instr_begin())
      return false;
    --I;
  }

  if (I->isTerminator() && isCondBranchOpcode(I->getOpcode())) {
    if (TBB)
      FBB = TBB;
    TBB = I->getOperand(0).getMBB();
    Cond.push_back(MachineOperand::CreateImm(I->getOpcode()));
    return false;
  }

  return true; // JMPR, JMPIND, RET, HLT, or unrecognized -- refuse
}

unsigned SebosInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                       MachineBasicBlock *TBB,
                                       MachineBasicBlock *FBB,
                                       ArrayRef<MachineOperand> Cond,
                                       const DebugLoc &DL,
                                       int *BytesAdded) const {
  assert(TBB && "insertBranch: must always have a true target");
  unsigned Count = 0;

  if (Cond.empty()) {
    BuildMI(&MBB, DL, get(Sebos::JMP)).addMBB(TBB);
    Count = 1;
  } else {
    unsigned Opc = Cond[0].getImm();
    BuildMI(&MBB, DL, get(Opc)).addMBB(TBB);
    Count = 1;
    if (FBB) {
      BuildMI(&MBB, DL, get(Sebos::JMP)).addMBB(FBB);
      Count = 2;
    }
  }

  if (BytesAdded)
    *BytesAdded = 0; // TODO: real byte-size accounting once needed
  return Count;
}

unsigned SebosInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  unsigned Count = 0;
  auto I = MBB.instr_end();
  while (I != MBB.instr_begin()) {
    --I;
    if (I->isDebugInstr())
      continue;
    if (I->getOpcode() != Sebos::JMP && !isCondBranchOpcode(I->getOpcode()))
      break;
    I->eraseFromParent();
    I = MBB.instr_end();
    ++Count;
  }
  if (BytesRemoved)
    *BytesRemoved = 0;
  return Count;
}

bool SebosInstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  if (Cond.size() != 1)
    return true;
  Cond[0].setImm(getOppositeBranchOpcode(Cond[0].getImm()));
  return false;
}

bool SebosInstrInfo::isAsCheapAsAMove(const MachineInstr &MI) const {
  return MI.getOpcode() == Sebos::LI;
}

bool SebosInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  case Sebos::ADDPSEUDO: return expandBinaryALUPseudo(MI, Sebos::ADD);
  case Sebos::SUBPSEUDO: return expandBinaryALUPseudo(MI, Sebos::SUB);
  case Sebos::ANDPSEUDO: return expandBinaryALUPseudo(MI, Sebos::AND);
  case Sebos::ORPSEUDO:  return expandBinaryALUPseudo(MI, Sebos::OR);
  case Sebos::XORPSEUDO: return expandBinaryALUPseudo(MI, Sebos::XOR);
  case Sebos::NOTPSEUDO: return expandUnaryALUPseudo(MI, Sebos::NOT);
  case Sebos::SHLPSEUDO: return expandUnaryALUPseudo(MI, Sebos::SHL);
  case Sebos::SLRPSEUDO: return expandUnaryALUPseudo(MI, Sebos::SLR);
  case Sebos::SARPSEUDO: return expandUnaryALUPseudo(MI, Sebos::SAR);
  case Sebos::RORPSEUDO: return expandUnaryALUPseudo(MI, Sebos::ROR);
  case Sebos::ROLPSEUDO: return expandUnaryALUPseudo(MI, Sebos::ROL);
  case Sebos::BEQPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BEQ);
  case Sebos::BNEPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BNE);
  case Sebos::BLTPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BLT);
  case Sebos::BLTUPSEUDO: return expandCondBranchPseudo(MI, Sebos::BLTU);
  case Sebos::BLEPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BLE);
  case Sebos::BLEUPSEUDO: return expandCondBranchPseudo(MI, Sebos::BLEU);
  case Sebos::BGEPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BGE);
  case Sebos::BGEUPSEUDO: return expandCondBranchPseudo(MI, Sebos::BGEU);
  case Sebos::BGTPSEUDO:  return expandCondBranchPseudo(MI, Sebos::BGT);
  case Sebos::BGTUPSEUDO: return expandCondBranchPseudo(MI, Sebos::BGTU);
  default: return false;
  }
}

void SebosInstrInfo::moveIntoALUOperand(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator MI,
                                         const DebugLoc &DL, MCRegister Dest,
                                         MCRegister Src) const {
  if (Dest == Src)
    return; // already in place -- no MOV Dest,Dest instruction exists
  // KillSrc=false: conservative. Correctly propagating the pseudo's
  // original per-operand kill flags through a multi-instruction expansion
  // needs more care than this draft gives it; false is always safe, just
  // potentially misses a scheduling opportunity, never a correctness bug.
  copyPhysReg(MBB, MI, DL, Dest, Src, /*KillSrc=*/false);
}

bool SebosInstrInfo::expandBinaryALUPseudo(MachineInstr &MI,
                                            unsigned RealOpc) const {
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc DL = MI.getDebugLoc();
  Register Dst = MI.getOperand(0).getReg();
  Register Src1 = MI.getOperand(1).getReg();
  Register Src2 = MI.getOperand(2).getReg();

  // The one case a simple move order can't resolve: src1 already sitting
  // in TMP and src2 already sitting in A is a true swap -- writing either
  // destination first destroys the other's source value. Needs a
  // temporary register to break the cycle; not yet implemented, so this
  // fails loudly rather than silently miscompiling.
  assert(!(Src1 == Sebos::TMP && Src2 == Sebos::A) &&
         "Sebos: ALU pseudo expansion hit an A/TMP swap case");

  moveIntoALUOperand(MBB, MI, DL, Sebos::TMP, Src2);
  moveIntoALUOperand(MBB, MI, DL, Sebos::A, Src1);

  BuildMI(MBB, MI, DL, get(RealOpc));

  moveIntoALUOperand(MBB, MI, DL, Dst, Sebos::A);

  MI.eraseFromParent();
  return true;
}

bool SebosInstrInfo::expandUnaryALUPseudo(MachineInstr &MI,
                                           unsigned RealOpc) const {
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc DL = MI.getDebugLoc();
  Register Dst = MI.getOperand(0).getReg();
  Register Src = MI.getOperand(1).getReg();

  moveIntoALUOperand(MBB, MI, DL, Sebos::A, Src);
  BuildMI(MBB, MI, DL, get(RealOpc));
  moveIntoALUOperand(MBB, MI, DL, Dst, Sebos::A);

  MI.eraseFromParent();
  return true;
}

bool SebosInstrInfo::expandCondBranchPseudo(MachineInstr &MI,
                                             unsigned RealOpc) const {
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc DL = MI.getDebugLoc();
  Register Lhs = MI.getOperand(0).getReg();
  Register Rhs = MI.getOperand(1).getReg();
  MachineBasicBlock *Dst = MI.getOperand(2).getMBB();

  // Same A/TMP swap gap as expandBinaryALUPseudo -- see that function's
  // comment. Not yet handled here either.
  assert(!(Lhs == Sebos::TMP && Rhs == Sebos::A) &&
         "Sebos: conditional branch pseudo expansion hit an A/TMP swap case");

  moveIntoALUOperand(MBB, MI, DL, Sebos::TMP, Rhs);
  moveIntoALUOperand(MBB, MI, DL, Sebos::A, Lhs);

  BuildMI(MBB, MI, DL, get(RealOpc)).addMBB(Dst);

  MI.eraseFromParent();
  return true;
}