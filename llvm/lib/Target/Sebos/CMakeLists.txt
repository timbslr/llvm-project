add_llvm_component_group(Sebos)

set(LLVM_TARGET_DEFINITIONS Sebos.td)

tablegen(LLVM SebosGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM SebosGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM SebosGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM SebosGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM SebosGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(SebosCommonTableGen)

add_llvm_target(SebosCodeGen
  SebosTargetMachine.cpp
  SebosRegisterInfo.cpp
  SebosSubtarget.cpp

  DEPENDS
  SebosCommonTableGen

  LINK_COMPONENTS
  Core
  CodeGen
  MC
  Support
  Target

  ADD_TO_COMPONENT
  Sebos
)

add_subdirectory(TargetInfo)