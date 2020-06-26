#ifndef COMPILATIONENGINE_CPP_INCLUDED
#define COMPILATIONENGINE_CPP_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VmWriter.h"
#include "CompilationEngine.h"

using std::cout;                using std::endl;
using std::vector;              using std::string;
using std::stoi;                using std::to_string;
using std::ofstream;

// static variables that cannot be defined in header file, only declared
const char op[] = "+-*/&|<>=";
const size_t op_size = sizeof(op) / sizeof(*op);
const char unary_op[] = "-~";
const size_t unary_op_size = sizeof(unary_op) / sizeof(*unary_op);
const string keyword_constant[] = {"true", "false", "null", "this"};
const size_t keyword_constant_size = sizeof(keyword_constant) / sizeof(*keyword_constant);

static int output_counter = 0;
static int output_counter_class = 0;

CompilationEngine::CompilationEngine(const string& filename, const char* argv)
{
    current_class_name = filename;

    global_if_counter = 0;
    global_while_counter = 0;
}


void CompilationEngine::importToken(const Token& imported_token)
{
    token = imported_token;
}


void CompilationEngine::write(const string& isclass = "",
                              const string& identifier_is_used = "")
{
//    void, used by the SyntaxAnalyser
}


void CompilationEngine::eatPreset(const string& current_token, const string& alt_token1 = "",
                                  const string& alt_token2 = "", const string& alt_token3 = "",
                                  const string& alt_token4 = "")
{
//    empty, just used as placeholder to keep track of parsed token in the subroutines flow
}


void CompilationEngine::eat(const string& type = "", const string& kind = "",
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
            if (getTokenType() == "IDENTIFIER")
            {
                if (isclass == "class")
                    class_symbols.define(getTokenValue(), type, kind);
                else if (isclass == "subroutine")
                    subroutine_symbols.define(getTokenValue(), type, kind);
            }
            // write(isclass, identifier_is_used);       // write in the SyntaxAnalyser, not here
        }
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


void CompilationEngine::printSymbolTable(const string& which)
{
    ofstream output;
    SymbolTable table;
    if ( which == "class")
    {
        output.open("check_" + which + "_symbols" + std::to_string(output_counter_class) + ".txt");
        table = class_symbols;
    }
    else
    {
        output.open("check_subroutine_symbols" + std::to_string(output_counter) + ".txt");
        table = subroutine_symbols;
    }

    SymbolTable::iterator iter = table.begin();
    while( iter != table.end() ){
        output << iter->first << " " << table.kindOf(iter->first) << " "
               << table.typeOf(iter->first) << " "
               << table.indexOf(iter->first) << endl;
        ++iter;}
    output.close();
    if ( which == "class")
        ++output_counter_class;
    else
        ++output_counter;
}


void CompilationEngine::compileClass(JackTokenizer& tokenizer, const string& filename, const char* argv)
{
    output_vm = VmWriter(filename, argv);

        pushForwardToken(tokenizer);
        // 'class'
        eatPreset("class");
        pushForwardToken(tokenizer);
        // className
        eat(getTokenType(), "class_name", "class_name");
        current_class_name = getTokenValue();               // static
        pushForwardToken(tokenizer);
        // '{'
        eatPreset("{");
        pushForwardToken(tokenizer);
        // classVarDec*
        class_symbols.startSubroutine();
        while (getTokenValue() == "static" || getTokenValue() == "field")
            compileClassVarDec(tokenizer);
        // subroutineDec*
        while (getTokenValue() == "constructor" || getTokenValue() == "function" || getTokenValue() == "method")
        {
            compileSubroutineDec(tokenizer);
        }
        // '}'
        eatPreset("}");
    output_vm.close();

    if ( !( class_symbols.getSymbolTable().empty() ) )
        printSymbolTable("class");
}


