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
    SourceLocation declstart;
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
             llvm::errs() <<edmgettoken<<" type found\n";
             for (auto D: llvm::dyn_cast<CXXConstructExpr>(I)->arguments()) {
                 auto F=llvm::dyn_cast<DeclRefExpr>(D)->getFoundDecl();
                 llvm::errs() << "token var name : ";
                 fname=F->getNameAsString();
                 llvm::errs() << fname<< "\n";
             }
         }    
         if ( iname.compare(0,edmhandle.size(),edmhandle) == 0 ) { 
             llvm::errs() << edmhandle <<" type found\n";
             auto R = llvm::dyn_cast<DeclRefExpr>(I);
             auto D = R->getFoundDecl();
             llvm::errs() << "handle var name : ";
             dname=D->getNameAsString();
             llvm::errs() << dname<< "\n";
             declstart = D->getBeginLoc();
             declend = D->getEndLoc();
             llvm::errs() << "template type: ";
             llvm::raw_string_ostream output(ttypename);
             temptype->dump(output);
             output.str();
             llvm::errs() <<ttypename<<"\n";
         }
      }
      diag(declend, "use function iEvent." + gethandle + "("+fname+") to initialize " + edmhandle +"<"+ttypename+">", DiagnosticIDs::Warning)
       << FixItHint::CreateInsertion(declend," = iEvent."+gethandle+"("+fname+");");
      diag(callstart, "function " + getbytoken +"("+fname+", "+dname+") is deprecated and should be removed and replaced with "+ gethandle + "("+fname+") as shown above.", DiagnosticIDs::Warning)
        << FixItHint::CreateRemoval(callrange);
    }
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
