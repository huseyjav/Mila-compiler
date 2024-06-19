#include <llvm-16/llvm/ADT/APInt.h>
#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/DerivedTypes.h>

#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AST.h"
#include "Lexer.hpp"
#include "Parser.hpp"

llvmClasses llvmC;
Parser::Parser(std::istream& os)
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
    bool isProcedure = false;
    if (CurTok == Token::tok_procedure) isProcedure = true;
    getNextToken();
    std::string funcName = m_Lexer.identifierStr();
    std::vector<std::string> args;
    consume(Token::tok_identifier);

    consume('(');

    while (CurTok != ')') {
        args.push_back(m_Lexer.identifierStr());
        consume(Token::tok_identifier);
        consume(':');
        consume(Token::tok_integer);
        if (CurTok != ')') consume(';');
    }
    consume(')');
    if (!isProcedure) {
        consume(':');
        consume(Token::tok_integer);
    }
    consume(';');
    curFunc = funcName;
    return std::make_shared<functionDefNode>(nullptr, funcName, args,
                                             isProcedure);
}
// FUNCTION := funcDef CODEBLOCK
std::shared_ptr<ASTNode> Parser::parseFunction() {
    auto funcDef = parseFunctionDefinition();
    auto vars = parseVariables();
    if (CurTok == Token::tok_forward) {
        consume(Token::tok_forward);
        return funcDef;
    }
    std::shared_ptr<ASTNode> funcBody = parseCodeBlock();
    return std::make_shared<functionBodyNode>(nullptr, funcDef, funcBody, vars);
}
// MAINFUNCTION := VARS CODEBLOCK
std::shared_ptr<functionBodyNode> Parser::parseMainFunction() {
    auto funcDef = std::make_shared<functionDefNode>(
        nullptr, "main", std::vector<std::string>());
    auto vars = parseVariables();
    std::shared_ptr<ASTNode> funcBody = parseCodeBlock();
    return std::make_shared<functionBodyNode>(nullptr, funcDef, funcBody, vars);
}
// primary := IDENTIFIER | num_literal
std::shared_ptr<ASTNode> Parser::parsePrimary() {
    std::shared_ptr<ASTNode> LHS;
    switch (CurTok) {
        case Token::tok_identifier:
            return std::make_shared<binOperatorNode>(nullptr, parseIdentifier(),
                                                     nullptr, binOps::VARIABLE);
        case '-':
        case Token::tok_number:
            return parseNumber();
        case '(': {
            consume('(');
            LHS = parseBinOp();
            consume(')');
            return LHS;
        }
    }
    return nullptr;
}
// IDENTIFIER := identifier | funccall
std::shared_ptr<ASTNode> Parser::parseIdentifier() {
    std::string identifier = m_Lexer.identifierStr();
    consume(Token::tok_identifier);
    if (CurTok != '(')
        return std::make_shared<identifierNode>(nullptr, identifier);
    consume('(');
    std::vector<std::shared_ptr<ASTNode>> argList;
    while (CurTok != ')') {
        auto arg = parseBinOp();
        argList.push_back(arg);
        if (CurTok != ')') consume(',');
    }
    consume(')');
    return std::make_shared<functionCallNode>(nullptr, argList, identifier);
}

std::shared_ptr<ASTNode> Parser::parseNumber() {
    bool negative = false;
    if (CurTok == '-') {
        consume('-');
        negative = true;
    }

    std::shared_ptr<ASTNode> retval =
        std::make_shared<numberNode>(nullptr, m_Lexer.numVal());

    std::shared_ptr<ASTNode> retvalNegative = std::make_shared<binOperatorNode>(
        nullptr, std::make_shared<numberNode>(nullptr, 0), retval,
        binOps::SUBTRACT);

    consume(Token::tok_number);

    return (negative ? retvalNegative : retval);
}