void CompilationEngine::compileGeneralVarDec(JackTokenizer& tokenizer,
                                             const string& aux_field = "", const string& isclass = "")
{
    // type
    compileType();
    string aux_type = getTokenValue();
    pushForwardToken(tokenizer);
    // varName
    eat(aux_type, aux_field, isclass);      // SymbolTable will be defined inside 'eat()'
    pushForwardToken(tokenizer);
    // (',' varName)*
    while (getTokenValue() != ";")
    {
        eatPreset(",");
        pushForwardToken(tokenizer);
        eat(aux_type, aux_field, isclass);  // any further varName
        pushForwardToken(tokenizer);
    }
    // ';'
    eatPreset(";");
}


void CompilationEngine::compileClassVarDec(JackTokenizer& tokenizer)
{
    // ('static'|'field')
    eatPreset("static", "field");
    string aux_field = getTokenValue();     // will hold "static" or "field"
    pushForwardToken(tokenizer);
    // type varName (',' varName)* ';'
    compileGeneralVarDec(tokenizer, aux_field, "class");    //...(tokenizer, aux_field, isclass);
    pushForwardToken(tokenizer);
}


void CompilationEngine::compileVarDec(JackTokenizer& tokenizer)
{
    // 'var'
    eatPreset("var");
    pushForwardToken(tokenizer);
    // type varName (',' varName)* ';'
    compileGeneralVarDec(tokenizer, "VAR", "subroutine");    //...(tok., aux_field, isclass);
}


void CompilationEngine::compileSubroutineDec(JackTokenizer& tokenizer)
{
    // ('constructor'|'function'|'method')
    subroutine_symbols.startSubroutine();       // start the SymbolTable for subroutine here

    eatPreset("constructor", "function", "method");
    subroutine_type = getTokenValue();
    if (subroutine_type == "method")
    {
        subroutine_type = "method";
        subroutine_symbols.define("this", current_class_name, "ARG");
    }
    else if (subroutine_type == "constructor")
    {
        subroutine_type = "constructor";
    }
    pushForwardToken(tokenizer);
    // ('void'| type)
    if (getTokenValue() == "void")
        eatPreset("void");
    else
        compileType();
    pushForwardToken(tokenizer);
    // subroutineName
    eat(getTokenType(), "subroutine_name", "subroutine_name");
    current_subroutine_name = getTokenValue();
    pushForwardToken(tokenizer);
    // '('
    eatPreset("(");
    pushForwardToken(tokenizer);
    // parameterList
    compileParameterList(tokenizer);
    pushForwardToken(tokenizer);
    // ')'
    eatPreset(")");
    pushForwardToken(tokenizer);
    // subroutineBody
    compileSubroutineBody(tokenizer);
    if( getTokenValue() == "}")
            pushForwardToken(tokenizer);
    if ( !( subroutine_symbols.getSymbolTable().empty() ) )
        printSymbolTable("subroutine");
}


void CompilationEngine::compileType()
{
    if (getTokenType() == "KEYWORD")
        eatPreset("int", "char", "boolean", "bool");
    else            // in case type is 'className'
        eat("", "", "class_name");
}


void CompilationEngine::compileParameterList(JackTokenizer& tokenizer)
{
    if (getTokenValue() != ")")
    {
        // type
        compileType();
        string aux_type_sub = getTokenValue();
        pushForwardToken(tokenizer);
        // varName
        eat(aux_type_sub, "ARG", "subroutine"); // ...(type, kind, isclass);
        pushForwardToken(tokenizer);
        // (',' type VarName)*
        while (getTokenValue() != ")")
        {
            eatPreset(",");  // ','
            pushForwardToken(tokenizer);
            compileType();     // any further type
            aux_type_sub = getTokenValue();
            pushForwardToken(tokenizer);
            eat(aux_type_sub, "ARG", "subroutine");             // any further varName
            pushForwardToken(tokenizer);
        }
    }
    pushTokenBack(tokenizer);  // last token read is ')': drop it as it will be read in the calling routine
}


