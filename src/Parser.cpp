#include <llvm-16/llvm/ADT/APInt.h>
#include <llvm-16/llvm/IR/BasicBlock.h>

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#include "AST.h"
#include "Lexer.hpp"
#include "Parser.hpp"

llvmClasses llvmC;
Parser::Parser(std::ifstream& os)
    : m_Lexer(os),
      MilaContext(),
      MilaBuilder(MilaContext),
      MilaModule("mila", MilaContext) {

    llvmC.MilaModule = &MilaModule;
    llvmC.MilaContext = &MilaContext;
    llvmC.MilaBuilder = &MilaBuilder;
}

// funcDef := function IDENTIFIER(IDENTIFIER : integer) : integer ;
std::shared_ptr<functionDefNode> Parser::parseFunctionDefinition() {
    // consume function
    getNextToken();
    if (CurTok != Token::tok_identifier)
        throw std::invalid_argument("expected function identifier");
    std::string funcName = m_Lexer.identifierStr();
    std::vector<std::string> args;
    // consume identifier
    getNextToken();

    if (CurTok != '(') throw std::invalid_argument("expected (");

    // consume (
    getNextToken();
    while (CurTok != ')') {
        if (CurTok != Token::tok_identifier)
            throw std::invalid_argument("expected identifier");
        args.push_back(m_Lexer.identifierStr());
        getNextToken();
        if (CurTok != ':') throw std::invalid_argument("expected :");
        getNextToken();
        if (CurTok != Token::tok_integer)
            throw std::invalid_argument(
                "expected integer (bold of you to assume there is anything "
                "other than integers)");
        getNextToken();
    }
    // consume )
    getNextToken();
    // consume :
    getNextToken();
    // consume integer
    getNextToken();
    // consume ;
    getNextToken();
    curFunc = funcName;
    return std::make_shared<functionDefNode>(nullptr, funcName, args);
}
// FUNCTION := funcDef CODEBLOCK
std::shared_ptr<functionBodyNode> Parser::parseFunction() {
    auto funcDef = parseFunctionDefinition();
    std::vector<std::shared_ptr<ASTNode>> funcBody = parseCodeBlock();
    return std::make_shared<functionBodyNode>(nullptr, funcDef, funcBody);
}
// MAINFUNCTION := VARS CODEBLOCK
std::shared_ptr<functionBodyNode> Parser::parseMainFunction(){
    
    auto funcDef = std::make_shared<functionDefNode>(nullptr, "main", std::vector<std::string>());
    std::vector<std::shared_ptr<ASTNode>> funcBody = parseCodeBlock();
    return std::make_shared<functionBodyNode>(nullptr, funcDef, funcBody);
}
// primary := IDENTIFIER | num_literal
std::shared_ptr<ASTNode> Parser::parsePrimary() {
    std::shared_ptr<ASTNode> LHS;
    switch (CurTok) {
        case Token::tok_identifier:
            return parseIdentifier();
        case Token::tok_number:
            return parseNumber();
    }
    return nullptr;
}
// IDENTIFIER := identifier | funccall
std::shared_ptr<ASTNode> Parser::parseIdentifier() {
    if (CurTok != Token::tok_identifier)
        throw std::invalid_argument("identifier expected");
    std::string identifier = m_Lexer.identifierStr();
    getNextToken();
    switch (CurTok) {
        case '(': {
            getNextToken();
            // TODO: IMPLEMENT MULTIPLE ARGUMENTS
            auto arg = parseBinOp();
            if (CurTok != ')')
                throw std::invalid_argument(") expected (identifier)");
            getNextToken();
            std::vector<std::shared_ptr<ASTNode>> argList;
            argList.push_back(arg);
            return std::make_shared<functionCallNode>(nullptr, argList,
                                                      identifier);
        }
        default: {
            return std::make_shared<identifierNode>(nullptr, identifier);
        }
    }
}

