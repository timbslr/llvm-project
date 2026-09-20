//===-- SebosISelDAGToDAG.cpp - A Dag to Dag Inst Selector for Sebos -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosISelDAGToDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"

using namespace llvm;

bool SebosDAGToDAGISel::SelectAddrFI(SDValue Addr, SDValue &Base,
                                      SDValue &Offset) {
  if (auto *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(
        FIN->getIndex(), TLI->getPointerTy(CurDAG->getDataLayout()));
    Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), MVT::i8);
    return true;
  }
  if (Addr.getOpcode() == ISD::ADD) {
    if (auto *FIN = dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
      if (auto *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
        Base = CurDAG->getTargetFrameIndex(
            FIN->getIndex(), TLI->getPointerTy(CurDAG->getDataLayout()));
        Offset = CurDAG->getTargetConstant(CN->getSExtValue(), SDLoc(Addr),
                                            MVT::i8);
        return true;
      }
    }
  }
  return false;
}

void SebosDAGToDAGISel::Select(SDNode *N) {
  // Let the TableGen-generated matcher (built from every Pat<>/Pattern in
  // SebosInstrInfo.td) try first; only fall through to hand-written cases
  // for anything it can't express.
  SelectCode(N);
}

// Factory function -- this is what SebosTargetMachine.cpp's
// createPassConfig()/addInstSelector() has been calling since the very
// start of this backend.
FunctionPass *llvm::createSebosISelDag(SebosTargetMachine &TM,
                                        CodeGenOptLevel OptLevel) {
  return new SebosDAGToDAGISel(TM, OptLevel);
}