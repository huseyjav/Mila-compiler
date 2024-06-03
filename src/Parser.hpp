#ifndef PJPPROJECT_PARSER_HPP
#define PJPPROJECT_PARSER_HPP

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <fstream>
#include <memory>
#include <ostream>

#include "AST.h"
#include "Lexer.hpp"

struct llvmClasses{
    llvm::LLVMContext* MilaContext;   // llvm context
    llvm::IRBuilder<>* MilaBuilder;   // llvm builder
    llvm::Module* MilaModule;         // llvm module
};
extern llvmClasses llvmC;
class Parser {
public:
    Parser();
    Parser(std::ifstream& os);
    ~Parser() = default;
    std::string curFunc;
    
    std::shared_ptr<program> m_Program;
    bool Parse();                    // parse
    const llvm::Module& Generate();  // generate

private:
    int getNextToken();

    Lexer m_Lexer;                   // lexer is used to read tokens
    int CurTok;                      // to keep the current tokens

    llvm::LLVMContext MilaContext;   // llvm context
    llvm::IRBuilder<> MilaBuilder;   // llvm builder
    llvm::Module MilaModule;         // llvm module

    std::shared_ptr<functionDefNode> parseFunctionDefinition();
    std::shared_ptr<functionBodyNode> parseFunction();
    std::shared_ptr<functionBodyNode> parseMainFunction();
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseIfElse();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseBinOp();
    std::shared_ptr<ASTNode> parseNumber();
    std::shared_ptr<ASTNode> parseIdentifier();
    std::vector<std::shared_ptr<ASTNode>> parseCodeBlock();
};


#endif //PJPPROJECT_PARSER_HPP
