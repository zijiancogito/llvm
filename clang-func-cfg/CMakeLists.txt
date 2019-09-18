set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(clang-func-cfg
  ClangFuncCFG.cpp
  )

clang_target_link_libraries(clang-func-cfg
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangRewrite
  clangTooling
  clangAnalysis
  )

install(TARGETS clang-func-cfg
  RUNTIME DESTINATION bin)