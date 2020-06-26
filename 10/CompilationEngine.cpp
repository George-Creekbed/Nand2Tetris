#ifndef COMPILATIONENGINE_CPP_INCLUDED
#define COMPILATIONENGINE_CPP_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "CompilationEngine.h"

using std::cout;                using std::endl;
using std::vector;              using std::string;

// static variables that cannot be defined in header file, only declared
const char op[] = "+-*/&|<>=";
const size_t op_size = sizeof(op) / sizeof(*op);
const char unary_op[] = "-~";
const size_t unary_op_size = sizeof(unary_op) / sizeof(*unary_op);
const string keyword_constant[] = {"true", "false", "null", "this"};
const size_t keyword_constant_size = sizeof(keyword_constant) / sizeof(*keyword_constant);

CompilationEngine::CompilationEngine(const string& filename, const char* argv)
{
    string argument(argv);
    size_t found_jack = argument.find(".jack");

    if (found_jack == string::npos) // from directory
    {
        string::size_type slash = argument.rfind("\\");   // look for last instance of '\' in the path of directory
        string folder_name( argument.substr(slash) );     // substring created from 'slash' to the end
        output_xml.open(argument + '\\' + filename + ".xml");
    }
    else
        output_xml.open(filename + ".xml");   // from file
}


void CompilationEngine::importToken(const Token& imported_token)
{
    token = imported_token;
}


void CompilationEngine::write(const int& indent)    // 'indent' times tab space '\t'
{
    output_xml << string(indent, '\t') << "<" << getTokenTypeXml() << "> " << getTokenValue()
               << " </" << getTokenTypeXml() << ">" << endl;
}


void CompilationEngine::eatPreset(const int& indent, const string& current_token, const string& alt_token1 = "",
                                  const string& alt_token2 = "", const string& alt_token3 = "",
                                  const string& alt_token4 = "")
{
    string error(current_token + " " + alt_token1 + " " + alt_token2 + " " + alt_token3 + " " + alt_token4);
    try
    {
        if ( current_token == getTokenValue() || alt_token1 == getTokenValue() || alt_token2 == getTokenValue() ||
                alt_token3 == getTokenValue() || alt_token4 == getTokenValue() )
            write(indent);
        else
            throw error;
    }
    catch (string error)
    {
        cout << "Preset token read incorrectly: " << error << endl;
    }
}


void CompilationEngine::eat(const int& indent)
{
    try
    {
        if (getTokenType() == "IDENTIFIER" || getTokenType() == "INT_CONST" ||
            getTokenType() == "STRING_CONST" || getTokenType() == "KEYWORD")
            write(indent);
        else
            throw getTokenValue();
    }
    catch (string error)
    {
        cout << "Token read incorrectly: " << error << endl;
    }
}


void CompilationEngine::pushForwardToken(JackTokenizer& tokenizer)
{
    tokenizer.advance();
    tokenizer.tokenType();
    importToken( tokenizer.getToken() );
}


void CompilationEngine::pushTokenBack(JackTokenizer& tokenizer)
{
    tokenizer.stepBack();
    tokenizer.tokenType();
    importToken( tokenizer.getToken() );
}


void CompilationEngine::compileClass(JackTokenizer& tokenizer)
{
    output_xml << "<class>" << endl;
        pushForwardToken(tokenizer);
        // 'class'
        eatPreset(1, "class");
        pushForwardToken(tokenizer);
        // className
        eat(1);
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(1, "{");
        pushForwardToken(tokenizer);
        // classVarDec*
        while (getTokenValue() == "static" || getTokenValue() == "field")
            compileClassVarDec(2, tokenizer);
        // subroutineDec*
        while (getTokenValue() == "constructor" || getTokenValue() == "function" || getTokenValue() == "method")
        {
            compileSubroutineDec(2, tokenizer);
        }
        // '}'
        eatPreset(1, "}");
    output_xml << "</class>" << endl;
}


void CompilationEngine::compileGeneralVarDec(const int& indent, JackTokenizer& tokenizer)
{
    // type
    compileType(indent);
    pushForwardToken(tokenizer);
    // varName
    eat(indent);
    pushForwardToken(tokenizer);
    // (',' varName)*
    while (getTokenValue() != ";")
    {
        eatPreset(indent, ",");
        pushForwardToken(tokenizer);
        eat(indent);    // any further varName
        pushForwardToken(tokenizer);
    }
    // ';'
    eatPreset(indent, ";");
}


