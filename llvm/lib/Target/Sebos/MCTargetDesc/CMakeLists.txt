add_llvm_component_library(LLVMSebosDesc
  SebosMCTargetDesc.cpp
  SebosMCAsmInfo.cpp
  SebosInstPrinter.cpp

  LINK_COMPONENTS
  MC
  SebosInfo
  Support

  ADD_TO_COMPONENT
  Sebos
  )