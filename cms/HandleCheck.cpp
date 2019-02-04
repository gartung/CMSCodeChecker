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
  auto edmGetTokenT = cxxRecordDecl(hasName("edm::EDGetTokenT"));
  auto edmHandle = cxxRecordDecl(hasName("edm::Handle"));
  auto edmEvent = cxxRecordDecl(hasName("edm::Event"));
  auto edmHandleVarRef = declRefExpr(
                           hasDeclaration(varDecl()),
                           hasType(edmHandle));
  auto edmGetTokenTRef = declRefExpr(
                           hasDeclaration(varDecl()),
                           hasType(edmGetTokenT));
  auto edmEventRef = declRefExpr(
                       hasDeclaration(varDecl()),
                       hasType(edmEvent));
  auto edmHandleVarInit = varDecl(
                            hasInitializer(
                              cxxOperatorCallExpr(hasAnyArgument(edmHandleVarRef),
                                                  hasOverloadedOperatorName("*"))));
  auto getByTokenDecl = cxxMethodDecl(
                           hasName(getbytoken),
                           ofClass(
                             hasName(edmevent))
                         );
  auto getByTokenCall = cxxMemberCallExpr(
                          hasParent(compoundStmt(hasParent(functionDecl()))),
                          callee(getByTokenDecl),
                          argumentCountIs(2),
                          hasAnyArgument(declRefExpr()),
                          hasAnyArgument(cxxConstructExpr())
                     );
  auto getByTokenCallNested = cxxMemberCallExpr(
                          unless(hasParent(compoundStmt(hasParent(functionDecl())))),
                          callee(getByTokenDecl),
                          argumentCountIs(2),
                          hasAnyArgument(declRefExpr()),
                          hasAnyArgument(cxxConstructExpr())
                     );
  Finder->addMatcher(getByTokenCall.bind("getbytokencallexpr"),this);
  Finder->addMatcher(getByTokenCallNested.bind("getbytokencallexprnested"),this);
}

void HandleCheck::report(CXXMemberCallExpr const * matchedCallExpr, std::string const& type) {
  if (matchedCallExpr){
    //llvm::errs() <<"++++++++++++++++matchedcallexpr "+type+"\n";
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
    std::string replacement;
    std::string insertion;
    SourceLocation declstart;
    SourceLocation declend;
    SourceRange declrange;
    auto matchedDecl = matchedCallExpr->getMethodDecl();
    auto matchedName = matchedDecl->getNameAsString();
    auto callstart = matchedCallExpr->getLocStart();
    auto callend = matchedCallExpr->getEndLoc();
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
             auto R = llvm::dyn_cast<DeclRefExpr>(I);
             auto D = R->getDecl();
             dname = D->getNameAsString();
             declend = D->getEndLoc().getLocWithOffset(dname.size());
             declrange = D->getSourceRange();
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
    if ( type == "direct") {    
      insertion = " = "+ioname+"."+gethandle+"("+fname+")";

      diag(declend, StringRef("use function "+ioname+"." + gethandle + "("+fname+") to initialize variable "+dname), DiagnosticIDs::Warning)
     << FixItHint::CreateInsertion(declend,StringRef(insertion));


      auto callrange = SourceRange(callstart,callend.getLocWithOffset(1));
      replacement = "";
      diag(callstart, StringRef("function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be removed here and replaced with "+ gethandle + "("+fname+") to inialize variable "+dname+"."), DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef(replacement));
    } else 
    if (type == "nested") {
      auto callrange = SourceRange(callstart,callend);
      replacement = dname+" = "+ioname+"."+gethandle + "("+fname+")";
      diag(callstart, StringRef("function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be replaced here with "+ gethandle + "("+fname+") to inialize variable "+dname+"."), DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef(replacement));
    }
  }
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *matchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("getbytokencallexpr")) {
      report(matchedCallExpr,"direct");}
  else
  if (const auto *matchedCallExprNested = Result.Nodes.getNodeAs<CXXMemberCallExpr>("getbytokencallexprnested")){
  report(matchedCallExprNested,"nested");}
}


} // namespace cms
} // namespace tidy
} // namespace clang