std::shared_ptr<ASTNode> Parser::parseCodeBlock() {
    std::vector<std::shared_ptr<ASTNode>> returnVector;
    consume(Token::tok_begin);
    while (CurTok != Token::tok_end) {
        returnVector.push_back(parseExpression());
        if (CurTok == ';')
            getNextToken();
        else
            break;
    }
    consume(Token::tok_end);
    return std::make_shared<codeBlockNode>(nullptr, returnVector);
}

std::set<std::string> Parser::parseIdentifiers() {
    std::set<std::string> toReturn;
    while (CurTok == Token::tok_identifier) {
        toReturn.insert(m_Lexer.identifierStr());
        consume(Token::tok_identifier);
        if (CurTok == ',')
            consume(',');
        else
            break;
    }
    return toReturn;
}
declaredVars Parser::parseVariables() {
    declaredVars toReturn;
    std::set<std::string> varNames;
    while (1) {
        switch (CurTok) {
            case Token::tok_var: {
                consume(Token::tok_var);
                while (CurTok == Token::tok_identifier) {
                    varNames = parseIdentifiers();
                    toReturn.intMutables.insert(varNames.begin(),
                                                varNames.end());
                    consume(':');
                    consume(Token::tok_integer);
                    consume(';');
                }
                break;
            }
            case Token::tok_const: {
                consume(Token::tok_const);
                while (CurTok == Token::tok_identifier) {
                    auto constName = m_Lexer.identifierStr();
                    consume(Token::tok_identifier);
                    consume('=');
                    auto constVal = m_Lexer.numVal();
                    consume(Token::tok_number);
                    toReturn.intConsts[constName] = constVal;
                    consume(';');
                }
                break;
            }
            default:
                goto breakwhile;
        }
    }
breakwhile:
    return toReturn;
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl0() { return parsePrimary(); }
std::shared_ptr<ASTNode> Parser::parseBinOpLvl1() {
    return parseBinOpLvl0();
    // under construction, TODO a
    auto LHS = parseBinOpLvl0();
    while (1) {
        auto backup = CurTok;
        switch (backup) {
            case '(':
            case '[': {
                consume(backup);
                auto RHS = parseBinOpLvl1();
                LHS = std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                        (binOps)backup);
                break;
            }
            default:
                return LHS;
        }
    }
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl2() {
    auto LHS = parseBinOpLvl1();
    while (1) {
        auto backup = CurTok;
        switch (backup) {
            case '*':
            case '/':
            case Token::tok_mod: {
                consume(backup);
                auto RHS = parseBinOpLvl1();
                LHS = std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                        (binOps)backup);
                break;
            }
            default:
                return LHS;
        }
    }
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl3() {
    auto LHS = parseBinOpLvl2();
    while (1) {
        auto backup = CurTok;
        switch (backup) {
            case '+':
            case '-': {
                consume(backup);
                auto RHS = parseBinOpLvl2();
                LHS = std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                        (binOps)backup);
                break;
            }
            default:
                return LHS;
        }
    }
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl4() {
    auto LHS = parseBinOpLvl3();
    while (1) {
        auto backup = CurTok;
        switch (backup) {
            case '<':
            case '>':
            case Token::tok_greaterequal:
            case Token::tok_lessequal: {
                consume(backup);
                auto RHS = parseBinOpLvl3();
                LHS = std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                        (binOps)backup);
                break;
            }
            default:
                return LHS;
        }
    }
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl5() {
    auto LHS = parseBinOpLvl4();
    while (1) {
        auto backup = CurTok;
        switch (backup) {
            case '=':
            case Token::tok_notequal:
            case Token::tok_or: {
                consume(backup);
                auto RHS = parseBinOpLvl4();
                LHS = std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                        (binOps)backup);
                break;
            }
            default:
                return LHS;
        }
    }
}
std::shared_ptr<ASTNode> Parser::parseBinOpLvl6() {
    auto LHS = parseBinOpLvl5();
    auto backup = CurTok;
    switch (backup) {
        case Token::tok_assign:
            consume(Token::tok_assign);
            auto RHS = parseBinOpLvl6();
            return std::make_shared<binOperatorNode>(nullptr, LHS, RHS,
                                                     (binOps)backup);
    }
    return LHS;
}
// BinOPRHS := LHS OPERATOR BINOpRHS |
//             LHS
std::shared_ptr<ASTNode> Parser::parseBinOp() { return parseBinOpLvl6(); }

