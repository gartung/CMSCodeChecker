//===--- HandleCheck.cpp - clang-tidy--------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "HandleCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>

using namespace clang::ast_matchers;
namespace clang {
namespace tidy {
namespace cms {

void HandleCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  Finder->addMatcher(cxxMemberCallExpr().bind("member"), this);
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  std::string edmgettoken = "edm::EDGetTokenT";
  std::string getbytoken = "getByToken";
  std::string tvname;
  std::string edmhandle = "edm::Handle";
  std::string gethandle = "getHandle";

  const auto *MatchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member");
  if (MatchedCallExpr){
    std::string hvname;
    std::string ttypename;
    std::string dname;
    std::string fname;
    SourceLocation declend;
    auto MatchedDecl = MatchedCallExpr->getMethodDecl();
    auto MatchedName = MatchedDecl->getNameAsString();
    auto callstart = MatchedCallExpr->getLocStart();
    auto callrange = MatchedCallExpr->getSourceRange();
    if (MatchedName.compare(getbytoken) == 0) {
      for (auto I: MatchedCallExpr->arguments()) {
         auto qualtype = I->getType();
         auto type = llvm::dyn_cast<ElaboratedType>(qualtype)->desugar();
         auto temptype=llvm::dyn_cast<TemplateSpecializationType>(type)->getArgs();
         auto iname = qualtype.getAsString();

         if ( iname.compare(0,edmgettoken.size(),edmgettoken) == 0 ) {
             for (auto D: llvm::dyn_cast<CXXConstructExpr>(I)->arguments()) {
                 auto F=llvm::dyn_cast<DeclRefExpr>(D)->getFoundDecl();
                 fname=F->getNameAsString();
             }
         }    
         if ( iname.compare(0,edmhandle.size(),edmhandle) == 0 ) { 
             auto R = llvm::dyn_cast<DeclRefExpr>(I);
             auto D = R->getFoundDecl();
             dname=D->getNameAsString();
             declend = D->getLocation().getLocWithOffset(1);
             llvm::raw_string_ostream output(ttypename);
             output.str();
         }
      }
      diag(declend, "use function iEvent." + gethandle + "("+fname+") to initialize " + edmhandle +"<"+ttypename+">", DiagnosticIDs::Warning)
       << FixItHint::CreateInsertion(declend,StringRef(" = iEvent."+gethandle+"("+fname+")"));
      diag(callstart, "function " + getbytoken +"("+fname+", "+dname+") is deprecated and should be removed and replaced with "+ gethandle + "("+fname+") as shown above.", DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef("//"));
    }
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
