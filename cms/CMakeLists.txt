set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyCMSModule
  CMSTidyModule.cpp
  HandleCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyUtils
  clangTooling
  )

