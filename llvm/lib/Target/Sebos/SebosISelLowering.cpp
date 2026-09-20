//===-- SebosISelLowering.cpp - Sebos DAG Lowering Implementation -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SebosISelLowering.h"
#include "SebosSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

#include "SebosGenCallingConv.inc"

SebosTargetLowering::SebosTargetLowering(const TargetMachine &TM,
                                          const SebosSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  addRegisterClass(MVT::i8, &Sebos::GPR8RegClass);
  addRegisterClass(MVT::i16, &Sebos::PTR16RegClass);

  computeRegisterProperties(STI.getRegisterInfo());

  // No hardware multiply/divide -- fall back to compiler-rt library calls.
  for (auto Op : {ISD::MUL, ISD::SDIV, ISD::UDIV, ISD::SREM, ISD::UREM})
    setOperationAction(Op, MVT::i8, Expand);

  // i16 is a legal register type (it lives in PTR16/XY), but the ALU only
  // operates on 8-bit A/TMP -- every i16 arithmetic/logic op needs
  // explicit handling rather than the default "assume hardware does this
  // in one instruction" behavior.
  setOperationAction(ISD::ADD, MVT::i16, Custom);
  setOperationAction(ISD::SUB, MVT::i16, Custom);

  // TODO: AND/OR/XOR/comparisons on i16 aren't handled yet -- each needs
  // similar treatment to lowerADDorSUB16 (extract sub_lo/sub_hi, apply
  // the 8-bit op twice via the ADDPSEUDO-style pseudos from
  // SebosInstrInfo, recombine via REG_SEQUENCE). Flagging rather than
  // guessing at each one here; happy to do these as a follow-up pass.

  setOperationAction(ISD::SHL, MVT::i8, Custom);
  setOperationAction(ISD::SRL, MVT::i8, Custom);
  setOperationAction(ISD::SRA, MVT::i8, Custom);
  // NOTE: hardware only shifts by exactly 1 (see SHLPSEUDO etc. in
  // SebosInstrInfo.td, which only match a literal shift amount of 1).
  // A variable-amount shift needs to be expanded into a runtime loop of
  // single-bit shifts -- not yet implemented; the Custom hook above is a
  // placeholder that will need a real lowerVariableShift() once a test
  // case actually exercises a non-constant shift amount.
}

//===----------------------------------------------------------------------===//
// i16 add/sub -- split into two 8-bit operations with carry propagation,
// the same ripple pattern already verified correct in ADDISPU/SUBISPU and
// LDSPRELU/STSPRELU's own microcode.
//===----------------------------------------------------------------------===//

SDValue SebosTargetLowering::lowerADDorSUB16(SDValue Op, SelectionDAG &DAG,
                                              bool IsAdd) const {
  SDLoc DL(Op);
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);

  SDValue LHSLo = DAG.getTargetExtractSubreg(Sebos::sub_lo, DL, MVT::i8, LHS);
  SDValue LHSHi = DAG.getTargetExtractSubreg(Sebos::sub_hi, DL, MVT::i8, LHS);
  SDValue RHSLo = DAG.getTargetExtractSubreg(Sebos::sub_lo, DL, MVT::i8, RHS);
  SDValue RHSHi = DAG.getTargetExtractSubreg(Sebos::sub_hi, DL, MVT::i8, RHS);

  // ISD::ADDC/ADDE (and SUBC/SUBE) are the classic carry-chain node pair:
  // the first produces a value plus a carry-out glue result; the second
  // consumes that carry-in glue alongside its own operands.
  SDVTList VTs = DAG.getVTList(MVT::i8, MVT::Glue);
  unsigned LoOpc = IsAdd ? ISD::ADDC : ISD::SUBC;
  unsigned HiOpc = IsAdd ? ISD::ADDE : ISD::SUBE;

  SDValue Lo = DAG.getNode(LoOpc, DL, VTs, LHSLo, RHSLo);
  SDValue Hi = DAG.getNode(HiOpc, DL, VTs, LHSHi, RHSHi, Lo.getValue(1));

  SDValue Undef = SDValue(
      DAG.getMachineNode(TargetOpcode::IMPLICIT_DEF, DL, MVT::i16), 0);
  SDValue Result = DAG.getTargetInsertSubreg(Sebos::sub_lo, DL, MVT::i16,
                                              Undef, Lo);
  Result = DAG.getTargetInsertSubreg(Sebos::sub_hi, DL, MVT::i16, Result, Hi);
  return Result;
}

SDValue SebosTargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::ADD:
    return lowerADDorSUB16(Op, DAG, /*IsAdd=*/true);
  case ISD::SUB:
    return lowerADDorSUB16(Op, DAG, /*IsAdd=*/false);
  default:
    llvm_unreachable("Sebos: unimplemented custom lowering for this opcode");
  }
}

//===----------------------------------------------------------------------===//
// Calling convention.
//===----------------------------------------------------------------------===//

SDValue SebosTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Sebos);

  for (const CCValAssign &VA : ArgLocs) {
    assert(VA.isMemLoc() && "Sebos: CC_Sebos only ever assigns stack slots");
    unsigned Size = VA.getLocVT().getStoreSize();
    // isImmutable=true: incoming argument slots are never written back to
    // by the callee touching the caller's copy.
    int FI = MFI.CreateFixedObject(Size, VA.getLocMemOffset(),
                                    /*isImmutable=*/true);
    SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue Load = DAG.getLoad(VA.getLocVT(), DL, Chain, FIN,
                                MachinePointerInfo::getFixedStack(MF, FI));
    InVals.push_back(Load);
  }

  return Chain;
}

SDValue
SebosTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  // TODO: full call lowering (argument marshalling via CC_Sebos, the
  // push/call/stack-teardown sequence, and reading the RetCC_Sebos
  // result out of A/XY afterward) still needs to be written -- this is
  // real, substantial work in its own right, deliberately not guessed at
  // here rather than shipped half-correct.
  report_fatal_error("Sebos: calls are not yet implemented");
}

SDValue SebosTargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_Sebos);

  SmallVector<SDValue, 4> RetOps(1, Chain);
  for (unsigned i = 0, e = RVLocs.size(); i != e; ++i) {
    const CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Sebos: RetCC_Sebos only ever assigns registers");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i]);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }
  RetOps[0] = Chain;

  return DAG.getNode(SebosISD::RET_FLAG, DL, MVT::Other, RetOps);
}