#include <sstream>
#include <string>
#include "clang/Basic/LangOptions.h"
#include "clang/AST/AST.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Refactoring/RecursiveSymbolVisitor.h"

#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Analysis/CFG.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");


class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
	public:
		MyASTVisitor(ASTContext &C, Rewriter &R) : TheContext(C), TheRewriter(R) {}

		bool VisitStmt(Stmt *s) {
			return true;
		}


		bool VisitDecl(Decl *d) {
            const NamedDecl *nd = dyn_cast<NamedDecl>(d);
            if(d->isFunctionOrFunctionTemplate()){
                if(d->hasBody()){
                    llvm::outs() << "function:\t";
                    llvm::outs() << nd->getDeclKindName() << "\t";
                    llvm::outs() << nd->getQualifiedNameAsString() << "\n";
                }
            }
            else if(const VarDecl *vd = dyn_cast<VarDecl>(d)) {
                if(vd->isLocalVarDecl()){
                    const NamedDecl *tmpnd = dyn_cast<NamedDecl>(vd->getParentFunctionOrMethod());
                    //llvm::outs() << "localvar:\t";
                    llvm::outs() << tmpnd->getNameAsString()<< ":\t";
                    llvm::outs() << nd->getDeclKindName() << "\t";
                    llvm::outs() << nd->getQualifiedNameAsString() << "\n";
                }
                else if(const ParmVarDecl *pvd = dyn_cast<ParmVarDecl>(d)){
                    const NamedDecl *tmpnd = dyn_cast<NamedDecl>(vd->getParentFunctionOrMethod());
                    //llvm::outs() << "localvar:\t";
                    llvm::outs() << tmpnd->getNameAsString()<< ":\t";
                    llvm::outs() << nd->getDeclKindName() << "\t";
                    llvm::outs() << nd->getQualifiedNameAsString() << "\n";
                }
                else {
                    llvm::outs() << "Global:\t";
                    llvm::outs() << nd->getDeclKindName() << "\t";
                    llvm::outs() << nd->getQualifiedNameAsString() << "\n";
                }
            }
            return true;
		}

	private:
		ASTContext &TheContext;
		Rewriter &TheRewriter;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
	public:
		MyASTConsumer(ASTContext &C, Rewriter &R) : Visitor(C, R) { }
            //Visitor.TraverseAST(C);

		// Override the method that gets called for each parsed top-level
		// declaration.
        void HandleTranslationUnit(ASTContext &Ctx) override{
            // DeclContext::decl_range Decls = Ctx.getTranslationUnitDecl()->decls();
            // for(DeclContext::decl_iterator b=Decls.begin(), e=Decls.end(); b!=e; ++b) {
            //     llvm::outs() << b->getDeclKindName() << "\n";
            //     llvm::outs() << (*b)->getDeclContext()->getDeclKindName() << '\n';
            //     llvm::outs() << "+---------------------------------------+\n";
            //     Visitor.TraverseAST((*b)->getASTContext());
                
            // }
            TranslationUnitDecl *TDecl = Ctx.getTranslationUnitDecl();
            Visitor.TraverseAST(Ctx);
        }
		
        void HandleInterestingDecl(DeclGroupRef DR) override {
			for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
				// Traverse the declaration using our AST visitor.
                //llvm::outs() << (*b)->getDeclKindName();
				// Visitor.TraverseDecl(*b);
                // Visitor.VisitAllDecl(*b);
				//(*b)->dump();
			}
        }
        
	private:
		MyASTVisitor Visitor;
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
