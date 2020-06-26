#ifndef CODEWRITER_CPP_INCLUDED
#define CODEWRITER_CPP_INCLUDED

#include <iostream>
#include <string>
#include <algorithm>

#include "CodeWriter.h"

using std::string;          using std::endl;
using std::transform;

CodeWriter::CodeWriter(const string& filename, const char* argv)
{
    string argument(argv);
    size_t found_vm = argument.find(".vm");

    if (found_vm == string::npos) // from directory
    {
        string::size_type slash = argument.rfind("\\");   // look for last instance of '\' in the path of directory
        string folder_name( argument.substr(slash) );   // substring created from 'slash' to the end
        write(argument + '\\' + folder_name + ".asm");
    }
    else
        write(filename + ".asm");   // from file
}


void CodeWriter::write(const string& filename)
{
    output_file.open(filename); //, std::ofstream::out | std::ofstream::app);
}


void CodeWriter::writeArithmetic(const string& arg1)
{
    static int counter_true = 0;
    static int counter_end = 0;

    if (arg1 == "add")
        add();
    else if (arg1 == "sub")
        sub();
    else if (arg1 == "neg")
        neg();
    else if (arg1 == "eq")  // TRUE = -1; FALSE = 0
        eq(counter_true, counter_end);
    else if (arg1 == "gt")  // TRUE = -1; FALSE = 0
        gt(counter_true, counter_end);
    else if (arg1 == "lt")  // TRUE = -1; FALSE = 0
        lt(counter_true, counter_end);
    else if (arg1 == "and")
        and_b();
    else if (arg1 == "or")
        or_b();
    else if (arg1 == "not")
        not_b();
}


void CodeWriter::writePushPop(const string& c_push_pop, const string& segment, const int& index, const string& filename)
{
    if (segment == "constant")
        constant(index);
    else if (segment == "local")
    {
        if (c_push_pop == "C_PUSH")
            localPush(index);
        else if (c_push_pop == "C_POP")
            localPop(index);
    }
    else if (segment == "argument")
    {
        if (c_push_pop == "C_PUSH")
            argumentPush(index);
        else if (c_push_pop == "C_POP")
            argumentPop(index);
    }
    else if (segment == "this")
    {
        if (c_push_pop == "C_PUSH")
            thisPush(index);
        else if (c_push_pop == "C_POP")
            thisPop(index);
    }
    else if (segment == "that")
    {
        if (c_push_pop == "C_PUSH")
            thatPush(index);
        else if (c_push_pop == "C_POP")
            thatPop(index);
    }
    else if (segment == "static")
    {
        if (c_push_pop == "C_PUSH")
            staticPush(index, filename);
        else if (c_push_pop == "C_POP")
            staticPop(index, filename);
    }
    else if (segment == "temp")
    {
        if (c_push_pop == "C_PUSH")
            tempPush(index);
        else if (c_push_pop == "C_POP")
            tempPop(index);
    }
    else if (segment == "pointer")
    {
        if (c_push_pop == "C_PUSH")
            pointerPush(index);
        else if (c_push_pop == "C_POP")
            pointerPop(index);
    }
}


// auxiliary method to writeLabel, writeIf, writeGoto: turns label uppercase
string CodeWriter::labelUpper(const string& label, const string& function_name)
{
    string label_upper(label.size(), ' ');
    transform(label.begin(), label.end(), label_upper.begin(), toupper);

    if ( function_name.empty() )
        return label_upper;
    else
        return function_name + '$' + label_upper;
}


void CodeWriter::writeLabel(const string& label, const string& function_name = "")
{
    output_file << '(' << labelUpper(label, function_name) << ')' << endl;
}


void CodeWriter::writeGoto(const string& label, const string& function_name = "")
{
    output_file << "@" << labelUpper(label, function_name) << endl;
    output_file << "0; JMP" << endl;
}


void CodeWriter::writeIf(const string& label, const string& function_name = "")
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@" << labelUpper(label, function_name) << endl;
    output_file << "D; JLT" << endl;  // TRUE condition D = -1
}


void CodeWriter::writeFunction(const string& function_name, const int& num_locals)
{
    // not using 'writeLabel' function for this because it'd generate all-uppercase label
    output_file << '(' << function_name << ')' << endl;

    for (int i = 0; i != num_locals; ++i)   // PUSH constant 0; num_locals times
        constant(0);
}


