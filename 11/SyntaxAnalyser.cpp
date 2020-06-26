#ifndef SYNTAXANALYSER_CPP_INCLUDED
#define SYNTAXANALYSER_CPP_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "SymbolTable.h"
#include "JackTokenizer.h"
#include "SyntaxAnalyser.h"

using std::cout;                using std::endl;
using std::vector;              using std::string;
using std::ofstream;

// static variables that cannot be defined in header file, only declared
const char op_[] = "+-*/&|<>=";
const size_t op_size_ = sizeof(op_) / sizeof(*op_);
const char unary_op_[] = "-~";
const size_t unary_op_size_ = sizeof(unary_op_) / sizeof(*unary_op_);
const string keyword_constant_[] = {"true", "false", "null", "this"};
const size_t keyword_constant_size_ = sizeof(keyword_constant_) / sizeof(*keyword_constant_);

static int output_counter_ = 0;
static int output_counter_class_ = 0;
static string current_class_name;

SyntaxAnalyser::SyntaxAnalyser(const string& filename, const char* argv)
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


void SyntaxAnalyser::importToken(const Token& imported_token)
{
    token = imported_token;
}


void SyntaxAnalyser::write(const int& indent, const string& isclass = "",
                              const string& identifier_is_used = "")    // 'indent' times tab space '\t'
{
    if (getTokenType() == "IDENTIFIER")
    {
        if (isclass == "class")
            output_xml << string(indent, '\t') << "<" << getTokenTypeXml() << "> TYPE: "
                       << class_symbols.typeOf( getTokenValue() ) << " - KIND: "
                       << class_symbols.kindOf( getTokenValue() ) << " - INDEX: "
                       << class_symbols.indexOf( getTokenValue() ) << " **+ "
                       << getTokenValue() << " +** " << identifier_is_used
                       << " </" << getTokenTypeXml() << ">" << endl;
        else if (isclass == "subroutine")
            output_xml << string(indent, '\t') << "<" << getTokenTypeXml() << "> TYPE: "
                       << subroutine_symbols.typeOf( getTokenValue() ) << " - KIND: "
                       << subroutine_symbols.kindOf( getTokenValue() ) << " - INDEX: "
                       << subroutine_symbols.indexOf( getTokenValue() ) << " **+ "
                       << getTokenValue() << " +** " << identifier_is_used
                       << " </" << getTokenTypeXml() << ">" << endl;
        else
            output_xml << string(indent, '\t') << "<" << getTokenTypeXml() << "> KIND: "
                       << isclass << " **+ " << getTokenValue() << " +** </" << getTokenTypeXml()
                       << ">" << endl;
    }
    else
    {
        output_xml << string(indent, '\t') << "<" << getTokenTypeXml() << "> " << getTokenValue()
                   << " </" << getTokenTypeXml() << ">" << endl;
    }
}


