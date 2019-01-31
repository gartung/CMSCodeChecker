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

const std::string edmgettoken = "EDGetTokenT";
const std::string getbytoken = "getByToken";
const std::string edmhandle = "Handle";
const std::string gethandle = "getHandle";
const std::string edmevent = "edm::Event";
const std::string thisp = "this->";


void HandleCheck::registerMatchers(MatchFinder *Finder) {
  auto edmGetTokenT = namedDecl(hasName("edm::EDGetTokenT"));
  auto edmHandle = namedDecl(hasName("edm::Handle"));
  auto edmEvent = namedDecl(hasName("edm::Event"));
  Finder->addMatcher(cxxMemberCallExpr(
                       callee(
                         cxxMethodDecl(
                           hasName(getbytoken),
                           ofClass(
                             hasName(edmevent))
                         )
                       ),
                       argumentCountIs(2) ,
                       hasAnyArgument(declRefExpr()),
                       hasAnyArgument(cxxConstructExpr())
                     ).bind("cxxmembercallexpr"),this);
  Finder->addMatcher(declRefExpr(hasDeclaration(varDecl()),hasType(edmHandle)).bind("varexprhandle"),this);
  Finder->addMatcher(declRefExpr(hasDeclaration(fieldDecl()),hasType(edmHandle)).bind("memexprhandle"),this);
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *matchedRefExprVar = Result.Nodes.getNodeAs<DeclRefExpr>("varexprhandle");
  if (matchedRefExprVar) {
    llvm::errs() <<"++++++++++++++++matched+mem+expr+handle\n";
    matchedRefExprVar->getFoundDecl()->dump();
    llvm::errs() <<"\n";
   }
  const auto *matchedRefExprMem = Result.Nodes.getNodeAs<DeclRefExpr>("memexprhandle");
  if (matchedRefExprMem) {
    llvm::errs() <<"++++++++++++++++matched+var+expr+handle\n";
    matchedRefExprMem->getFoundDecl()->dump();
    llvm::errs() <<"\n";
   } 
  const auto *matchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("cxxmembercallexpr");
  if (matchedCallExpr){
    //llvm::errs() <<"++++++++++++++++matchedcallexpr\n";
    //matchedCallExpr->dump();
    //llvm::errs() <<"\n";
    clang::LangOptions LangOpts;
    LangOpts.CPlusPlus = true;
    clang::PrintingPolicy Policy(LangOpts);
    std::string tvname;
    std::string hvname;
    std::string ttemptype;
    std::string dname;
    std::string fname;
    std::string qname;
    std::string ioname;
    SourceLocation declstart;
    SourceRange declrange;
    auto matchedDecl = matchedCallExpr->getMethodDecl();
    auto matchedName = matchedDecl->getNameAsString();
    auto callstart = matchedCallExpr->getLocStart();
    auto callrange = matchedCallExpr->getSourceRange();
    auto implicitObjectExpr = matchedCallExpr->getImplicitObjectArgument();
    std::string bufferi;
    llvm::raw_string_ostream outputi(bufferi);
    implicitObjectExpr->printPretty(outputi,0,Policy);
    ioname=outputi.str();
    for (auto I: matchedCallExpr->arguments()) {
       auto qualtype = I->getType();
       auto type = qualtype.getTypePtr();
       auto tstype = type->getAs<TemplateSpecializationType>();
       auto temptype = tstype->getArgs();
       std::string buffert;
       llvm::raw_string_ostream outputt(buffert);
       temptype->print(Policy,outputt);
       ttemptype=outputt.str();
       auto iname = qualtype.getAsString();
       if ( iname.find(edmhandle,0) != std::string::npos ) { 
             std::string buffer;
             llvm::raw_string_ostream output(buffer);
             I->printPretty(output,0,Policy);
             qname=output.str();
       }
       if ( iname.find(edmgettoken,0) != std::string::npos ) {
             clang::LangOptions LangOpts;
             LangOpts.CPlusPlus = true;
             clang::PrintingPolicy Policy(LangOpts);
             std::string buffer;
             llvm::raw_string_ostream output(buffer);
             I->printPretty(output,0,Policy);
             fname=output.str();
             auto n = fname.find(thisp,0);
             if (n != std::string::npos) {
                 fname.erase(n,thisp.size());
             }
       }    
    }
      diag(callstart, StringRef("function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be replaced with "+ gethandle + "("+edmgettoken+"<"+ttemptype+">&) as shown."), DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef(qname+" = "+ioname+"."+gethandle+"("+fname+")"));
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