void CompilationEngine::compileSubroutineBody(JackTokenizer& tokenizer)
{
    // '{'
    eatPreset("{");
    pushForwardToken(tokenizer);
    // varDec*
    while (getTokenValue() == "var")
    {
        compileVarDec(tokenizer);
        pushForwardToken(tokenizer);
    }
    output_vm.writeFunction( current_class_name + "." + current_subroutine_name, subroutine_symbols.varCount("VAR") );
    if (subroutine_type == "constructor")
    {
        output_vm.writePush( "CONST", class_symbols.varCount("field") );
        output_vm.writeCall("Memory.alloc", 1);
        output_vm.writePop("POINTER", 0);
    }
    else if (subroutine_type == "method")
    {
        output_vm.writePush("ARG", 0);
        output_vm.writePop("POINTER", 0);
    }
    // statements
    compileStatements(tokenizer);
    // '}'
    if (getTokenValue() == "}")
        eatPreset("}");
    pushForwardToken(tokenizer);
    if (getTokenValue() == "}")
        eatPreset("}");
}


void CompilationEngine::compileStatements(JackTokenizer& tokenizer)
{
    // statement*
    while (getTokenValue() == "let" || getTokenValue() == "if" || getTokenValue() == "while" ||
            getTokenValue() == "do" || getTokenValue() == "return")
    {
        if (getTokenValue() == "let")
            compileLetStatement(tokenizer);
        else if (getTokenValue() == "if")
            compileIfStatement(tokenizer);
        else if (getTokenValue() == "while")
            compileWhileStatement(tokenizer);
        else if (getTokenValue() == "do")
            compileDoStatement(tokenizer);
        else if (getTokenValue() == "return")
            compileReturnStatement(tokenizer);

        pushForwardToken(tokenizer);
    }
    pushTokenBack(tokenizer);
    tokenizer.advanceCounter();
}


void CompilationEngine::compileLetStatement(JackTokenizer& tokenizer)
{
    // 'let'
    eatPreset("let");
    pushForwardToken(tokenizer);

    // varName
    string lhs_kind, lhs_isclass, lhs;  // characteristics of the L.eft H.and S.ide token of the expression
    checkIdentifier(lhs_kind, lhs_isclass);
    eat(getTokenValue(), lhs_kind, lhs_isclass);
    lhs = getTokenValue();
    pushForwardToken(tokenizer);
    // ( '[' expression ']' )?
    string array_index = "0";
    if (getTokenValue() == "[")
    {
        eatPreset("[");                 // '['
        pushForwardToken(tokenizer);
        compileExpression(tokenizer);   // expression
        array_index = getTokenValue();
        pushForwardToken(tokenizer);
        eatPreset("]");                 // ']'
        pushForwardToken(tokenizer);
    }
    if (array_index != "0")
    {
        if (lhs_isclass == "class")
            output_vm.writePush( lhs_kind, class_symbols.indexOf(lhs) );
        else if (lhs_isclass == "subroutine")
            output_vm.writePush( lhs_kind, subroutine_symbols.indexOf(lhs) );
        output_vm.writeArithmetic("add");
    }
    // '='
    eatPreset("=");
    pushForwardToken(tokenizer);
    // expression
    compileExpression(tokenizer);
    pushForwardToken(tokenizer);
    // ';'
    eatPreset(";");
    if (array_index != "0")
    {
        output_vm.writePop("TEMP", 0);
        output_vm.writePop("POINTER", 1);
        output_vm.writePush("TEMP", 0);
        output_vm.writePop("THAT", 0);
    }
    else
    {
        if (lhs_isclass == "class")
            output_vm.writePop( lhs_kind, class_symbols.indexOf(lhs) );
        else if (lhs_isclass == "subroutine")
            output_vm.writePop( lhs_kind, subroutine_symbols.indexOf(lhs) );
    }
}


// locates identifier in the class or subroutine symbol tables. If not there, then it is a className or subroutineName
void CompilationEngine::checkIdentifier(string& kind, string& isclass)
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