void CompilationEngine::compileClassVarDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<classVarDec>" << endl;
        // ('static'|'field')
        eatPreset(indent, "static", "field");
        pushForwardToken(tokenizer);
        // type varName (',' varName)* ';'
        compileGeneralVarDec(indent, tokenizer);
        pushForwardToken(tokenizer);
    output_xml << string(indent - 1, '\t') << "</classVarDec>" << endl;
}


void CompilationEngine::compileVarDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<varDec>" << endl;
        // 'var'
        eatPreset(indent, "var");
        pushForwardToken(tokenizer);
        // type varName (',' varName)* ';'
        compileGeneralVarDec(indent, tokenizer);
    output_xml << string(indent - 1, '\t') << "</varDec>" << endl;
}


void CompilationEngine::compileSubroutineDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<subroutineDec>" << endl;
        // ('constructor'|'function'|'method')
        eatPreset(indent, "constructor", "function", "method");
        pushForwardToken(tokenizer);
        // ('void'| type)
        if (getTokenType() == "KEYWORD")
            eatPreset(indent, "void");
        else
            compileType(indent);
        pushForwardToken(tokenizer);
        // subroutineName
        eat(indent);
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // parameterList
        compileParameterList(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // subroutineBody
        compileSubroutineBody(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
    output_xml << string(indent - 1, '\t') << "</subroutineDec>" << endl;
}


void CompilationEngine::compileParameterList(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<parameterList>" << endl;
        if (getTokenValue() != ")")
        {
            // type
            compileType(indent);
            pushForwardToken(tokenizer);
            // varName
            eat(indent);
            pushForwardToken(tokenizer);
            // (',' type VarName)*
            while (getTokenValue() != ")")
            {
                eatPreset(indent, ",");  // ','
                pushForwardToken(tokenizer);
                compileType(indent);     // any further type
                pushForwardToken(tokenizer);
                eat(indent);             // any further varName
                pushForwardToken(tokenizer);
            }
        }
        pushTokenBack(tokenizer);  // last token read is ')': drop it as it will be read in the calling routine
    output_xml << string(indent - 1, '\t') << "</parameterList>" << endl;
}


void CompilationEngine::compileSubroutineBody(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<subroutineBody>" << endl;
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // varDec*
        while (getTokenValue() == "var")
        {
            compileVarDec(indent + 1, tokenizer);
            pushForwardToken(tokenizer);
        }
        // statements
        compileStatements(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // '}'
        eatPreset(indent, "}");
    output_xml << string(indent - 1, '\t') << "</subroutineBody>" << endl;
}


void CompilationEngine::compileStatements(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<statements>" << endl;
        // statement*
        while (getTokenValue() == "let" || getTokenValue() == "if" || getTokenValue() == "while" ||
                getTokenValue() == "do" || getTokenValue() == "return")
        {
            if (getTokenValue() == "let")
                compileLetStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "if")
                compileIfStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "while")
                compileWhileStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "do")
                compileDoStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "return")
                compileReturnStatement(indent + 1, tokenizer);

            pushForwardToken(tokenizer);
        }
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
    output_xml << string(indent - 1, '\t') << "</statements>" << endl;
}


void CompilationEngine::compileLetStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<letStatement>" << endl;
        // 'let'
        eatPreset(indent, "let");
        pushForwardToken(tokenizer);
        // varName
        eat(indent);
        pushForwardToken(tokenizer);
        // ( '[' expression ']' )?
        if (getTokenValue() == "[")
        {
            eatPreset(indent, "[");                     // '['
            pushForwardToken(tokenizer);
            compileExpression(indent + 1, tokenizer);   // expression
            pushForwardToken(tokenizer);
            eatPreset(indent, "]");                     // ']'
            pushForwardToken(tokenizer);
        }
        // '='
        eatPreset(indent, "=");
        pushForwardToken(tokenizer);
        // expression
        compileExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ';'
        eatPreset(indent, ";");
    output_xml << string(indent - 1, '\t') << "</letStatement>" << endl;
}