void SyntaxAnalyser::eatPreset(const int& indent, const string& current_token, const string& alt_token1 = "",
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


void SyntaxAnalyser::eat(const int& indent, const string& type = "", const string& kind = "",
                            const string& isclass = "")
{
    try
    {
        string identifier_is_used = "defined";
        if (isclass == "class")
        {
            for (SymbolTable::iterator iter = class_symbols.begin(); iter != class_symbols.end(); ++iter)
            {
                if ( iter->first == getTokenValue() )
                {
                    identifier_is_used = "used";
                    break;
                }
            }
        }
        else if (isclass == "subroutine")
        {
            for (SymbolTable::iterator iter = subroutine_symbols.begin(); iter != subroutine_symbols.end(); ++iter)
            {
                if ( iter->first == getTokenValue() )
                {
                    identifier_is_used = "used";
                    break;
                }
            }
        }

        if (getTokenType() == "IDENTIFIER" || getTokenType() == "INT_CONST" ||
            getTokenType() == "STRING_CONST" || getTokenType() == "KEYWORD")
        {
            if (getTokenType() == "IDENTIFIER")// && identifier_is_used == "defined")
            {
                if (isclass == "class")
                    class_symbols.define(getTokenValue(), type, kind);
                else if (isclass == "subroutine")
                    subroutine_symbols.define(getTokenValue(), type, kind);
            }

            write(indent, isclass, identifier_is_used);
        }
        else
            throw getTokenValue();
    }
    catch (string error)
    {
        cout << "Token read incorrectly: " << error << endl;
    }
}


void SyntaxAnalyser::pushForwardToken(JackTokenizer& tokenizer)
{
    //if ( !( tokenizer.getTokenValue().empty() ) )
    //{
    //cout << "token1: " << getTokenValue() << endl;
        tokenizer.advance();
        //cout << "token1: " << getTokenValue() << endl;
        tokenizer.tokenType();
        //cout << "token2: " << getTokenValue() << endl;
        importToken( tokenizer.getToken() );
        //cout << "token3: " << getTokenValue() << endl;
    //}
}


void SyntaxAnalyser::pushTokenBack(JackTokenizer& tokenizer)
{
    tokenizer.stepBack();
    tokenizer.tokenType();
    importToken( tokenizer.getToken() );
}


void SyntaxAnalyser::printSymbolTable(const string& which)
{
    ofstream output;
    SymbolTable table;
    if ( which == "class")
    {
        output.open("check_" + which + "_symbols" + std::to_string(output_counter_class_) + ".txt");
        table = class_symbols;
        //SymbolTable::iterator iter = class_symbols.begin();
    }
    else
    {
        output.open("check_subroutine_symbols" + std::to_string(output_counter_) + ".txt");
        table = subroutine_symbols;
        //SymbolTable::iterator iter = subroutine_symbols.begin();
    }

    SymbolTable::iterator iter = table.begin();
    while( iter != table.end() ){
        output << iter->first << " " << table.kindOf(iter->first) << " "
               << table.typeOf(iter->first) << " "
               << table.indexOf(iter->first) << endl;
        ++iter;}
    output.close();
    if ( which == "class")
        ++output_counter_class_;
    else
        ++output_counter_;
}


void SyntaxAnalyser::analyseClass(JackTokenizer& tokenizer)
{
    output_xml << "<class>" << endl;
        pushForwardToken(tokenizer);
        // 'class'
        eatPreset(1, "class");
        pushForwardToken(tokenizer);
        // className
        eat(1, getTokenType(), "class_name", "class_name");
        current_class_name = getTokenValue();
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(1, "{");
        pushForwardToken(tokenizer);
        // classVarDec*
        class_symbols.startSubroutine();
        while (getTokenValue() == "static" || getTokenValue() == "field")
            analyseClassVarDec(2, tokenizer);
        // subroutineDec*
        while (getTokenValue() == "constructor" || getTokenValue() == "function" || getTokenValue() == "method")
        {
            analyseSubroutineDec(2, tokenizer);
        }
        // '}'
        eatPreset(1, "}");
    output_xml << "</class>" << endl;
    if ( !( class_symbols.getSymbolTable().empty() ) )
    printSymbolTable("class");
}


void SyntaxAnalyser::analyseGeneralVarDec(const int& indent, JackTokenizer& tokenizer,
                                             const string& aux_field = "", const string& isclass = "")
{
    // type
    analyseType(indent);
    string aux_type = getTokenValue();
    pushForwardToken(tokenizer);
    // varName
    eat(indent, aux_type, aux_field, isclass);       // SymbolTable will be defined inside 'eat()'
    pushForwardToken(tokenizer);
    // (',' varName)*
    while (getTokenValue() != ";")
    {
        eatPreset(indent, ",");
        pushForwardToken(tokenizer);
        eat(indent, aux_type, aux_field, isclass);    // any further varName
        pushForwardToken(tokenizer);
    }
    // ';'
    eatPreset(indent, ";");
}


void SyntaxAnalyser::analyseClassVarDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<classVarDec>" << endl;
        // ('static'|'field')
        eatPreset(indent, "static", "field");
        string aux_field = getTokenValue();     // will hold "static" or "field"
        pushForwardToken(tokenizer);
        // type varName (',' varName)* ';'
        analyseGeneralVarDec(indent, tokenizer, aux_field, "class");    //...(indent, tok., aux_field, isclass);
        pushForwardToken(tokenizer);
    output_xml << string(indent - 1, '\t') << "</classVarDec>" << endl;
}


void SyntaxAnalyser::analyseVarDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<varDec>" << endl;
        // 'var'
        eatPreset(indent, "var");
        pushForwardToken(tokenizer);
        // type varName (',' varName)* ';'
        analyseGeneralVarDec(indent, tokenizer, "var", "subroutine");    //...(indent, tok., aux_field, isclass);
    output_xml << string(indent - 1, '\t') << "</varDec>" << endl;
}