void CompilationEngine::compileIfStatement(JackTokenizer& tokenizer)
{
    global_if_counter++;
    int if_counter = global_if_counter;
    string else_label = "ELSE";
    string endif_label = "END_IF";
    // 'if'
    eatPreset("if");
    pushForwardToken(tokenizer);
    // '('
    eatPreset("(");
    pushForwardToken(tokenizer);
    // expression
    compileExpression(tokenizer);
    output_vm.writeArithmetic("~");
    output_vm.writeIf( else_label + to_string(if_counter) );
    pushForwardToken(tokenizer);
    // ')'
    eatPreset(")");
    pushForwardToken(tokenizer);
    // '{'
    eatPreset("{");
    pushForwardToken(tokenizer);
    // statements
    compileStatements(tokenizer);
    // '}'
    eatPreset("}");
    pushForwardToken(tokenizer);

    output_vm.writeGoto( endif_label + to_string(if_counter) );
    output_vm.writeLabel( else_label + to_string(if_counter) );
    // ('else' '{' statements '}')?
    if (getTokenValue() == "else")
    {
        eatPreset("else");          // 'else'
        pushForwardToken(tokenizer);
        eatPreset("{");             // '{'
        pushForwardToken(tokenizer);
        compileStatements(tokenizer);       // statements
        eatPreset("}");             // '}'
    }
    else
        pushTokenBack(tokenizer);
    output_vm.writeLabel(endif_label + to_string(if_counter) );
}


void CompilationEngine::compileWhileStatement(JackTokenizer& tokenizer)
{
    global_while_counter++;
    int while_counter = global_while_counter;
    string while_label = "WHILE";
    string endwhile_label = "END_WHILE";
    // 'while'
    eatPreset("while");
    pushForwardToken(tokenizer);
    output_vm.writeLabel( while_label + to_string(while_counter) );
    // '('
    eatPreset("(");
    pushForwardToken(tokenizer);
    // expression
    compileExpression(tokenizer);
    output_vm.writeArithmetic("~");
    output_vm.writeIf( endwhile_label + to_string(while_counter) );
    pushForwardToken(tokenizer);
    // ')'
    eatPreset(")");
    pushForwardToken(tokenizer);
    // '{'
    eatPreset("{");
    pushForwardToken(tokenizer);
    // statements
    compileStatements(tokenizer);
    output_vm.writeGoto( while_label + to_string(while_counter) );
    // '}'
    eatPreset("}");
    output_vm.writeLabel( endwhile_label + to_string(while_counter) );
}


void CompilationEngine::compileDoStatement(JackTokenizer& tokenizer)
{
    // 'do'
    eatPreset("do");
    pushForwardToken(tokenizer);
    // subroutineCall
    compileSubroutineCall(tokenizer);
    output_vm.writePop("TEMP", 0);
    pushForwardToken(tokenizer);
    // ';'
    eatPreset(";");
}


void CompilationEngine::compileReturnStatement(JackTokenizer& tokenizer)
{
    // 'return'
    eatPreset("return");
    pushForwardToken(tokenizer);
    // expression?
    if (getTokenValue() != ";")
    {
        compileExpression(tokenizer);   // expression
        // ';'
        eatPreset(";");
        tokenizer.advanceCounter();
    }
    else
    {
        // ';'
        eatPreset(";");
        pushTokenBack(tokenizer);
        output_vm.writePush("CONST", 0);
    }

    output_vm.writeReturn();
}


