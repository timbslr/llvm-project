//===-- SebosRegisterInfo.td - Sebos Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Num matches the 3-bit register field used in Sebos's 8-bit instruction
// encoding.
class SebosReg<bits<3> num, string n> : Register<n> {
  field bits<3> Num = num;
  let Namespace = "Sebos";
}

let Namespace = "Sebos" in {
  def sub_lo : SubRegIndex<8>;
  def sub_hi : SubRegIndex<8, 8>;
}

//===----------------------------------------------------------------------===//
// Compiler-visible general-purpose / accumulator registers
//
// Encodings below match the fixed hardware numbering:
//   A => 0b000, TMP => 0b001, B => 0b010, C => 0b011, X => 0b100, Y => 0b101
//===----------------------------------------------------------------------===//

// ALU accumulator: fixed destination/source for arithmetic, but a
// register like any other otherwise.
def A : SebosReg<0b000, "a">;
def TMP : SebosReg<0b001, "tmp">;
def B : SebosReg<0b010, "b">;
def C : SebosReg<0b011, "c">;
def X : SebosReg<0b100, "x">;
def Y : SebosReg<0b101, "y">;

// X:Y combined as a 16-bit address/pointer register. Sub-registers of one
// physical unit so the allocator tracks the overlap between X/Y used as
// independent 8-bit scalars and XY used as a pointer pair.
let SubRegIndices = [sub_hi, sub_lo], CoveredBySubRegs = 1 in
def XY : RegisterWithSubRegs<"xy", [X, Y]>;

//===----------------------------------------------------------------------===//
// Non-allocatable registers
//
// Never assigned to hold a compiler-generated value directly. Present here
// only so instructions can reference them as explicit physical registers
// (implicit Uses/Defs); deliberately not members of any RegisterClass below.
// Marked reserved in SebosRegisterInfo.cpp's getReservedRegs().
//===----------------------------------------------------------------------===//

def SP_L : SebosReg<0b110, "sp_l">;
def SP_H : SebosReg<0b111, "sp_h">;
let SubRegIndices = [sub_hi, sub_lo], CoveredBySubRegs = 1 in
def SP : RegisterWithSubRegs<"sp", [SP_H, SP_L]>;

def PC_L : SebosReg<0b000, "pc_l">; // encoding N/A -- not addressed as an operand
def PC_H : SebosReg<0b000, "pc_h">;
let SubRegIndices = [sub_hi, sub_lo], CoveredBySubRegs = 1 in
def PC : RegisterWithSubRegs<"pc", [PC_H, PC_L]>;

def MAR_L : SebosReg<0b000, "mar_l">;
def MAR_H : SebosReg<0b000, "mar_h">;
let SubRegIndices = [sub_hi, sub_lo], CoveredBySubRegs = 1 in
def MAR : RegisterWithSubRegs<"mar", [MAR_H, MAR_L]>;

def BUF : SebosReg<0b000, "buf">;
def IR  : SebosReg<0b000, "ir">;

def SER_RX : SebosReg<0b000, "ser_rx">;
def SEG7   : SebosReg<0b000, "seg7">;

def FLAGS : SebosReg<0b000, "flags">;

//===----------------------------------------------------------------------===//
// Register classes
//===----------------------------------------------------------------------===//

def GPR8  : RegisterClass<"Sebos", [i8], 8, (add A, B, C, X, Y, TMP)>;
def PTR16 : RegisterClass<"Sebos", [i16], 16, (add XY)>;