// EXPR := IfElse | PRIM
std::shared_ptr<ASTNode> Parser::parseExpression() {
    switch (CurTok) {
        case Token::tok_if:
            return parseIfElse();
        case Token::tok_begin:
            return parseCodeBlock();
        case Token::tok_exit:
            return parseExit();
        case Token::tok_while:
            return parseWhile();
        case Token::tok_for:
            return parseFor();
        default:
            return parseBinOp();
    }
}

std::shared_ptr<ASTNode> Parser::parseIfElse() {
    consume(Token::tok_if);
    auto condition = parseBinOp();
    consume(Token::tok_then);
    auto ifBranch = parseExpression();
    // if next is ; then there is no else block
    if (CurTok == ';')
        return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                            nullptr);
    if (CurTok != Token::tok_else)
        return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                            nullptr);
    getNextToken();
    auto elseBranch = parseExpression();
    // consume(';');
    return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                        elseBranch);
}

std::shared_ptr<ASTNode> Parser::parseExit() {
    consume(Token::tok_exit);
    return std::make_shared<exitNode>();
}

std::shared_ptr<ASTNode> Parser::parseWhile() {
    consume(Token::tok_while);
    auto condition = parseBinOpLvl6();
    consume(Token::tok_do);
    auto body = parseExpression();
    return std::make_shared<whileNode>(nullptr, condition, body);
}

std::shared_ptr<ASTNode> Parser::parseFor(){
    bool downto=false;
    consume(Token::tok_for);

    std::shared_ptr<binOperatorNode> variable = 
        std::dynamic_pointer_cast<binOperatorNode>(parsePrimary());

    consume(Token::tok_assign);
    std::shared_ptr<ASTNode> init = parseBinOp();

    if(CurTok==Token::tok_downto){
        consume(Token::tok_downto);
        downto = true;
    }
    else consume(Token::tok_to);

    std::shared_ptr<ASTNode> final = parseBinOp();

    consume(Token::tok_do);

    std::shared_ptr<ASTNode> body = parseExpression();
    return std::make_shared<forNode>(nullptr,variable, init, final, body, downto);
}
// parse the program (only functions)
// S := FUNCTION | MAINFUNCTION
bool Parser::Parse() {
    getNextToken();
    consume(Token::tok_program);
    consume(Token::tok_identifier);
    consume(';');

    std::vector<std::shared_ptr<ASTNode>> funcs;
    while (CurTok != Token::tok_eof) {
        switch (CurTok) {
            // parse functions
            case Token::tok_function:
            case Token::tok_procedure:
                funcs.push_back(parseFunction());
                getNextToken();
                break;
            // parse main function
            // case Token::tok_var:
            default:
                funcs.push_back(parseMainFunction());
                getNextToken();
                break;
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
    {
        std::vector<llvm::Type*> Ints(1, llvm::Type::getInt32Ty(MilaContext));
        llvm::FunctionType* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(MilaContext), Ints, false);
        llvm::Function* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "write", MilaModule);
        for (auto& Arg : F->args()) Arg.setName("x");
    }
    {
        std::vector<llvm::Type*> Ptr(
            1, llvm::PointerType::get(llvm::Type::getInt32Ty(MilaContext), 0));
        llvm::FunctionType* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(MilaContext), Ptr, false);
        llvm::Function* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "readln", MilaModule);
        for (auto& Arg : F->args()) Arg.setName("x");
    }
    for (auto i : m_Program->m_Funcs) i->codegen();
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
void Parser::consume(int consumeTok) {
    std::stringstream expectedSS;
    expectedSS << consumeTok;
    std::stringstream gotSS;
    gotSS << CurTok;
    if (CurTok != consumeTok)
        throw std::invalid_argument("expected: " + expectedSS.str() +
                                    " got: " + gotSS.str());
    getNextToken();
}
