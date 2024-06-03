#ifndef ASTH
#define ASTH

#include <llvm-16/llvm/IR/Type.h>
#include <llvm-16/llvm/IR/Value.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
//#include "Parser.hpp"
using namespace llvm;

enum binOps {
    ADD = '+',
    SUBTRACT = '-',
    ASSIGN = -23,
    MULTIPLY = '*',
    DIVIDE = -26,
    MODULO = -25,
    LESSTHAN = '<' 
};


class functionBodyNode;
struct llvmClasses;
extern llvmClasses llvmC;

class ASTNode {
    ASTNode* m_Parent = nullptr;

   public:
    ASTNode() = default;
    ASTNode(ASTNode* parent);
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen();
    virtual functionBodyNode* getParentFunc();
    void setParent(ASTNode* parent);
};

class numberNode : public ASTNode {
    int m_Val;

   public:
    numberNode(ASTNode* parent, int val);
    llvm::Value* codegen() override;
};

class identifierNode : public ASTNode {
    std::string m_Identifier;

   public:
    identifierNode(ASTNode* parent, const std::string& identifier);
    std::string getId();
    llvm::Value* codegen() override;
};

class functionCallNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> m_Arguments;
    std::string m_FunctionName;

   public:
    functionCallNode(ASTNode* parent, const std::vector<std::shared_ptr<ASTNode>>&,
                     const std::string&);
    llvm::Value* codegen() override;
};

class binOperatorNode : public ASTNode {
    std::shared_ptr<ASTNode> m_Lhs, m_Rhs;
    binOps m_OpType;

   public:
    binOperatorNode(ASTNode* parent, std::shared_ptr<ASTNode> lhs,
                    std::shared_ptr<ASTNode> rhs, binOps opType);
    llvm::Value* codegen() override;
};

class ifElseNode : public ASTNode {
    std::shared_ptr<ASTNode> m_Condition;
    std::shared_ptr<ASTNode> m_If, m_Else;

   public:
    ifElseNode(ASTNode*, std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>,
               std::shared_ptr<ASTNode>);
    llvm::Value* codegen() override;
};

class functionDefNode : public ASTNode {
   public:
    std::string m_FuncName;
    std::vector<std::string> m_VarNames;

    functionDefNode(ASTNode* parent, const std::string& funcName,
                    const std::vector<std::string>& varNames);
    //llvm::Value* codengen() override;
};

class functionBodyNode : public ASTNode {
    std::shared_ptr<functionDefNode> m_Definition;
    std::vector<std::shared_ptr<ASTNode>> m_Body;
    std::map<std::string, Value*> m_VarTable;

   public:
    functionBodyNode(ASTNode* parent, std::shared_ptr<functionDefNode>,
            const std::vector<std::shared_ptr<ASTNode>>& body);

    functionBodyNode* getParentFunc() override;
    llvm::Value* codegen() override;
    Value* getValue(const std::string& name);
};

class program : public ASTNode {
   public:
    std::vector<std::shared_ptr<functionBodyNode>> m_Funcs;
    //llvm::Value* codengen() override;
};

class returnNode : public ASTNode{
    std::shared_ptr<ASTNode> m_RetVal;
    public:
    returnNode(ASTNode*, std::shared_ptr<ASTNode>);
    llvm::Value* codegen() override;
};

#endif