void SyntaxAnalyser::analyseSubroutineDec(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<subroutineDec>" << endl;
        // ('constructor'|'function'|'method')
        subroutine_symbols.startSubroutine();       // start the SymbolTable for subroutine here
        eatPreset(indent, "constructor", "function", "method");
        if (getTokenValue() == "method")
            subroutine_symbols.define("this", current_class_name, "arg");
        pushForwardToken(tokenizer);
        // ('void' | type)
        if (getTokenValue() == "void")
            eatPreset(indent, "void");
        else
            analyseType(indent);
        pushForwardToken(tokenizer);
        // subroutineName
        eat(indent, getTokenType(), "subroutine_name", "subroutine_name");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // parameterList
        //subroutine_symbols.startSubroutine();       // start the SymbolTable for subroutine here
        analyseParameterList(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // subroutineBody
        analyseSubroutineBody(indent + 1, tokenizer);
//        cout << "here?: " << getTokenValue() << endl;
        if (getTokenValue() == "}")
        {
//            cout << "token before: " << getTokenValue() << endl;
            pushForwardToken(tokenizer);
//            cout << "token: " << getTokenValue() << endl;
        }
        //cout << "subroutine out, token: " << getTokenValue() << endl;

    output_xml << string(indent - 1, '\t') << "</subroutineDec>" << endl;
    if ( !( subroutine_symbols.getSymbolTable().empty() ) )
        printSymbolTable("subroutine");
}


void SyntaxAnalyser::analyseParameterList(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<parameterList>" << endl;
        if (getTokenValue() != ")")
        {
            // type
            analyseType(indent);
            string aux_type_sub = getTokenValue();
            pushForwardToken(tokenizer);
            // varName
            eat(indent, aux_type_sub, "arg", "subroutine"); // ...(indent, type, kind, isclass);
            pushForwardToken(tokenizer);
            // (',' type VarName)*
            while (getTokenValue() != ")")
            {
                eatPreset(indent, ",");  // ','
                pushForwardToken(tokenizer);
                analyseType(indent);     // any further type
                aux_type_sub = getTokenValue();
                pushForwardToken(tokenizer);
                eat(indent, aux_type_sub, "arg", "subroutine");             // any further varName
                pushForwardToken(tokenizer);
            }
        }
        pushTokenBack(tokenizer);  // last token read is ')': drop it as it will be read in the calling routine
    output_xml << string(indent - 1, '\t') << "</parameterList>" << endl;
}


void SyntaxAnalyser::analyseSubroutineBody(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<subroutineBody>" << endl;
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // varDec*
        while (getTokenValue() == "var")
        {
            analyseVarDec(indent + 1, tokenizer);
            pushForwardToken(tokenizer);
        }
        // statements
        analyseStatements(indent + 1, tokenizer);
        // '}'
        if (getTokenValue() == "}")
            eatPreset(indent, "}");
        pushForwardToken(tokenizer);
//        if (getTokenValue() == "}")
//            eatPreset(indent, "}");
    output_xml << string(indent - 1, '\t') << "</subroutineBody>" << endl;
}


void SyntaxAnalyser::analyseStatements(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<statements>" << endl;
        // statement*
        while (getTokenValue() == "let" || getTokenValue() == "if" || getTokenValue() == "while" ||
                getTokenValue() == "do" || getTokenValue() == "return")
        {
            if (getTokenValue() == "let")
                analyseLetStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "if")
                analyseIfStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "while")
                analyseWhileStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "do")
                analyseDoStatement(indent + 1, tokenizer);
            else if (getTokenValue() == "return")
                analyseReturnStatement(indent + 1, tokenizer);

            pushForwardToken(tokenizer);
        }
        pushTokenBack(tokenizer);
        //pushForwardToken(tokenizer);
        tokenizer.advanceCounter();
    output_xml << string(indent - 1, '\t') << "</statements>" << endl;
}

