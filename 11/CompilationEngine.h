#ifndef COMPILATIONENGINE_H_INCLUDED
#define COMPILATIONENGINE_H_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "SymbolTable.h"
#include "VmWriter.h"

struct Token;
class JackTokenizer;

class CompilationEngine
{
public:
    CompilationEngine(): global_if_counter(0), global_while_counter(0) {}
    CompilationEngine(const std::string&, const char*);
    //~CompilationEngine(){output_xml.close();}

    std::string getTokenType() {return token.type;}
    std::string getTokenTypeXml() {return token.type_xml;}
    std::string getTokenValue() {return token.value;}

    void importToken(const Token&);
    void write(const std::string&, const std::string&);
    void eatPreset(const std::string&, const std::string&,
                   const std::string&, const std::string&, const std::string&);
    void eat(const std::string&, const std::string&, const std::string&);
    void pushForwardToken(JackTokenizer&);
    void pushTokenBack(JackTokenizer&);

    void compileType();
    void compileGeneralVarDec(JackTokenizer&, const std::string&, const std::string&);
    void compileSubroutineCall(JackTokenizer&);

    void compileClass(JackTokenizer&, const std::string&, const char*); // also constructs VmWriter needs filename and argv to do so
    void compileClassVarDec(JackTokenizer&);
    void compileSubroutineDec(JackTokenizer&);
    void compileParameterList(JackTokenizer&);
    void compileVarDec(JackTokenizer&);
    void compileSubroutineBody(JackTokenizer&);
    void compileStatements(JackTokenizer&);

    void compileLetStatement(JackTokenizer&);
    void compileIfStatement(JackTokenizer&);
    void compileWhileStatement(JackTokenizer&);
    void compileDoStatement(JackTokenizer&);
    void compileReturnStatement(JackTokenizer&);

    void checkIdentifier(std::string&, std::string&);
    void printSymbolTable(const std::string&);

    void compileExpression(JackTokenizer&);
    void compileTerm(JackTokenizer&);
    void compileExpressionList(JackTokenizer&, int&);

    std::string testIfOp(bool&, const std::string&);
    std::string testIfUnaryOp(bool&, const std::string&);
    std::string testIfKeywordConstant(bool&, const std::string&);

private:
    Token token;

    SymbolTable class_symbols;
    SymbolTable subroutine_symbols;

    std::string current_class_name;
    std::string current_subroutine_name;
    std::string subroutine_type;

    int global_if_counter, global_while_counter;
    VmWriter output_vm;
};

#include "CompilationEngine.cpp"

#endif // COMPILATIONENGINE_H_INCLUDED
