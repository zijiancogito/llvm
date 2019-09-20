set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(clang-yaml
  clang-yaml.cpp
  )

clang_target_link_libraries(clang-yaml
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangRewrite
  clangTooling
  clangAnalysis
  )

install(TARGETS clang-yaml
  RUNTIME DESTINATION bin)