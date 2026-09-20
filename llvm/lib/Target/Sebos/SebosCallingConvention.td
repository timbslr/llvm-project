//===-- SebosCallingConv.td - Calling Conventions for Sebos -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Argument passing: everything goes on the stack, regardless of type.
// CCAssignToStack<size, align> reserves `size` bytes at the next
// available, correctly aligned stack slot for this argument.
def CC_Sebos : CallingConv<[
  CCIfType<[i8],  CCAssignToStack<1, 1>>,
  CCIfType<[i16], CCAssignToStack<2, 1>>
]>;

// Return value: i8 results in A, i16 (pointer) results in the XY pair.
def RetCC_Sebos : CallingConv<[
  CCIfType<[i8],  CCAssignToReg<[A]>>,
  CCIfType<[i16], CCAssignToReg<[XY]>>
]>;

// Callee-saved registers -- currently none (matches SebosRegisterInfo.cpp's
// getCalleeSavedRegs() stub). Revisit once register pressure/performance
// considerations make saving something across calls worthwhile.
def CSR_Sebos : CalleeSavedRegs<(add)>;