void CodeWriter::writeReturn()
{
    output_file << "@LCL" << endl;  // endFrame[R14] = LCL
    output_file << "D = M" << endl;
    output_file << "@R14" << endl;
    output_file << "M = D" << endl;
    output_file << "@5" << endl;  // retAddr[R15] = *(endFrame[R14] - 5)
    output_file << "D = A" << endl;
    output_file << "@R14" << endl;
    output_file << "D = M - D" << endl;
    output_file << "A = D" << endl;
    output_file << "D = M" << endl;
    output_file << "@R15" << endl;
    output_file << "M = D" << endl;
    argumentPop(0);  // *ARG = pop
    output_file << "@ARG" << endl;  // SP = ARG + 1
    output_file << "D = M + 1" << endl;
    output_file << "@SP" << endl;
    output_file << "M = D" << endl;
    output_file << "@R14" << endl;  // THAT = *(endFrame[R14] - 1)
    output_file << "MD = M - 1" << endl;
    output_file << "A = D" << endl;
    output_file << "D = M" << endl;
    output_file << "@THAT" << endl;
    output_file << "M = D" << endl;
    output_file << "@R14" << endl;  // THIS = *(endFrame - 2)
    output_file << "MD = M - 1" << endl;
    output_file << "A = D" << endl;
    output_file << "D = M" << endl;
    output_file << "@THIS" << endl;
    output_file << "M = D" << endl;
    output_file << "@R14" << endl;  // ARG = *(endFrame - 3)
    output_file << "MD = M - 1" << endl;
    output_file << "A = D" << endl;
    output_file << "D = M" << endl;
    output_file << "@ARG" << endl;
    output_file << "M = D" << endl;
    output_file << "@R14" << endl;  // LCL = *(endFrame - 4)
    output_file << "MD = M - 1" << endl;
    output_file << "A = D" << endl;
    output_file << "D = M" << endl;
    output_file << "@LCL" << endl;
    output_file << "M = D" << endl;
    output_file << "@R15" << endl;
    output_file << "A = M" << endl;
    output_file << "0; JMP" << endl;
}


void CodeWriter::writeCall(const string& function_name, const int& num_args)
{
    static int counter_return = 0;
    string return_address( function_name + ".RETURN." + std::to_string(counter_return) );

    output_file << "@" << return_address << endl;  // push retAddrLabel
    output_file << "D = A" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
    output_file << "@LCL" << endl;  // push LCL
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
    output_file << "@ARG" << endl;  // push ARG
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
    output_file << "@THIS" << endl;  // push THIS
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
    output_file << "@THAT" << endl;  // push THAT
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
    output_file << "@5" << endl;  // ARG = SP - 5 - num_args
    output_file << "D = A" << endl;
    output_file << "@" << num_args << endl;
    output_file << "D = D + A" << endl;
    output_file << "@SP" << endl;
    output_file << "D = M - D" << endl;
    output_file << "@ARG" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;  // LCL = SP
    output_file << "D = M" << endl;
    output_file << "@LCL" << endl;
    output_file << "M = D" << endl;
    output_file << "@" << function_name << endl;  // goto function_name label of called function
    output_file << "0; JMP" << endl;
    output_file << '(' << return_address << ')' << endl;  // define retAddrLabel
    ++counter_return;
}


// auxiliaries to writeArithmetic
void CodeWriter::add()
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = M + D" << endl;
}


void CodeWriter::sub()
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = M - D" << endl;
}


void CodeWriter::neg()
{
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = -M" << endl;
}


void CodeWriter::eq(int& counter_true, int& counter_end)  // TRUE = -1; FALSE = 0
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "D = M - D" << endl;
    output_file << "@TRUE" << counter_true << endl;
    output_file << "D; JEQ" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = 0" << endl;
    output_file << "@END" << counter_end << endl;
    output_file << "0; JMP" << endl;
    output_file << "(TRUE" << counter_true << ")" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = -1" << endl;
    output_file << "(END" << counter_end << ")" << endl;
    counter_true++;
    counter_end++;
}


void CodeWriter::gt(int& counter_true, int& counter_end)  // TRUE = -1; FALSE = 0
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "D = M - D" << endl;
    output_file << "@TRUE" << counter_true << endl;
    output_file << "D; JGT" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = 0" << endl;
    output_file << "@END" << counter_end << endl;
    output_file << "0; JMP" << endl;
    output_file << "(TRUE" << counter_true << ")" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = -1" << endl;
    output_file << "(END" << counter_end << ")" << endl;
    counter_true++;
    counter_end++;
}


void CodeWriter::lt(int& counter_true, int& counter_end)  // TRUE = -1; FALSE = 0
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "D = M - D" << endl;
    output_file << "@TRUE" << counter_true << endl;
    output_file << "D; JLT" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = 0" << endl;
    output_file << "@END" << counter_end << endl;
    output_file << "0; JMP" << endl;
    output_file << "(TRUE" << counter_true << ")" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = -1" << endl;
    output_file << "(END" << counter_end << ")" << endl;
    counter_true++;
    counter_end++;
}


