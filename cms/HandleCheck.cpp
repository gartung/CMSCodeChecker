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
                           ofClass(cxxRecordDecl(
                             hasName(edmevent)))
                         );

  auto getByTokenCallVarInit = varDecl(
                                hasInitializer(anyOf(
                                  hasDescendant( 
                                   cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprboolreturn")),
                                  cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprboolreturn")
                                  )));

  auto getByTokenCallAssign = binaryOperator(
                                  isAssignmentOperator(),
                                  hasRHS(
                                  cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprboolreturn")
                                  ));


  auto getByTokenCallIfPar = ifStmt(
                              hasCondition(anyOf(
                                hasDescendant(
                                   cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprifpar")),
                               cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprifpar")
                               )));

  auto getByTokenCallRetPar = returnStmt(
                                hasReturnValue(
                                   cxxMemberCallExpr(
                                     callee(getByTokenDecl),
                                     argumentCountIs(2),
                                     hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT))),
                                       anyOf(
                                         hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                                         hasAnyArgument(ignoringParenImpCasts(memberExpr())))
                                   ).bind("getbytokencallexprboolreturn"))
                               );

  auto getByTokenCall = cxxMemberCallExpr(
                          callee(getByTokenDecl),
                          argumentCountIs(2),
                          anyOf(
                            hasAnyArgument(ignoringParenImpCasts(declRefExpr())),
                            hasAnyArgument(ignoringParenImpCasts(memberExpr()))),
                          unless(hasAncestor(getByTokenCallIfPar)),
                          unless(hasAncestor(getByTokenCallAssign)),
                          unless(hasAncestor(getByTokenCallRetPar)),
                          unless(hasAncestor(getByTokenCallVarInit)),  
                          hasArgument(0,cxxConstructExpr(hasType(edmGetTokenT)))
                        ).bind("getbytokencallexpr");


  Finder->addMatcher(getByTokenCall,this);
  Finder->addMatcher(getByTokenCallVarInit,this);
  Finder->addMatcher(getByTokenCallAssign,this);
  Finder->addMatcher(getByTokenCallIfPar,this);
  Finder->addMatcher(getByTokenCallRetPar,this);
}

void HandleCheck::report(CXXMemberCallExpr const * matchedCallExpr, calltype ct) {
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
    auto callstart = matchedCallExpr->getBeginLoc();
    auto callend = matchedCallExpr->getEndLoc();
    auto implicitObjectExpr = matchedCallExpr->getImplicitObjectArgument();
    auto callee = matchedCallExpr->getCallee();
    std::string bufferi;
    llvm::raw_string_ostream outputi(bufferi);
    callee->printPretty(outputi,0,Policy);
    ioname=outputi.str();
    auto n = ioname.find(getbytoken,0);
    if (n != std::string::npos) {
        ioname.erase(n,getbytoken.size());
    }
    auto p = ioname.find(thisp,0);
    if (p != std::string::npos) {
        ioname.erase(p,thisp.size());
    }


    
    for (auto I: matchedCallExpr->arguments()) {
       auto qualtype = I->getType();
       //auto type = qualtype.getTypePtr();
       //auto tstype = type->getAs<TemplateSpecializationType>();
       //auto temptype = tstype->getArgs();
       //std::string buffert;
       //llvm::raw_string_ostream outputt(buffert);
       //temptype->print(Policy,outputt);
       //ttemptype=outputt.str();
       auto iname = qualtype.getAsString();
       if (iname.find(edmhandle,0) != std::string::npos) {
          if (const auto * R = llvm::dyn_cast<DeclRefExpr>(I)) {
             auto D = R->getDecl();
             dname = D->getNameAsString();
             declend = D->getEndLoc().getLocWithOffset(dname.size());
             declrange = D->getSourceRange();
             std::string buffer;
             llvm::raw_string_ostream output(buffer);
             I->printPretty(output,0,Policy);
             qname=output.str();
          }
          else if ( const auto * R = llvm::dyn_cast<MemberExpr>(I)) {
             auto D = R->getMemberDecl();
             dname = D->getNameAsString();
             declend = D->getEndLoc().getLocWithOffset(dname.size());
             declrange = D->getSourceRange();
             std::string buffer;
             llvm::raw_string_ostream output(buffer);
             I->printPretty(output,0,Policy);
             qname=output.str();
             auto n = qname.find(thisp,0);
             if (n != std::string::npos) {
                 qname.erase(n,thisp.size());
             }
          }
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
    switch (ct) {
      case ifpar: {
        auto callrange = SourceRange(callstart,callend);
        replacement = "("+qname+" = "+ioname+gethandle + "("+fname+"))";
        diag(callstart, StringRef("bool return call of function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be replaced here with ("+qname+" = "+ioname+ gethandle + "("+fname+"))."), DiagnosticIDs::Warning)
        << FixItHint::CreateReplacement(callrange, StringRef(replacement));
        break;
      };

      case boolret: {
        auto callrange = SourceRange(callstart,callend);
        replacement = "bool("+qname+" = "+ioname+gethandle + "("+fname+"))";
        diag(callstart, StringRef("bool return call of function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be replaced here with bool("+ qname + " = "+ioname+ gethandle + "("+fname+"))."), DiagnosticIDs::Warning)
          << FixItHint::CreateReplacement(callrange, StringRef(replacement));
        break;
      }; 

      case direct : {
        auto callrange = SourceRange(callstart,callend);
        replacement = qname+" = "+ioname+gethandle + "("+fname+")";
        diag(callstart, StringRef("direct call of function " + getbytoken +"("+edmgettoken+"<"+ttemptype+">&, "+edmhandle+"<"+ttemptype+">&) is deprecated and should be replaced here with "+ qname + " = "+ioname+ gethandle + "("+fname+")."), DiagnosticIDs::Warning)
          << FixItHint::CreateReplacement(callrange, StringRef(replacement));
        break;
      };
    }
  }
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *matchedCallExprIfPar = Result.Nodes.getNodeAs<CXXMemberCallExpr>("getbytokencallexprifpar")){
         report(matchedCallExprIfPar,ifpar);
  }
  if (const auto *matchedCallExprNested = Result.Nodes.getNodeAs<CXXMemberCallExpr>("getbytokencallexprboolreturn")){
         report(matchedCallExprNested,boolret);
  }
  if (const auto *matchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("getbytokencallexpr")) {
         report(matchedCallExpr,direct);
  }
}


} // namespace cms
} // namespace tidy
} // namespace clang
