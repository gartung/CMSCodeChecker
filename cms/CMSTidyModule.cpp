//===--- CMSTidyModule.cpp - clang-tidy ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "HandleCheck.h"

namespace clang {
namespace tidy {
namespace cms {

class CMSModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<HandleCheck>(
        "cms-handle");
  }
};

 
} // namespace cms

// Register the CMSTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<cms::CMSModule> X("cms-module",
						     "Adds CMS specific checks");


// This anchor is used to force the linker to link in the generated object file
// and thus register the CMSModule.
volatile int CMSModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