void CompilationEngine::compileIfStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<ifStatement>" << endl;
        // 'if'
        eatPreset(indent, "if");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expression
        compileExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // statements
        compileStatements(indent + 1, tokenizer);
        //pushForwardToken(tokenizer);
        // '}'
        eatPreset(indent, "}");
        pushForwardToken(tokenizer);
        // ('else' '{' statements '}')?
        if (getTokenValue() == "else")
        {
            eatPreset(indent, "else");          // 'else'
            pushForwardToken(tokenizer);
            eatPreset(indent, "{");             // '{'
            pushForwardToken(tokenizer);
            compileStatements(indent + 1, tokenizer);       // statements
            eatPreset(indent, "}");             // '}'
        }
        else
        {
            pushTokenBack(tokenizer);
            //tokenizer.advanceCounter();
        }
    output_xml << string(indent - 1, '\t') << "</ifStatement>" << endl;
}


void CompilationEngine::compileWhileStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<whileStatement>" << endl;
        // 'while'
        eatPreset(indent, "while");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expression
        compileExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // statements
        compileStatements(indent + 1, tokenizer);
        // '}'
        eatPreset(indent, "}");
    output_xml << string(indent - 1, '\t') << "</whileStatement>" << endl;
}


void CompilationEngine::compileDoStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<doStatement>" << endl;
        // 'do'
        eatPreset(indent, "do");
        pushForwardToken(tokenizer);
        // subroutineCall
        compileSubroutineCall(indent, tokenizer);
        pushForwardToken(tokenizer);
        // ';'
        eatPreset(indent, ";");
    output_xml << string(indent - 1, '\t') << "</doStatement>" << endl;
}


void CompilationEngine::compileReturnStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<returnStatement>" << endl;
        // 'return'
        eatPreset(indent, "return");
        pushForwardToken(tokenizer);
        // expression?
        if (getTokenValue() != ";")
        {
            compileExpression(indent + 1, tokenizer);   // expression
            // ';'
            eatPreset(indent, ";");
        }
        else
        {
            // ';'
            eatPreset(indent, ";");
            pushTokenBack(tokenizer);
            //tokenizer.advanceCounter();
        }

    output_xml << string(indent - 1, '\t') << "</returnStatement>" << endl;
}


void CompilationEngine::compileType(const int& indent)
{
    if (getTokenType() == "KEYWORD")
        eatPreset(indent, "int", "char", "boolean", "bool");
    else            // in case type is 'className'
        eat(indent);
}


void CompilationEngine::compileTerm(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<term>" << endl;
        // (   unaryOp term | '(' expression ')' | integerConstant | stringConstant | keywordConstant |
        //     varName | subroutineCall | varName '(' expression ')'   )
        bool is_unary_op = false;
        // unaryOp term
        string operator_unary_op = testIfUnaryOp( is_unary_op, getTokenValue() );
        if (is_unary_op)
        {
            eatPreset(indent, operator_unary_op);
            pushForwardToken(tokenizer);
            compileTerm(indent + 1, tokenizer);
        }
        // '(' expression ')'
        else if (getTokenValue() == "(")
        {
            eatPreset(indent, "(");
            pushForwardToken(tokenizer);
            compileExpression(indent + 1, tokenizer);
            eatPreset(indent, ")");
            pushForwardToken(tokenizer);
            pushForwardToken(tokenizer);///!!!!!!!!!!!!!!! inexplicable fix !!!!!!!!!!! just tinkering ...
        }
        else
        {
            bool is_keyword_constant = false;
            string operator_keyword_constant = testIfKeywordConstant( is_keyword_constant, getTokenValue() );
            // integerConstant | stringConstant | keywordConstant
            if (getTokenType() == "INT_CONST" || getTokenType() == "STRING_CONST" || is_keyword_constant)
            {
                eat(indent);
                pushForwardToken(tokenizer);
            }

            else if (getTokenType() == "IDENTIFIER")
            {
                pushForwardToken(tokenizer);
                // varName <--- array
                if (getTokenValue() == "[")
                {
                    pushTokenBack(tokenizer);
                    pushTokenBack(tokenizer);
                    eat(indent);
                    tokenizer.advanceCounter();
                    pushForwardToken(tokenizer);
                    eatPreset(indent, "[");
                    pushForwardToken(tokenizer);
                    compileExpression(indent + 1, tokenizer);
                    pushForwardToken(tokenizer);
                    eatPreset(indent, "]");
                    pushForwardToken(tokenizer);
                }
                // ( varName '(' expression ')' | subroutineCall )
                else if (getTokenValue() == "(" || getTokenValue() == ".")
                {
                    pushTokenBack(tokenizer);
                    compileSubroutineCall(indent, tokenizer);
                    pushForwardToken(tokenizer);
                }
                // varName
                else
                {
                    pushTokenBack(tokenizer);
                    pushTokenBack(tokenizer);
                    eat(indent);
                    tokenizer.advanceCounter();
                    pushForwardToken(tokenizer);
                }
            }
        }
    output_xml << string(indent - 1, '\t') << "</term>" << endl;
}