//eat(const int& indent, const string& type = "", const string& kind = "", const string& isclass = "")
//eat(indent, aux_type_sub, "arg", "subroutine");
void SyntaxAnalyser::analyseLetStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<letStatement>" << endl;
        // 'let'
        eatPreset(indent, "let");
        pushForwardToken(tokenizer);

        // varName
        string kind, isclass;
        checkIdentifier(kind, isclass);
        eat(indent, getTokenType(), kind, isclass);
        pushForwardToken(tokenizer);
        // ( '[' expression ']' )?
        if (getTokenValue() == "[")
        {
            eatPreset(indent, "[");                     // '['
            pushForwardToken(tokenizer);
            analyseExpression(indent + 1, tokenizer);   // expression
            pushForwardToken(tokenizer);
            eatPreset(indent, "]");                     // ']'
            pushForwardToken(tokenizer);
        }
        // '='
        eatPreset(indent, "=");
        pushForwardToken(tokenizer);
        // expression
        analyseExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ';'
        eatPreset(indent, ";");
    output_xml << string(indent - 1, '\t') << "</letStatement>" << endl;
}


void SyntaxAnalyser::checkIdentifier(string& kind, string& isclass)
{
    SymbolTable::iterator in_class = class_symbols.find( getTokenValue() );
    SymbolTable::iterator in_subroutine = subroutine_symbols.find( getTokenValue() );
    if ( in_class != class_symbols.end() )
    {
        kind = class_symbols.kindOf(in_class->first);
        isclass = "class";
    }
    else if ( in_subroutine != subroutine_symbols.end() )
    {
        kind = subroutine_symbols.kindOf(in_subroutine->first);
        isclass = "subroutine";
    }
    else
    {
        kind = "class_name";
        isclass = "class_name";
    }
}


void SyntaxAnalyser::analyseIfStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<ifStatement>" << endl;
        // 'if'
        eatPreset(indent, "if");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expression
        analyseExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // statements
        analyseStatements(indent + 1, tokenizer);
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
            analyseStatements(indent + 1, tokenizer);       // statements
            eatPreset(indent, "}");             // '}'
        }
        else
        {
            pushTokenBack(tokenizer);
            //tokenizer.advanceCounter();
        }
    output_xml << string(indent - 1, '\t') << "</ifStatement>" << endl;
}


void SyntaxAnalyser::analyseWhileStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<whileStatement>" << endl;
        // 'while'
        eatPreset(indent, "while");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expression
        analyseExpression(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
        pushForwardToken(tokenizer);
        // '{'
        eatPreset(indent, "{");
        pushForwardToken(tokenizer);
        // statements
        analyseStatements(indent + 1, tokenizer);
        // '}'
        eatPreset(indent, "}");
    output_xml << string(indent - 1, '\t') << "</whileStatement>" << endl;
}


void SyntaxAnalyser::analyseDoStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<doStatement>" << endl;
        // 'do'
        eatPreset(indent, "do");
        pushForwardToken(tokenizer);
        // subroutineCall
        analyseSubroutineCall(indent, tokenizer);
        pushForwardToken(tokenizer);
        // ';'
        eatPreset(indent, ";");
    output_xml << string(indent - 1, '\t') << "</doStatement>" << endl;
}


void SyntaxAnalyser::analyseReturnStatement(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<returnStatement>" << endl;
        // 'return'
        eatPreset(indent, "return");
        pushForwardToken(tokenizer);
        // expression?
        if (getTokenValue() != ";")
        {
            analyseExpression(indent + 1, tokenizer);   // expression
            // ';'
            eatPreset(indent, ";");
            tokenizer.advanceCounter();
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


void SyntaxAnalyser::analyseType(const int& indent)
{
    if (getTokenType() == "KEYWORD")
        eatPreset(indent, "int", "char", "boolean", "bool");
    else            // in case type is 'className'
        eat(indent, "", "", "class_name");
}


void SyntaxAnalyser::analyseTerm(const int& indent, JackTokenizer& tokenizer)
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
            analyseTerm(indent + 1, tokenizer);
        }
        // '(' expression ')'
        else if (getTokenValue() == "(")
        {
            eatPreset(indent, "(");
            pushForwardToken(tokenizer);
            analyseExpression(indent + 1, tokenizer);
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

                    string kind, isclass;
                    checkIdentifier(kind, isclass);
                    eat(indent, getTokenType(), kind, isclass);

                    tokenizer.advanceCounter();
                    pushForwardToken(tokenizer);
                    eatPreset(indent, "[");
                    pushForwardToken(tokenizer);
                    analyseExpression(indent + 1, tokenizer);
                    pushForwardToken(tokenizer);
                    eatPreset(indent, "]");
                    pushForwardToken(tokenizer);
                }
                // ( varName '(' expression ')' | subroutineCall )
                else if (getTokenValue() == "(" || getTokenValue() == ".")
                {
                    pushTokenBack(tokenizer);
                    analyseSubroutineCall(indent, tokenizer);
                    pushForwardToken(tokenizer);
                }
                // varName
                else
                {
                    pushTokenBack(tokenizer);
                    pushTokenBack(tokenizer);
                    string kind, isclass;
                    checkIdentifier(kind, isclass);
                    eat(indent, getTokenType(), kind, isclass);
                    //eat(indent);
                    tokenizer.advanceCounter();
                    pushForwardToken(tokenizer);
                }
            }
        }
    output_xml << string(indent - 1, '\t') << "</term>" << endl;
}


