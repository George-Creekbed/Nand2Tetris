#ifndef SYNTAXANALYSER_H_INCLUDED
#define SYNTAXANALYSER_H_INCLUDED

#include <fstream>
#include <vector>
#include <string>

#include "SymbolTable.h"

struct Token;
class JackTokenizer;
struct Table;
class SymbolTable;

class SyntaxAnalyser
{
public:
    SyntaxAnalyser(){}
    SyntaxAnalyser(const std::string&, const char*);
    ~SyntaxAnalyser(){output_xml.close();}

    std::string getTokenType() {return token.type;}
    std::string getTokenTypeXml() {return token.type_xml;}
    std::string getTokenValue() {return token.value;}

    void importToken(const Token&);
    void write(const int&, const std::string&, const std::string&);
    void eatPreset(const int&, const std::string&, const std::string&,
                   const std::string&, const std::string&, const std::string&);
    void eat(const int&, const std::string&, const std::string&, const std::string&);
    void pushForwardToken(JackTokenizer&);
    void pushTokenBack(JackTokenizer&);

    void analyseType(const int&);
    void analyseGeneralVarDec(const int&, JackTokenizer&, const std::string&, const std::string&);
    void analyseSubroutineCall(const int&, JackTokenizer&);

    void analyseClass(JackTokenizer&);
    void analyseClassVarDec(const int&, JackTokenizer&);
    void analyseSubroutineDec(const int&, JackTokenizer&);
    void analyseParameterList(const int&, JackTokenizer&);
    void analyseVarDec(const int&, JackTokenizer&);
    void analyseSubroutineBody(const int&, JackTokenizer&);
    void analyseStatements(const int&, JackTokenizer&);

    void analyseLetStatement(const int&, JackTokenizer&);
    void analyseIfStatement(const int&, JackTokenizer&);
    void analyseWhileStatement(const int&, JackTokenizer&);
    void analyseDoStatement(const int&, JackTokenizer&);
    void analyseReturnStatement(const int&, JackTokenizer&);

    void checkIdentifier(std::string&, std::string&);
    void printSymbolTable(const std::string&);

    void analyseExpression(const int&, JackTokenizer&);
    void analyseTerm(const int&, JackTokenizer&);
    void analyseExpressionList(const int&, JackTokenizer&);

    std::string testIfOp(bool&, const std::string&);
    std::string testIfUnaryOp(bool&, const std::string&);
    std::string testIfKeywordConstant(bool&, const std::string&);

private:
    std::ofstream output_xml;
    Token token;

    SymbolTable class_symbols;
    SymbolTable subroutine_symbols;
};

#include "SyntaxAnalyser.cpp"

#endif // SYNTAXANALYSER_H_INCLUDED