void CompilationEngine::compileExpression(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<expression>" << endl;
        // term
        compileTerm(indent + 1, tokenizer);
        // (op term)*
        bool is_op = false;
        string operatore = testIfOp( is_op, getTokenValue() );
        while (is_op)
        {
            eatPreset(indent, getTokenValue() );   // op
            pushForwardToken(tokenizer);
            compileTerm(indent + 1, tokenizer); // term
            operatore = testIfOp( is_op, getTokenValue() );
        }
        pushTokenBack(tokenizer);
    output_xml << string(indent - 1, '\t') << "</expression>" << endl;
}


void CompilationEngine::compileExpressionList(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<expressionList>" << endl;
        bool is_op = false;
        string operator_op = testIfOp( is_op, getTokenValue() );
        bool is_unary_op = false;
        string operator_unary_op = testIfUnaryOp( is_unary_op, getTokenValue() );
        bool is_keyword_constant = false;
        string operator_keyword_constant = testIfKeywordConstant( is_keyword_constant, getTokenValue() );

        // (expression (',' expression)* )?
        if (getTokenValue() != ")")
        {
            compileExpression(indent + 1, tokenizer);
            pushTokenBack(tokenizer);
            tokenizer.advanceCounter();
            pushForwardToken(tokenizer);

            while (getTokenValue() == ",")
            {
                eatPreset(indent + 1, ",");
                pushForwardToken(tokenizer);
                compileExpression(indent + 1, tokenizer);
                pushForwardToken(tokenizer);
            }
        }
        pushTokenBack(tokenizer);
    output_xml << string(indent - 1, '\t') << "</expressionList>" << endl;
}


string CompilationEngine::testIfOp(bool& is_op, const string& value)
{
    is_op = false;
    string ret;

    if (value == "&lt;") is_op = true;
    else if (value == "&gt;") is_op = true;
    else if (value == "&quot;") is_op = true;
    else if (value == "&amp;") is_op = true;
    else
    {
        for (size_t i = 0; i != op_size; ++i)
        {
            if ( value == string(1, op[i]) )
            {
                is_op = true;
                ret = value;
                break;
            }
        }
    }

    ret = value;

    return ret;
}


string CompilationEngine::testIfUnaryOp(bool& is_unary_op, const string& value)
{
    is_unary_op = false;
    string ret;

    for (size_t i = 0; i != unary_op_size; ++i)
    {
        if ( value == string(1, unary_op[i]) )
        {
            is_unary_op = true;
            ret = value;
            break;
        }
    }

    ret = value;

    return ret;
}


string CompilationEngine::testIfKeywordConstant(bool& is_keyword_constant, const string& value)
{
    is_keyword_constant = false;
    string ret;

    for (size_t i = 0; i != keyword_constant_size; ++i)
    {
        if ( getTokenValue() == keyword_constant[i] )
        {
            is_keyword_constant = true;
            ret = value;
            break;
        }
    }

    ret = value;

    return ret;
}


void CompilationEngine::compileSubroutineCall(const int& indent, JackTokenizer& tokenizer)
{
    pushForwardToken(tokenizer);
    // pick between subroutineName '(' expressionList ')'
    // .and.
    // (className|varName) '.' subroutineName '(' expressionList ')'
    if (getTokenValue() == "(")
    {
        pushTokenBack(tokenizer);
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
        // subroutineName
        eat(indent);
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expressionList
        compileExpressionList(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
    }
    else if (getTokenValue() == ".")
    {
        pushTokenBack(tokenizer);
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
        // (className|varName)
        eat(indent);
        pushForwardToken(tokenizer);
        // '.'
        eatPreset(indent, ".");
        pushForwardToken(tokenizer);
        // subroutineName
        eat(indent);
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expressionList
        compileExpressionList(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
    }
}

#endif // COMPILATIONENGINE_CPP_INCLUDED