void SyntaxAnalyser::analyseExpression(const int& indent, JackTokenizer& tokenizer)
{
    output_xml << string(indent - 1, '\t') << "<expression>" << endl;
        // term
        analyseTerm(indent + 1, tokenizer);
        // (op term)*
        bool is_op = false;
        string operatore = testIfOp( is_op, getTokenValue() );
        while (is_op)
        {
            eatPreset(indent, getTokenValue() );   // op
            pushForwardToken(tokenizer);
            analyseTerm(indent + 1, tokenizer); // term
            operatore = testIfOp( is_op, getTokenValue() );
        }
        pushTokenBack(tokenizer);
    output_xml << string(indent - 1, '\t') << "</expression>" << endl;
}


void SyntaxAnalyser::analyseExpressionList(const int& indent, JackTokenizer& tokenizer)
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
            analyseExpression(indent + 1, tokenizer);
            pushTokenBack(tokenizer);
            tokenizer.advanceCounter();
            pushForwardToken(tokenizer);

            while (getTokenValue() == ",")
            {
                eatPreset(indent + 1, ",");
                pushForwardToken(tokenizer);
                analyseExpression(indent + 1, tokenizer);
                pushForwardToken(tokenizer);
            }
        }
        pushTokenBack(tokenizer);
    output_xml << string(indent - 1, '\t') << "</expressionList>" << endl;
}


string SyntaxAnalyser::testIfOp(bool& is_op, const string& value)
{
    is_op = false;
    string ret;

    if (value == "&lt;") is_op = true;
    else if (value == "&gt;") is_op = true;
    else if (value == "&quot;") is_op = true;
    else if (value == "&amp;") is_op = true;
    else
    {
        for (size_t i = 0; i != op_size_; ++i)
        {
            if ( value == string(1, op_[i]) )
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


string SyntaxAnalyser::testIfUnaryOp(bool& is_unary_op, const string& value)
{
    is_unary_op = false;
    string ret;

    for (size_t i = 0; i != unary_op_size_; ++i)
    {
        if ( value == string(1, unary_op_[i]) )
        {
            is_unary_op = true;
            ret = value;
            break;
        }
    }

    ret = value;

    return ret;
}


string SyntaxAnalyser::testIfKeywordConstant(bool& is_keyword_constant, const string& value)
{
    is_keyword_constant = false;
    string ret;

    for (size_t i = 0; i != keyword_constant_size_; ++i)
    {
        if ( getTokenValue() == keyword_constant_[i] )
        {
            is_keyword_constant = true;
            ret = value;
            break;
        }
    }

    ret = value;

    return ret;
}


void SyntaxAnalyser::analyseSubroutineCall(const int& indent, JackTokenizer& tokenizer)
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
        eat(indent, "", "subroutine_name", "subroutine_name");
        //eat(indent);
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expressionList
        analyseExpressionList(indent + 1, tokenizer);
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
        string kind, isclass;
        checkIdentifier(kind, isclass);
        eat(indent, getTokenType(), kind, isclass);
        //eat(indent);

        pushForwardToken(tokenizer);
        // '.'
        eatPreset(indent, ".");
        pushForwardToken(tokenizer);
        // subroutineName
        eat(indent, "", "subroutine_name", "subroutine_name");
        pushForwardToken(tokenizer);
        // '('
        eatPreset(indent, "(");
        pushForwardToken(tokenizer);
        // expressionList
        analyseExpressionList(indent + 1, tokenizer);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(indent, ")");
    }
}

#endif // SYNTAXANALYSER_CPP_INCLUDED
