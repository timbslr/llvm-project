// Sebos.td

include "llvm/Target/Target.td"
include "SebosRegisterInfo.td"
include "SebosInstrInfo.td"

defm : RemapAllTargetPseudoPointerOperands<PTR16>;

def : ProcessorModel<"generic", NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def Sebos : Target {
  let InstructionSet = SebosInstrInfo;
}