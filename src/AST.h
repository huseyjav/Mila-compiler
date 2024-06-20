#ifndef ASTH
#define ASTH

#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/GlobalVariable.h>
#include <llvm-16/llvm/IR/Instructions.h>
#include <llvm-16/llvm/IR/Type.h>
#include <llvm-16/llvm/IR/Value.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
// #include "Parser.hpp"
// #include "Parser.hpp"
using namespace llvm;

enum binOps {
    ADD = '+',
    SUBTRACT = '-',
    ASSIGN = -23,
    MULTIPLY = '*',
    DIVIDE = -26,
    MODULO = -25,
    LESSTHAN = '<',
    MORETHAN = '>',
    EQUAL = '=',
    VARIABLE = -100,
    ARRAYIDX = -101,
    NOTEQUAL = -20,
    LESSEQUAL = -21,
    GREATEQUAL = -22,
    OR = -24,
    AND = -28,
};

class functionBodyNode;
class program;
struct llvmClasses;
extern llvmClasses llvmC;
struct arrayDeclaration;
struct declaredVars {
    std::set<std::string> intMutables;
    std::vector<arrayDeclaration> intArrays;
    std::map<std::string, int> intConsts;
};
class ASTNode {
    ASTNode* m_Parent = nullptr;

   public:
    ASTNode() = default;
    ASTNode(ASTNode* parent);
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen();
    virtual functionBodyNode* getParentFunc();
    virtual program* getParentProg();
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
    functionCallNode(ASTNode* parent,
                     const std::vector<std::shared_ptr<ASTNode>>&,
                     const std::string&);
    llvm::Value* codegen() override;
};

class binOperatorNode : public ASTNode {
    std::shared_ptr<ASTNode> m_Lhs, m_Rhs;
    binOps m_OpType;

   public:
    binOperatorNode(ASTNode* parent, std::shared_ptr<ASTNode> lhs,
                    std::shared_ptr<ASTNode> rhs, binOps opType);
    // if opType is variable or array index codegen will load it
    llvm::Value* codegen() override;
    // will be called when evaluating an assignment operator
    // will return pointer for createstore
    llvm::Value* getPtr();
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
    bool m_isProcedure = false;
    functionDefNode(ASTNode* parent, const std::string& funcName,
                    const std::vector<std::string>& varNames,
                    bool isProcedure = false);
    llvm::Value* codegen() override;
};

class functionBodyNode : public ASTNode {
    std::shared_ptr<functionDefNode> m_Definition;
    std::shared_ptr<ASTNode> m_Body;
    //  function arguments
    std::map<std::string, Value*> m_VarTable;
    // dynamic variables
    declaredVars m_DecVar;
    std::map<std::string, AllocaInst*> m_DynamicVars;
    std::map<std::string, Value*> m_ConstVars;

    // array declarations 
    std::map<std::string, arrayDeclaration> m_ArrayDecTable;
    llvm::BasicBlock* m_ExitBlock = nullptr;

   public:
    functionBodyNode(ASTNode* parent, std::shared_ptr<functionDefNode>,
                     std::shared_ptr<ASTNode> body, declaredVars& decVar);

    functionBodyNode* getParentFunc() override;
    llvm::Value* codegen() override;
    Value* getValue(const std::string& name);
    AllocaInst* getDynamicVar(const std::string& name);
    arrayDeclaration getArrayDec(const std::string& name);
    llvm::BasicBlock* getExitBlock();
};

class program : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> m_Funcs;
    declaredVars globalVars;
    std::map<std::string, GlobalVariable*> m_VarTable;
   public:
    program(const std::vector<std::shared_ptr<ASTNode>>& funcs);
    void addGlobalVars(const declaredVars& toAdd);
    llvm::Value* codegen() override;
    program* getParentProg() override;
};

class returnNode : public ASTNode {
    std::shared_ptr<ASTNode> m_RetVal;

   public:
    returnNode(ASTNode*, std::shared_ptr<ASTNode>);
    llvm::Value* codegen() override;
};

class codeBlockNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> m_Block;

   public:
    codeBlockNode(ASTNode* parent,
                  const std::vector<std::shared_ptr<ASTNode>>& block);
    llvm::Value* codegen() override;
};

class exitNode : public ASTNode {
   public:
    llvm::Value* codegen() override;
};

class whileNode : public ASTNode {
    std::shared_ptr<ASTNode> m_Condition, m_Body;

   public:
    whileNode(ASTNode* parent, std::shared_ptr<ASTNode> condition,
              std::shared_ptr<ASTNode> body);
    llvm::Value* codegen() override;
};

class forNode : public ASTNode {
    std::shared_ptr<binOperatorNode> m_Variable;
    std::shared_ptr<ASTNode> m_InitValue, m_FinalValue, m_Body;
    bool m_Downto;

   public:
    forNode(ASTNode* parent, std::shared_ptr<binOperatorNode> variable,
            std::shared_ptr<ASTNode> initValue,
            std::shared_ptr<ASTNode> finalValue, std::shared_ptr<ASTNode> body,
            bool downto);
    llvm::Value* codegen() override;
};

class globalVarDeclaration : public ASTNode {
    declaredVars m_Vars;

   public:
    globalVarDeclaration(ASTNode* parent, const declaredVars& vars);
    llvm::Value* codegen() override;
};

#endif
