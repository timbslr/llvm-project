//===-- SebosISelLowering.h - Sebos DAG Lowering Interface -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSISELLOWERING_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class SebosSubtarget;

class SebosTargetLowering : public TargetLowering {
  const SebosSubtarget &Subtarget;

public:
  explicit SebosTargetLowering(const TargetMachine &TM,
                                const SebosSubtarget &STI);

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                bool isVarArg,
                                const SmallVectorImpl<ISD::InputArg> &Ins,
                                const SDLoc &DL, SelectionDAG &DAG,
                                SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                     SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                       const SmallVectorImpl<ISD::OutputArg> &Outs,
                       const SmallVectorImpl<SDValue> &OutVals,
                       const SDLoc &DL, SelectionDAG &DAG) const override;

private:
  SDValue lowerADDorSUB16(SDValue Op, SelectionDAG &DAG, bool IsAdd) const;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_SEBOSISELLOWERING_H