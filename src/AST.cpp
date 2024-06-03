#include <AST.h>
#include <llvm-16/llvm/IR/Value.h>

#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "Parser.hpp"
binOperatorNode::binOperatorNode(ASTNode* parent, std::shared_ptr<ASTNode> lhs,
                                 std::shared_ptr<ASTNode> rhs, binOps opType)
    : ASTNode(parent), m_Lhs(lhs), m_Rhs(rhs), m_OpType(opType) {
    m_Lhs->setParent(this);
    m_Rhs->setParent(this);
}

functionDefNode::functionDefNode(ASTNode* parent, const std::string& funcName,
                                 const std::vector<std::string>& varNames)
    : ASTNode(parent), m_FuncName(funcName), m_VarNames(varNames) {}

functionBodyNode::functionBodyNode(
    ASTNode* parent, std::shared_ptr<functionDefNode> definition,
    const std::vector<std::shared_ptr<ASTNode>>& body)
    : ASTNode(parent), m_Definition(definition), m_Body(body) {
    m_Definition->setParent(this);
    for (auto i : m_Body) i->setParent(this);
}

functionBodyNode* ASTNode::getParentFunc() { return m_Parent->getParentFunc(); }

functionBodyNode* functionBodyNode::getParentFunc() { return this; }

ASTNode::ASTNode(ASTNode* parent) : m_Parent(parent) {}

ifElseNode::ifElseNode(ASTNode* parent, std::shared_ptr<ASTNode> condition,
                       std::shared_ptr<ASTNode> ifExpr,
                       std::shared_ptr<ASTNode> elseExpr)
    : ASTNode(parent), m_Condition(condition), m_If(ifExpr), m_Else(elseExpr) {
    m_Condition->setParent(this);
    m_If->setParent(this);
    m_Else->setParent(this);
}

numberNode::numberNode(ASTNode* parent, int val)
    : ASTNode(parent), m_Val(val) {}

identifierNode::identifierNode(ASTNode* parent, const std::string& identifier)
    : ASTNode(parent), m_Identifier(identifier) {}

functionCallNode::functionCallNode(
    ASTNode* parent, const std::vector<std::shared_ptr<ASTNode>>& arguments,
    const std::string& functionName)
    : ASTNode(parent), m_Arguments(arguments), m_FunctionName(functionName) {
    for (auto i : m_Arguments) i->setParent(this);
}

void ASTNode::setParent(ASTNode* parent) { m_Parent = parent; }

llvm::Value* ASTNode::codegen() { return nullptr; }
std::string identifierNode::getId() { return m_Identifier; }
returnNode::returnNode(ASTNode* parent, std::shared_ptr<ASTNode> retval)
    : ASTNode(nullptr), m_RetVal(retval) {
    m_RetVal->setParent(this);
}

llvm::Value* functionBodyNode::codegen() {
    std::vector<llvm::Type*> Ints(m_Definition->m_VarNames.size(),
                                  llvm::Type::getInt32Ty(*(llvmC.MilaContext)));
    llvm::FunctionType* FT = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*(llvmC.MilaContext)), Ints, false);
    llvm::Function* F =
        llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                               m_Definition->m_FuncName, *llvmC.MilaModule);
    // TODO: IMPLEMENT MULTIPOLE ARGUMENTS PROPERLY
    for (auto& Arg : F->args()) {
        Arg.setName(m_Definition->m_VarNames[0]);
        m_VarTable[m_Definition->m_VarNames[0]] = &Arg;
    }

    llvm::BasicBlock* BB =
        llvm::BasicBlock::Create(*llvmC.MilaContext, "entry", F);
    llvmC.MilaBuilder->SetInsertPoint(BB);

    for(auto i : m_Body) i->codegen();
    llvmC.MilaBuilder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmC.MilaContext), 0));
    return nullptr;
}

Value* functionBodyNode::getValue(const std::string& name){
    return m_VarTable[name];
}
// TODO IMPLEMENT PROPER CODE BLOCKS ETC
llvm::Value* ifElseNode::codegen() {
    auto cond = m_Condition->codegen();
    cond = llvmC.MilaBuilder->CreateICmpNE(
        llvmC.MilaBuilder->CreateZExt(cond, llvmC.MilaBuilder->getInt32Ty()), ConstantInt::get(*llvmC.MilaContext, llvm::APInt(32, 0)));

    Function* TheFunction = llvmC.MilaBuilder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at
    // the end of the function.
    BasicBlock* ThenBB =
        BasicBlock::Create(*llvmC.MilaContext, "", TheFunction);
    BasicBlock* ElseBB = BasicBlock::Create(*llvmC.MilaContext);
    BasicBlock* MergeBB = BasicBlock::Create(*llvmC.MilaContext);
    
    llvmC.MilaBuilder->CreateCondBr(cond, ThenBB, ElseBB);

    llvmC.MilaBuilder->SetInsertPoint(ThenBB);
    
    auto thenVal = m_If->codegen();

    llvmC.MilaBuilder->CreateBr(MergeBB);

    TheFunction->insert(TheFunction->end(), ElseBB);
    llvmC.MilaBuilder->SetInsertPoint(ElseBB);
    auto elseVal = m_Else->codegen();
    llvmC.MilaBuilder->CreateBr(MergeBB);
    llvmC.MilaBuilder->SetInsertPoint(ThenBB);

    TheFunction->insert(TheFunction->end(), MergeBB);
    llvmC.MilaBuilder->SetInsertPoint(MergeBB);
    return nullptr;
}
llvm::Value* binOperatorNode::codegen(){
    auto leftVal = m_Lhs->codegen();
    auto rightVal = m_Rhs->codegen();
    switch(m_OpType){
        case binOps::ADD:
            return llvmC.MilaBuilder->CreateAdd(leftVal,rightVal);
        case binOps::SUBTRACT:
            return llvmC.MilaBuilder->CreateSub(leftVal,rightVal);
        case binOps::LESSTHAN:
            return llvmC.MilaBuilder->CreateICmpSLT(leftVal,rightVal);
    }
    throw std::invalid_argument("unsupported operation");
    return nullptr;
}

llvm::Value* returnNode::codegen(){
    auto retVal = m_RetVal->codegen();
    return llvmC.MilaBuilder->CreateRet(retVal);
} 

llvm::Value* numberNode::codegen(){
    return ConstantInt::get(*llvmC.MilaContext, llvm::APInt(32, m_Val));
}
llvm::Value* identifierNode::codegen(){
    if(!getParentFunc()) throw std::invalid_argument("no parent");
    return getParentFunc()->getValue(m_Identifier);
}


llvm::Value* functionCallNode::codegen(){
    std::vector<Value*> args;
    for(auto i : m_Arguments) args.push_back(i->codegen());

    return llvmC.MilaBuilder->CreateCall(
            llvmC.MilaModule->getFunction(m_FunctionName),
            args);
}
