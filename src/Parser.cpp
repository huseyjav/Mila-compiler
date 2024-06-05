#include <llvm-16/llvm/ADT/APInt.h>
#include <llvm-16/llvm/IR/BasicBlock.h>

#include <memory>
#include <ostream>
#include <sstream>
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
    bool isProcedure=false;
    if(CurTok==Token::tok_procedure) isProcedure=true;
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
        if(CurTok != ')') consume(';');
    }
    consume(')');
    if(!isProcedure){
        consume(':');
        consume(Token::tok_integer);
    }
    consume(';');
    curFunc = funcName;
    return std::make_shared<functionDefNode>(nullptr, funcName, args, isProcedure);
}
// FUNCTION := funcDef CODEBLOCK
std::shared_ptr<ASTNode> Parser::parseFunction() {
    auto funcDef = parseFunctionDefinition();
    if(CurTok==Token::tok_forward){
        consume(Token::tok_forward);
        //consume(';');
        return funcDef;
    }
    std::shared_ptr<ASTNode> funcBody = parseCodeBlock();
    return std::make_shared<functionBodyNode>(nullptr, funcDef, funcBody);
}
// MAINFUNCTION := VARS CODEBLOCK
std::shared_ptr<functionBodyNode> Parser::parseMainFunction() {
    auto funcDef = std::make_shared<functionDefNode>(
        nullptr, "main", std::vector<std::string>());
    std::shared_ptr<ASTNode> funcBody = parseCodeBlock();
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
    std::string identifier = m_Lexer.identifierStr();
    consume(Token::tok_identifier);
    if (CurTok != '(')
        return std::make_shared<identifierNode>(nullptr, identifier);
    consume('(');
    std::vector<std::shared_ptr<ASTNode>> argList;
    while (CurTok != ')') {
        auto arg = parseBinOp();
        argList.push_back(arg);
        if(CurTok!=')') consume(',');
    }
    consume(')');
    return std::make_shared<functionCallNode>(nullptr, argList,
            identifier);
}

std::shared_ptr<ASTNode> Parser::parseNumber() {
    auto retval = std::make_shared<numberNode>(nullptr, m_Lexer.numVal());
    consume(Token::tok_number);
    return retval;
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
    return std::make_shared<codeBlockNode>(nullptr,returnVector);
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
        case '>':
        case Token::tok_assign:
            getNextToken();
            return std::make_shared<binOperatorNode>(nullptr, LHS, parseBinOp(),
                                                     (binOps)backup);
        default:
            return LHS;
    }
}

// EXPR := IfElse | PRIM
std::shared_ptr<ASTNode> Parser::parseExpression() {
    switch (CurTok) {
        case Token::tok_if:
            return parseIfElse();
        case Token::tok_begin:
            return parseCodeBlock();
        case Token::tok_exit:
            return parseExit();
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
    if(CurTok==';') 
        return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                            nullptr);
    if (CurTok != Token::tok_else)
        return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                            nullptr);
    getNextToken();
    auto elseBranch = parseExpression();
    //consume(';');
    return std::make_shared<ifElseNode>(nullptr, condition, ifBranch,
                                        elseBranch);
}

std::shared_ptr<ASTNode> Parser::parseExit(){
    consume(Token::tok_exit);
    return std::make_shared<exitNode>();
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
    for(auto i : m_Program->m_Funcs) i->codegen();
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
