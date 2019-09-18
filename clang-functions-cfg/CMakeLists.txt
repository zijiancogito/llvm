set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(clang-functions-cfg
  ClangFunctionsCFG.cpp
  )

clang_target_link_libraries(clang-functions-cfg
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangRewrite
  clangTooling
  clangAnalysis
  )

install(TARGETS clang-functions-cfg
  RUNTIME DESTINATION bin)