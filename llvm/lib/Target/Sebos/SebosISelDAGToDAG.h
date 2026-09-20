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

namespace llvm {

class SebosDAGToDAGISel : public SelectionDAGISel {
public:
  SebosDAGToDAGISel(SebosTargetMachine &TM, CodeGenOptLevel OptLevel)
      : SelectionDAGISel(TM, OptLevel) {}

  bool SelectAddrFI(SDValue Addr, SDValue &Base, SDValue &Offset);

  // Generated from SebosInstrInfo.td's Pat<>/Pattern entries.
  void Select(SDNode *N) override;

#include "SebosGenDAGISel.inc"
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSISELDAGTODAG_H