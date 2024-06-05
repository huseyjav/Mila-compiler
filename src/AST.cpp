#include <AST.h>
#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/Value.h>

#include <cstdio>
#include <iostream>
#include <memory>
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
                                 const std::vector<std::string>& varNames,
                                 bool isProcedure)
    : ASTNode(parent),
      m_FuncName(funcName),
      m_VarNames(varNames),
      m_isProcedure((isProcedure)) {}

llvm::Value* functionDefNode::codegen() {
    if (llvmC.MilaModule->getFunction(m_FuncName)) return nullptr;
    std::vector<llvm::Type*> Ints(m_VarNames.size(),
                                  llvm::Type::getInt32Ty(*(llvmC.MilaContext)));
    llvm::FunctionType* FT = llvm::FunctionType::get(
        (m_isProcedure ? (llvm::Type::getVoidTy(*(llvmC.MilaContext)))
                       : (llvm::Type::getInt32Ty(*(llvmC.MilaContext)))),
        Ints, false);
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, m_FuncName,
                           *llvmC.MilaModule);
    return nullptr;
}
functionBodyNode::functionBodyNode(ASTNode* parent,
                                   std::shared_ptr<functionDefNode> definition,
                                   std::shared_ptr<ASTNode> body)
    : ASTNode(parent), m_Definition(definition), m_Body(body) {
    m_Definition->setParent(this);
    m_Body->setParent(this);
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
    if(m_Else) m_Else->setParent(this);
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
    : ASTNode(parent), m_RetVal(retval) {
    m_RetVal->setParent(this);
}

llvm::Value* functionBodyNode::codegen() {
    m_ExitBlock = llvm::BasicBlock::Create(*llvmC.MilaContext);
    m_Definition->codegen();
    auto F = llvmC.MilaModule->getFunction(m_Definition->m_FuncName);
    int i = 0;
    for (auto& Arg : F->args()) {
        m_VarTable[m_Definition->m_VarNames[i]] = &Arg;
        i++;
    }

    llvm::BasicBlock* BB =
        llvm::BasicBlock::Create(*llvmC.MilaContext, "entry", F);
    llvmC.MilaBuilder->SetInsertPoint(BB);
    if (!m_Definition->m_isProcedure)
        m_DynamicVars[m_Definition->m_FuncName] =
            llvmC.MilaBuilder->CreateAlloca(
                Type::getInt32Ty(*llvmC.MilaContext));
    m_Body->codegen();
    if (!llvmC.MilaBuilder->GetInsertBlock()->getTerminator())
        llvmC.MilaBuilder->CreateBr(m_ExitBlock);
    F->insert(F->end(), m_ExitBlock);
    llvmC.MilaBuilder->SetInsertPoint(m_ExitBlock);
    llvmC.MilaBuilder->CreateRet(getValue(m_Definition->m_FuncName));
    return nullptr;
}

Value* functionBodyNode::getValue(const std::string& name) {
    if (m_VarTable.count(name)) return m_VarTable[name];
    if (!m_DynamicVars.count(name)) return nullptr;
    return llvmC.MilaBuilder->CreateLoad(
        m_DynamicVars[name]->getAllocatedType(), m_DynamicVars[name]);
}
AllocaInst* functionBodyNode::getDynamicVar(const std::string& name) {
    if (!m_DynamicVars.count(name))
        throw std::invalid_argument("dynamic variable doesnt exist");
    return m_DynamicVars[name];
}
llvm::BasicBlock* functionBodyNode::getExitBlock() { return m_ExitBlock; }

llvm::Value* ifElseNode::codegen() {
    auto cond = m_Condition->codegen();

    cond = llvmC.MilaBuilder->CreateICmpNE(
        llvmC.MilaBuilder->CreateZExt(cond, llvmC.MilaBuilder->getInt32Ty()),
        ConstantInt::get(*llvmC.MilaContext, llvm::APInt(32, 0)));

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

    if (!llvmC.MilaBuilder->GetInsertBlock()->getTerminator())
        llvmC.MilaBuilder->CreateBr(MergeBB);

    TheFunction->insert(TheFunction->end(), ElseBB);
    llvmC.MilaBuilder->SetInsertPoint(ElseBB);
    if(m_Else) auto elseVal = m_Else->codegen();
    if (!llvmC.MilaBuilder->GetInsertBlock()->getTerminator())
        llvmC.MilaBuilder->CreateBr(MergeBB);

    TheFunction->insert(TheFunction->end(), MergeBB);
    llvmC.MilaBuilder->SetInsertPoint(MergeBB);
    return nullptr;
}
llvm::Value* binOperatorNode::codegen() {
    Value* leftVal = nullptr;
    if (m_OpType != binOps::ASSIGN) leftVal = m_Lhs->codegen();
    auto rightVal = m_Rhs->codegen();
    switch (m_OpType) {
        case binOps::ADD:
            return llvmC.MilaBuilder->CreateAdd(leftVal, rightVal);
        case binOps::SUBTRACT:
            return llvmC.MilaBuilder->CreateSub(leftVal, rightVal);
        case binOps::LESSTHAN:
            return llvmC.MilaBuilder->CreateICmpSLT(leftVal, rightVal);
        case binOps::MORETHAN:
            return llvmC.MilaBuilder->CreateICmpUGT(leftVal, rightVal);
        case binOps::ASSIGN: {
            std::shared_ptr<identifierNode> LhsIdentifier =
                std::dynamic_pointer_cast<identifierNode>(m_Lhs);
            if (!LhsIdentifier)
                throw std::invalid_argument(
                    "assignment operator LHS not identifier");
            return llvmC.MilaBuilder->CreateStore(
                rightVal,
                getParentFunc()->getDynamicVar(LhsIdentifier->getId()));
        }
    }
    throw std::invalid_argument("unsupported operation");
    return nullptr;
}

llvm::Value* returnNode::codegen() {
    auto retVal = m_RetVal->codegen();
    return llvmC.MilaBuilder->CreateRet(retVal);
}

llvm::Value* numberNode::codegen() {
    return ConstantInt::get(*llvmC.MilaContext, llvm::APInt(32, m_Val));
}
llvm::Value* identifierNode::codegen() {
    if (!getParentFunc()) throw std::invalid_argument("no parent");
    return getParentFunc()->getValue(m_Identifier);
}

llvm::Value* functionCallNode::codegen() {
    std::vector<Value*> args;
    for (auto i : m_Arguments) args.push_back(i->codegen());

    return llvmC.MilaBuilder->CreateCall(
        llvmC.MilaModule->getFunction(m_FunctionName), args);
}
codeBlockNode::codeBlockNode(ASTNode* parent,
                             const std::vector<std::shared_ptr<ASTNode>>& block)
    : ASTNode(parent), m_Block(block) {
    for (auto i : m_Block) i->setParent(this);
}
llvm::Value* codeBlockNode::codegen() {
    for (auto i : m_Block) i->codegen();
    return nullptr;
}
llvm::Value* exitNode::codegen() {
    return llvmC.MilaBuilder->CreateBr(getParentFunc()->getExitBlock());
}
