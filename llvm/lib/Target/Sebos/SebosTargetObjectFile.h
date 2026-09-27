// SebosTargetObjectFile.h
#ifndef LLVM_LIB_TARGET_SEBOS_SEBOSTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_SEBOS_SEBOSTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class SebosTargetObjectFile : public TargetLoweringObjectFileELF {};
} // namespace llvm

#endif