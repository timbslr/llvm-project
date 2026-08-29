add_llvm_component_library(LLVMSebosInfo
  SebosTargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  Sebos
)