void CodeWriter::and_b()
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = M & D" << endl;
}


void CodeWriter::or_b()
{
    output_file << "@SP" << endl;
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = M | D" << endl;
}


void CodeWriter::not_b()
{
    output_file << "@SP" << endl;
    output_file << "A = M - 1" << endl;
    output_file << "M = !M" << endl;
}


// auxiliaries to writePushPop
void CodeWriter::constant(const int& index)
{
    output_file << "@" << index << endl;
    output_file << "D = A" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;
    output_file << "M = M + 1" << endl;
}


void CodeWriter::localPush(const int& index)
{
    output_file << "@" << index << endl; // addr = LCL + i
    output_file << "D = A" << endl;
    output_file << "@LCL" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = M" << endl;      // *SP = *addr
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::localPop(const int& index)
{
    output_file << "@" << index << endl; // addr = LCL + i
    output_file << "D = A" << endl;
    output_file << "@LCL" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::argumentPush(const int& index)
{
    output_file << "@" << index << endl; // addr = ARG + i
    output_file << "D = A" << endl;
    output_file << "@ARG" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = M" << endl;      // *SP = *addr
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::argumentPop(const int& index)
{
    output_file << "@" << index << endl; // addr = ARG + i
    output_file << "D = A" << endl;
    output_file << "@ARG" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::thisPush(const int& index)
{
    output_file << "@" << index << endl; // addr = THIS + i
    output_file << "D = A" << endl;
    output_file << "@THIS" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = M" << endl;      // *SP = *addr
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::thisPop(const int& index)
{
    output_file << "@" << index << endl; // addr = THIS + i
    output_file << "D = A" << endl;
    output_file << "@THIS" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::thatPush(const int& index)
{
    output_file << "@" << index << endl; // addr = THAT + i
    output_file << "D = A" << endl;
    output_file << "@THAT" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = M" << endl;      // *SP = *addr
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::thatPop(const int& index)
{
    output_file << "@" << index << endl; // addr = THAT + i
    output_file << "D = A" << endl;
    output_file << "@THAT" << endl;
    output_file << "A = M + D" << endl;
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::staticPush(const int& index, const string& filename)
{
    output_file << "@" << filename << '.' << index << endl; // addr = 16 + filename[0] + i
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;   // *SP = *addr
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::staticPop(const int& index, const string& filename)
{
    output_file << "@" << filename << '.' << index << endl; // addr = 16 + filename[0] + i
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::tempPush(const int& index)
{
    output_file << "@" << index << endl; // addr = 5 + i
    output_file << "D = A" << endl;
    output_file << "@5" << endl;
    output_file << "A = A + D" << endl;
    output_file << "D = M" << endl;      // *SP = *addr
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::tempPop(const int& index)
{
    output_file << "@" << index << endl; // addr = 5 + i
    output_file << "D = A" << endl;
    output_file << "@5" << endl;
    output_file << "A = A + D" << endl;
    output_file << "D = A" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // *addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::pointerPush(const int& index)
{
    output_file << "@" << index << endl; // addr = THIS + 0/1
    output_file << "D = A" << endl;
    output_file << "@THIS" << endl;
    output_file << "A = A + D" << endl;  // *SP = addr
    output_file << "D = M" << endl;
    output_file << "@SP" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP++
    output_file << "M = M + 1" << endl;
}


void CodeWriter::pointerPop(const int& index)
{
    output_file << "@" << index << endl; // addr = THIS + 0/1
    output_file << "D = A" << endl;
    output_file << "@THIS" << endl;
    output_file << "D = A + D" << endl;
    output_file << "@R13" << endl;
    output_file << "M = D" << endl;
    output_file << "@SP" << endl;        // SP--
    output_file << "AM = M - 1" << endl;
    output_file << "D = M" << endl;      // addr = *SP
    output_file << "@R13" << endl;
    output_file << "A = M" << endl;
    output_file << "M = D" << endl;
}


void CodeWriter::writeInit()
{
    output_file << "@256" << endl;  // SP = 256
    output_file << "D = A" << endl;
    output_file << "@SP" << endl;
    output_file << "M = D" << endl;
    writeCall("Sys.init", 0);  // call Sys.init;
}


void CodeWriter::close()
{
    if ( output_file.is_open() )
        output_file.close();
}

#endif // CODEWRITER_CPP_INCLUDED