std::shared_ptr<ASTNode> Parser::parseNumber() {
    if (CurTok != Token::tok_number)
        throw std::invalid_argument("number token expected");
    auto retval = std::make_shared<numberNode>(nullptr, m_Lexer.numVal());
    getNextToken();
    return retval;
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseCodeBlock() {
    std::vector<std::shared_ptr<ASTNode>> returnVector;
    if (CurTok != Token::tok_begin)
        throw std::invalid_argument("expected begin");
    getNextToken();
    while (CurTok != Token::tok_end) {
        returnVector.push_back(parseExpression());
        if (CurTok == ';')
            getNextToken();
        else
            break;
    }
    if (CurTok != Token::tok_end) throw std::invalid_argument("expected end");
    getNextToken();
    return returnVector;
}

// BinOPRHS := LHS OPERATOR BINOpRHS |
//             LHS
std::shared_ptr<ASTNode> Parser::parseBinOp() {
    auto LHS = parsePrimary();
    auto backup = CurTok;
    switch (backup) {
        case '-':
        case '+':
        case '<':
            getNextToken();
            return std::make_shared<binOperatorNode>(nullptr, LHS, parseBinOp(),
                                                     (binOps)backup);
        case Token::tok_assign: {
            getNextToken();
            identifierNode* idNode = dynamic_cast<identifierNode*>(LHS.get());
            if (!idNode) throw std::invalid_argument("expected identifier");
            if (idNode->getId() == curFunc) {
                // return statement
                return std::make_shared<returnNode>(nullptr, parseBinOp());
            }
            return std::make_shared<binOperatorNode>(nullptr, LHS, parseBinOp(),
                                                     (binOps)backup);
        }
        default:
            return LHS;
    }
}

// EXPR := IfElse | PRIM
std::shared_ptr<ASTNode> Parser::parseExpression() {
    switch (CurTok) {
        case Token::tok_if:
            return parseIfElse();
        default:
            return parseBinOp();
    }
}

std::shared_ptr<ASTNode> Parser::parseIfElse() {
    if (CurTok != Token::tok_if) throw std::invalid_argument("expected if");
    getNextToken();
    auto condition = parseBinOp();
    if (CurTok != Token::tok_then) throw std::invalid_argument("expected then");
    getNextToken();
    auto ifBranch = parseExpression();
    if (CurTok != Token::tok_else)
        return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                            nullptr);
    getNextToken();
    auto elseBranch = parseExpression();
    if (CurTok != ';') throw std::invalid_argument("expected ;");
    getNextToken();
    return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                        elseBranch);
}

// parse the program (only functions)
// S := FUNCTION | MAINFUNCTION
bool Parser::Parse() {
    getNextToken();
    if (CurTok != Token::tok_program)
        throw std::invalid_argument("program keyword not prsent");
    getNextToken();
    if (CurTok != Token::tok_identifier)
        throw std::invalid_argument("program identifier not present");
    getNextToken();
    if (CurTok != ';') throw std::invalid_argument("missing the thingy");
    getNextToken();

    std::vector<std::shared_ptr<functionBodyNode>> funcs;
    while (CurTok != Token::tok_eof) {
        switch (CurTok) {
            // parse functions
            case Token::tok_function:
                funcs.push_back(parseFunction());
                getNextToken();
                break;
            // parse main function
            //case Token::tok_var:
            case Token::tok_begin:
                funcs.push_back(parseMainFunction());
                getNextToken();
                break;
            default:
                CurTok = Token::tok_eof;
        }
    }
    m_Program = std::make_shared<program>();
    m_Program->m_Funcs = funcs;
    return true;
}
const llvm::Module& Parser::Generate() {
    {
        std::vector<llvm::Type*> Ints(1, llvm::Type::getInt32Ty(MilaContext));
        llvm::FunctionType* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(MilaContext), Ints, false);
        llvm::Function* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "writeln", MilaModule);
        for (auto& Arg : F->args()) Arg.setName("x");
    }
    m_Program->m_Funcs[0]->codegen();
    m_Program->m_Funcs[1]->codegen();
    return this->MilaModule;
    // create writeln function
    {
        std::vector<llvm::Type*> Ints(1, llvm::Type::getInt32Ty(MilaContext));
        llvm::FunctionType* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(MilaContext), Ints, false);
        llvm::Function* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "writeln", MilaModule);
        for (auto& Arg : F->args()) Arg.setName("x");
    }
    {
        std::vector<llvm::Type*> Ints(1, llvm::Type::getInt32Ty(MilaContext));
        llvm::FunctionType* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(MilaContext), Ints, false);
        llvm::Function* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "sikbasi", MilaModule);
        llvm::BasicBlock* BB =
            llvm::BasicBlock::Create(MilaContext, "entry", F);
        MilaBuilder.SetInsertPoint(BB);
        auto retval = llvm::APInt(32, 20);
        auto sds = MilaBuilder.CreateCall(MilaModule.getFunction("sikbasi"));
        MilaBuilder.CreateRet(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(MilaContext), 10));
    }

    // create main function
    {
        llvm::FunctionType* FT =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(MilaContext), false);
        llvm::Function* MainFunction = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "main", MilaModule);

        // block
        llvm::BasicBlock* BB =
            llvm::BasicBlock::Create(MilaContext, "entry", MainFunction);
        MilaBuilder.SetInsertPoint(BB);

        // call writeln with value from lexel
        MilaBuilder.CreateCall(
            MilaModule.getFunction("writeln"),
            {llvm::ConstantInt::get(MilaContext, llvm::APInt(32, 42))});

        // return 0
        MilaBuilder.CreateRet(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(MilaContext), 0));
    }

    return this->MilaModule;
}

/**
 * @brief Simple token buffer.
 *
 * CurTok is the current token the parser is looking at
 * getNextToken reads another token from the lexer and updates curTok with ts
 * result Every function in the parser will assume that CurTok is the cureent
 * token that needs to be parsed
 */
int Parser::getNextToken() {
    CurTok = m_Lexer.gettok();
    return CurTok;
}
