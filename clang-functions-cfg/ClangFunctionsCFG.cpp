#include <sstream>
#include <string>
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Analysis/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
	public:
		MyASTConsumer(ASTContext &C, Rewriter &R) : TheContext(C) {}

		// Override the method that gets called for each parsed top-level
		// declaration.
        
        void HandleInlineFunctionDefinition(FunctionDecl *D) override {
            // std::cout << "function-decl: " << D->getQualifiedNameAsString() << " " << D->getNameAsString() << "\n";
            // std::cout << "function-decl: \"" << D->getNameAsString() << "\"\n";
            if (D->hasBody()) {
			    Stmt *funcBody = D->getBody();
				//CFG 
                // Print function name
                llvm::outs() << "function-decl: " << D->getQualifiedNameAsString() << " " << D->getNameAsString() << "\n"; 
			    // Print param
				llvm::outs() << "param-decl: ";
				for(clang::FunctionDecl::param_const_iterator pb = D->param_begin(), pe = D->param_end(); pb!=pe; ++pb){
					const ParmVarDecl *P = *pb;
					// llvm::outs() << P->getNameAsString() << '\n';
					llvm::outs() << P->getType().getAsString() << "; ";
                }
				llvm::outs() << "\n";
				std::unique_ptr<CFG> sourceCFG = CFG::buildCFG(D, funcBody, &TheContext, CFG::BuildOptions());
			    // Print function CFG
                sourceCFG->print(llvm::outs(), LangOptions(), false);
            }
        }
		bool HandleTopLevelDecl(DeclGroupRef DR) override {
			for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
				const Decl *D = *b;
                if (const FunctionDecl *ND = dyn_cast<FunctionDecl>(D)){
                    // std::cout << "function-decl: " << ND->getQualifiedNameAsString() << ' ' << ND->getNameAsString() << "\n";

                    if (ND->hasBody()) {
				        Stmt *funcBody = ND->getBody();
				        //CFG 
                        // Print function name
                        llvm::outs() << "function-decl: " << ND->getQualifiedNameAsString() << ' ' << ND->getNameAsString() << "\n";
				        // Print param
						llvm::outs() << "param-decl: ";
                        for(clang::FunctionDecl::param_const_iterator pb = ND->param_begin(), pe = ND->param_end(); pb!=pe; ++pb){
							const ParmVarDecl *P = *pb;
							// llvm::outs() << P->getNameAsString() << '\n';
							llvm::outs() << P->getType().getAsString() << "; ";
                        }
                        llvm::outs() << '\n';
                        std::unique_ptr<CFG> sourceCFG = CFG::buildCFG(ND, funcBody, &TheContext, CFG::BuildOptions());
				        // Print function CFG
                        sourceCFG->print(llvm::outs(), LangOptions(), false);

			        }
                }
			}
			return true;
		}

	private:
		// MyASTVisitor Visitor;
        ASTContext &TheContext;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
	public:
		MyFrontendAction() {}

		std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
			TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
			return llvm::make_unique<MyASTConsumer>(CI.getASTContext(), TheRewriter);
		}

	private:
		Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
	CommonOptionsParser op(argc, argv, ToolingSampleCategory);
	ClangTool Tool(op.getCompilations(), op.getSourcePathList());

	// ClangTool::run accepts a FrontendActionFactory, which is then used to
	// create new objects implementing the FrontendAction interface. Here we use
	// the helper newFrontendActionFactory to create a default factory that will
	// return a new MyFrontendAction object every time.
	// To further customize this, we could create our own factory class.
	return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
