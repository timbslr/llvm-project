//===-- SebosISelDAGToDAG.h - A Dag to Dag Inst Selector for Sebos -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSISELDAGTODAG_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSISELDAGTODAG_H

#include "SebosTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "MCTargetDesc/SebosMCTargetDesc.h"
#include "SebosISelLowering.h"   // for SebosISD::RET_FLAG

namespace llvm {

// 1. Implementation class contains the actual selection logic
class SebosDAGToDAGISelImpl : public SelectionDAGISel {
public:
  SebosDAGToDAGISelImpl(SebosTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  bool SelectAddrFI(SDValue Addr, SDValue &Base, SDValue &Offset);

  // Generated from SebosInstrInfo.td's Pat<>/Pattern entries.
  void Select(SDNode *N) override;

#include "SebosGenDAGISel.inc"
};

// 2. Legacy pass wrapper that inherits from FunctionPass (via SelectionDAGISelLegacy)
class SebosDAGToDAGISel : public SelectionDAGISelLegacy {
public:
  static char ID;

  SebosDAGToDAGISel(SebosTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISelLegacy(ID, std::make_unique<SebosDAGToDAGISelImpl>(TM, OptLevel)) {}
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSISELDAGTODAG_H