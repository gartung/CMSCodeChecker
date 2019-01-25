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
  std::string edmhandle = "edm::Handle";
  std::string gethandle = "getHandle";
  std::string edmevent = "edm::Event";

  const auto *matchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member");
  if (matchedCallExpr){
    std::string tvname;
    std::string hvname;
    std::string ttypename;
    std::string dname;
    std::string fname;
    SourceLocation declstart;
    SourceRange declrange;
    auto matchedDecl = matchedCallExpr->getMethodDecl();
    auto implicitObjectExpr = matchedCallExpr->getImplicitObjectArgument();
    auto implicitObjectDecl = llvm::dyn_cast<DeclRefExpr>(implicitObjectExpr)->getFoundDecl();
    auto ioname=implicitObjectDecl->getNameAsString();
    auto matchedName = matchedDecl->getNameAsString();
    auto callstart = matchedCallExpr->getLocStart();
    auto callrange = matchedCallExpr->getSourceRange();
    if (matchedName.compare(getbytoken) == 0) {
      for (auto I: matchedCallExpr->arguments()) {
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
             declstart = D->getLocStart();
             declrange = D->getSourceRange();
             std::string buffer;
             llvm::raw_string_ostream output(buffer);
             temptype->dump(output);
             ttypename=output.str();
         }
      }
      diag(declstart, StringRef("use function "+ioname+"." + gethandle + "("+fname+") to initialize " + edmhandle +"<"+ttypename+"> "+dname), DiagnosticIDs::Warning)
       << FixItHint::CreateReplacement(declrange,StringRef(edmhandle +"<"+ttypename+"> "+dname+" = "+ioname+"."+gethandle+"("+fname+")"));
      diag(callstart, "function " + getbytoken +"("+fname+", "+dname+") is deprecated and should be removed and replaced with "+ gethandle + "("+fname+") as shown above.", DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef("//"));
    }
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