void CompilationEngine::compileTerm(JackTokenizer& tokenizer)
{
    // (   unaryOp term | '(' expression ')' | integerConstant | stringConstant | keywordConstant |
    //     varName | subroutineCall | varName '(' expression ')'   )
    bool is_unary_op = false;
    // unaryOp term
    string operator_unary_op = testIfUnaryOp( is_unary_op, getTokenValue() );
    if (is_unary_op)
    {
        eatPreset(operator_unary_op);
        pushForwardToken(tokenizer);
        compileTerm(tokenizer);
        if (operator_unary_op == "-")       // unary op 'neg' different from 'sub', but both use '-'
            output_vm.writeArithmetic("-NEG");
        else                                // it must be a 'NOT' tilde then
            output_vm.writeArithmetic(operator_unary_op);
    }
    // '(' expression ')'
    else if (getTokenValue() == "(")
    {
        eatPreset("(");
        pushForwardToken(tokenizer);
        compileExpression(tokenizer);
        eatPreset(")");
        pushForwardToken(tokenizer);
        pushForwardToken(tokenizer);    ///!!!!!!!!!!!!!!! inexplicable fix !!!!!!!!!!! just tinkering ...
    }
    else
    {
        bool is_keyword_constant = false;
        string operator_keyword_constant = testIfKeywordConstant( is_keyword_constant, getTokenValue() );
        // integerConstant | stringConstant | keywordConstant
        if (getTokenType() == "INT_CONST")
        {
            eat();
            output_vm.writePush( "CONST", stoi(getTokenValue()) );
            pushForwardToken(tokenizer);
        }
        else if (getTokenType() == "STRING_CONST")
        {
            eat();
            output_vm.writePush( "CONST", getTokenValue().size() );
            output_vm.writeCall("String.new", 1);
            string to_be_printed( getTokenValue() );
            for (string::iterator iter = to_be_printed.begin(); iter != to_be_printed.end(); ++iter)
            {
                output_vm.writePush("CONST", *iter);
                output_vm.writeCall("String.appendChar", 2);
            }
            //output_vm.writeCall("Output.printString", 1);
            //output_vm.writePop("TEMP", 0);      // Output.printString returns void
            pushForwardToken(tokenizer);
        }
        else if (is_keyword_constant)   //"true", "false", "null", "this"
        {
            eat();
            if (getTokenValue() == "true")
            {
                output_vm.writePush("CONST", 0);
                output_vm.writeArithmetic("~");
            }
            else if (getTokenValue() == "false")
            {
                output_vm.writePush("CONST", 0);
            }
            else if (getTokenValue() == "null")
            {
                output_vm.writePush("CONST", 0);
            }
            else if (getTokenValue() == "this")
            {
                output_vm.writePush("POINTER", 0);
            }
            pushForwardToken(tokenizer);
        }
        // varName | subroutineCall | varName '[' expression ']'
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
                eat(getTokenType(), kind, isclass);
//                    push local 2 i
//                    push local 0 a
//                    add          a + i = a[i]
//                    pop pointer 1
//                    push that 0
                Table name_of_array;
                if (isclass == "class")
                    name_of_array = Table(getTokenValue(), getTokenType(), kind, class_symbols.indexOf( getTokenValue() ) );
                else if (isclass == "subroutine")
                    name_of_array = Table(getTokenValue(), getTokenType(), kind, subroutine_symbols.indexOf( getTokenValue()));
                tokenizer.advanceCounter();
                pushForwardToken(tokenizer);
                eatPreset("[");
                pushForwardToken(tokenizer);
                compileExpression(tokenizer);
                pushForwardToken(tokenizer);
                eatPreset("]");
                output_vm.writePush(name_of_array.kind, name_of_array.entry_counter);
                output_vm.writeArithmetic("+");
                output_vm.writePop("POINTER", 1);
                output_vm.writePush("THAT", 0);
                pushForwardToken(tokenizer);
            }
            // ( varName '(' expression ')' | subroutineCall )
            else if (getTokenValue() == "(" || getTokenValue() == ".")
            {
                pushTokenBack(tokenizer);
                compileSubroutineCall(tokenizer);
                pushForwardToken(tokenizer);
            }
            // varName
            else
            {
                pushTokenBack(tokenizer);
                pushTokenBack(tokenizer);
                string kind, isclass;
                checkIdentifier(kind, isclass);
                eat(getTokenType(), kind, isclass);
                if (isclass == "class")
                    output_vm.writePush(kind, class_symbols.indexOf( getTokenValue() ));
                else if (isclass == "subroutine")
                    output_vm.writePush(kind, subroutine_symbols.indexOf( getTokenValue() ));
                tokenizer.advanceCounter();
                pushForwardToken(tokenizer);
            }
        }
    }
}


void CompilationEngine::compileExpression(JackTokenizer& tokenizer)
{
    // term
    compileTerm(tokenizer);
    // (op term)*
    bool is_op = false;
    string operatore = testIfOp( is_op, getTokenValue() );
    while (is_op)
    {
        eatPreset( getTokenValue() );    // op
        string operatore1 = getTokenValue();
        pushForwardToken(tokenizer);
        compileTerm(tokenizer);          // term
        operatore = testIfOp( is_op, getTokenValue() );
        if (operatore1 == "*")
            output_vm.writeCall("Math.multiply", 2);
        else if (operatore1 == "/")
            output_vm.writeCall("Math.divide", 2);
        else
            output_vm.writeArithmetic(operatore1);
    }
    pushTokenBack(tokenizer);
}


