//===-- SebosMCTargetDesc.h - Sebos Target Descriptions --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_MCTARGETDESC_SEBOSMCTARGETDESC_H
#define LLVM_LIB_TARGET_SEBOS_MCTARGETDESC_SEBOSMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {

class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCAsmInfo;
class Target;

Target &getTheSebosTarget();

MCInstrInfo *createSebosMCInstrInfo();
MCRegisterInfo *createSebosMCRegisterInfo();

} // namespace llvm

#define GET_REGINFO_ENUM
#include "SebosGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "SebosGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "SebosGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_SEBOS_MCTARGETDESC_SEBOSMCTARGETDESC_H