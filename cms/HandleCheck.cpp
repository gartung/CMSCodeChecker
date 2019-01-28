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

const std::string edmgettoken = "edm::EDGetTokenT";
const std::string getbytoken = "getByToken";
const std::string edmhandle = "Handle";
const std::string gethandle = "getHandle";
const std::string edmevent = "edm::Event";



void HandleCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  Finder->addMatcher(cxxMemberCallExpr(
                       callee(
                         cxxMethodDecl(
                           hasName(getbytoken),
                           ofClass(
                             hasName(edmevent))
                         )
                       ),
                       argumentCountIs(2) //,
                       //hasAnyArgument(declRefExpr()),
                       //hasAnyArgument(cxxConstructExpr(hasAnyArgument(declRefExpr())))
                     ).bind("cxxmembercallexpr"),this);
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *matchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("cxxmembercallexpr");
  if (matchedCallExpr){
//    matchedCallExpr->dump();
    std::string tvname;
    std::string hvname;
    std::string ttypename;
    std::string dname;
    std::string fname;
    std::string ioname;
    SourceLocation declstart;
    SourceRange declrange;
    auto matchedDecl = matchedCallExpr->getMethodDecl();
    auto matchedName = matchedDecl->getNameAsString();
    auto callstart = matchedCallExpr->getLocStart();
    auto callrange = matchedCallExpr->getSourceRange();
    if (matchedName.compare(getbytoken) == 0) {
      auto implicitObjectExpr = matchedCallExpr->getImplicitObjectArgument();
      auto implicitObjectDecl = llvm::dyn_cast<DeclRefExpr>(implicitObjectExpr)->getFoundDecl();
      ioname=implicitObjectDecl->getNameAsString();
      for (auto I: matchedCallExpr->arguments()) {
         auto qualtype = I->getType();
         auto type = qualtype.getTypePtr();
         auto tstype = type->getAs<TemplateSpecializationType>();
         auto temptype = tstype->getArgs();
         auto iname = qualtype.getAsString();

         if ( iname.compare(0,edmgettoken.size(),edmgettoken) == 0 ) {
             for (auto D: llvm::dyn_cast<CXXConstructExpr>(I)->arguments()) {
                 auto F=llvm::dyn_cast<DeclRefExpr>(D)->getFoundDecl();
                 fname=F->getNameAsString();
             }
         }    
         if ( iname.find(edmhandle,0) != std::string::npos ) { 
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
      diag(declstart, StringRef("use function "+ioname+"." + gethandle + "("+fname+") to initialize edm::" + edmhandle +"<"+ttypename+"> "+dname), DiagnosticIDs::Warning)
       << FixItHint::CreateReplacement(declrange,StringRef("edm::"+edmhandle +"<"+ttypename+"> "+dname+" = "+ioname+"."+gethandle+"("+fname+")"));
      diag(callstart, "function " + getbytoken +"("+fname+", "+dname+") is deprecated and should be removed and replaced with "+ gethandle + "("+fname+") as shown above.", DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef("//"));
    }
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
