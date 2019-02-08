//===--- HandleCheck.h - clang-tidy------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CMS_HANDLE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CMS_HANDLE_H

#include "../ClangTidy.h"
#include <llvm/Support/SaveAndRestore.h>

namespace clang {
namespace tidy {
namespace cms {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/cms-handle.html
class HandleCheck : public ClangTidyCheck {
public:
  HandleCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  enum calltype {direct,nested,ifpar,varinit,boolret};
  void report(CXXMemberCallExpr const * matchedCallExpr, calltype);

};

} // namespace cms
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CMS_HANDLE_H