void CompilationEngine::compileExpressionList(JackTokenizer& tokenizer, int& counter_commas)
{
    bool is_op = false;
    string operator_op = testIfOp( is_op, getTokenValue() );
    bool is_unary_op = false;
    string operator_unary_op = testIfUnaryOp( is_unary_op, getTokenValue() );
    bool is_keyword_constant = false;
    string operator_keyword_constant = testIfKeywordConstant( is_keyword_constant, getTokenValue() );
    // (expression (',' expression)* )?
    if (getTokenValue() != ")")
    {
        ++counter_commas;
        compileExpression(tokenizer);
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
        pushForwardToken(tokenizer);

        while (getTokenValue() == ",")
        {
            eatPreset(",");
            ++counter_commas;
            pushForwardToken(tokenizer);
            compileExpression(tokenizer);
            pushForwardToken(tokenizer);
        }
    }
    pushTokenBack(tokenizer);
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


void CompilationEngine::compileSubroutineCall(JackTokenizer& tokenizer)
{
    pushForwardToken(tokenizer);
    // pick between subroutineName '(' expressionList ')'
    // .or.
    // (className|varName) '.' subroutineName '(' expressionList ')'
    if (getTokenValue() == "(")
    {
        pushTokenBack(tokenizer);
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
        // subroutineName
        output_vm.writePush("pointer", 0);          // push 'this' object address before the other subroutine arguments
        eat("", "subroutine_name", "subroutine_name");
        current_subroutine_name = getTokenValue();
        pushForwardToken(tokenizer);
        // '('
        eatPreset("(");
        pushForwardToken(tokenizer);
        string save_current_subroutine_name = current_subroutine_name;
        // expressionList
        int count_commas_in_expression_list = 0;
        compileExpressionList(tokenizer, count_commas_in_expression_list);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(")");
        output_vm.writeCall(current_class_name + "." + save_current_subroutine_name,
                            count_commas_in_expression_list + 1); // +1 is the index
    }
    else if (getTokenValue() == ".")
    {
        pushTokenBack(tokenizer);
        pushTokenBack(tokenizer);
        tokenizer.advanceCounter();
        // (className|varName)
        string kind, isclass;
        checkIdentifier(kind, isclass);
        eat(getTokenType(), kind, isclass);
        string external_class_name = getTokenValue();
        if ( kind != "class_name")
        {
            if ( kind == "field" || kind == "static")   // push 'this' object address before the other subroutine arguments
                output_vm.writePush(kind, class_symbols.indexOf(external_class_name) );
            else
                output_vm.writePush(kind, subroutine_symbols.indexOf(external_class_name) );
        }
        pushForwardToken(tokenizer);
        // '.'
        eatPreset(".");
        pushForwardToken(tokenizer);
        // subroutineName
        eat("", "subroutine_name", "subroutine_name");
        string external_subroutine_name = getTokenValue();
        pushForwardToken(tokenizer);
        // '('
        eatPreset("(");
        pushForwardToken(tokenizer);
        // expressionList
        int count_commas_in_expression_list = 0;
        compileExpressionList(tokenizer, count_commas_in_expression_list);
        pushForwardToken(tokenizer);
        // ')'
        eatPreset(")");

        if ( kind != "class_name")
        {
            if ( kind == "field" || kind == "static")
            {
                output_vm.writeCall(class_symbols.typeOf(external_class_name)  + "." +
                external_subroutine_name, count_commas_in_expression_list + 1);
            }
            else
            {
                output_vm.writeCall(subroutine_symbols.typeOf(external_class_name)  + "." +
                external_subroutine_name, count_commas_in_expression_list + 1);
            }
        }
        else if (kind == "class_name")
            output_vm.writeCall(external_class_name + "." + external_subroutine_name, count_commas_in_expression_list);
    }
}

#endif // COMPILATIONENGINE_CPP_INCLUDED
