//===-- SebosTargetInfo.h - Sebos Target Implementation ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SEBOS_TARGETINFO_SEBOSTARGETINFO_H
#define LLVM_LIB_TARGET_SEBOS_TARGETINFO_SEBOSTARGETINFO_H

namespace llvm {

class Target;

/// Returns the singleton Target object representing Sebos.
/// Registered by LLVMInitializeSebosTargetInfo(); returns the same
/// object every call.
Target &getTheSebosTarget();

} // namespace llvm

#endif // LLVM_LIB_TARGET_SEBOS_TARGETINFO_SEBOSTARGETINFO_H