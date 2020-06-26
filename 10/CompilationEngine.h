#ifndef COMPILATIONENGINE_H_INCLUDED
#define COMPILATIONENGINE_H_INCLUDED

#include <fstream>
#include <vector>
#include <string>

struct Token;
class JackTokenizer;

class CompilationEngine
{
public:
    CompilationEngine(){}
    CompilationEngine(const std::string&, const char*);
    ~CompilationEngine(){output_xml.close();}

    std::string getTokenType() {return token.type;}
    std::string getTokenTypeXml() {return token.type_xml;}
    std::string getTokenValue() {return token.value;}

    void importToken(const Token&);
    void write(const int&);
    void eatPreset(const int&, const std::string&, const std::string&,
                   const std::string&, const std::string&, const std::string&);
    void eat(const int&);
    void pushForwardToken(JackTokenizer&);
    void pushTokenBack(JackTokenizer&);

    void compileType(const int&);
    void compileGeneralVarDec(const int&, JackTokenizer&);
    void compileSubroutineCall(const int&, JackTokenizer&);

    void compileClass(JackTokenizer&);
    void compileClassVarDec(const int&, JackTokenizer&);
    void compileSubroutineDec(const int&, JackTokenizer&);
    void compileParameterList(const int&, JackTokenizer&);
    void compileVarDec(const int&, JackTokenizer&);
    void compileSubroutineBody(const int&, JackTokenizer&);
    void compileStatements(const int&, JackTokenizer&);

    void compileLetStatement(const int&, JackTokenizer&);
    void compileIfStatement(const int&, JackTokenizer&);
    void compileWhileStatement(const int&, JackTokenizer&);
    void compileDoStatement(const int&, JackTokenizer&);
    void compileReturnStatement(const int&, JackTokenizer&);

    void compileExpression(const int&, JackTokenizer&);
    void compileTerm(const int&, JackTokenizer&);
    void compileExpressionList(const int&, JackTokenizer&);

    std::string testIfOp(bool&, const std::string&);
    std::string testIfUnaryOp(bool&, const std::string&);
    std::string testIfKeywordConstant(bool&, const string&);

private:
    std::ofstream output_xml;
    Token token;
};

#include "CompilationEngine.cpp"

#endif // COMPILATIONENGINE_H_INCLUDED
