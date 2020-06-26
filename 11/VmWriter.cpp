#ifndef VMWRITER_CPP_INCLUDED
#define VMWRITER_CPP_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "VmWriter.h"

using std::cout;        using std::endl;
using std::vector;      using std::string;

VmWriter::VmWriter(const string& filename, const char* argv)
{
    string argument(argv);
    size_t found_jack = argument.find(".jack");

    if (found_jack == string::npos)                       // from directory
    {
        string::size_type slash = argument.rfind("\\");   // look for last instance of '\' in the path of directory
        string folder_name( argument.substr(slash) );     // substring created from 'slash' to the end
        output_vm.open(argument + '\\' + filename + ".vm");
    }
    else
        output_vm.open(filename + ".vm");                 // from file
}


void VmWriter::writePush(const std::string& segment, int index)
{
    if (segment == "CONST")
        output_vm << "push constant " << index << endl;
    else if (segment == "ARG")
        output_vm << "push argument " << index << endl;
    else if (segment == "VAR"  || segment == "local"  || segment == "LOCAL")
        output_vm << "push local " << index << endl;
    else if (segment == "STATIC" || segment == "static")
        output_vm << "push static " << index << endl;
    else if (segment == "FIELD" || segment == "field")
        output_vm << "push this " << index << endl;
    else if (segment == "THAT"  || segment == "that")
        output_vm << "push that " << index << endl;
    else if (segment == "POINTER" || segment == "pointer")
        output_vm << "push pointer " << index << endl;
    else if (segment == "TEMP"  || segment == "temp")
        output_vm << "push temp " << index << endl;
    else
        throw "incorrect segment push definition";
}


void VmWriter::writePop(const std::string& segment, int index)
{
    if (segment == "CONST")
        output_vm << "pop constant " << index << endl;
    else if (segment == "ARG")
        output_vm << "pop argument " << index << endl;
    else if (segment == "VAR"  || segment == "local"  || segment == "LOCAL")
        output_vm << "pop local " << index << endl;
    else if (segment == "STATIC" || segment == "static")
        output_vm << "pop static " << index << endl;
    else if (segment == "FIELD" || segment == "field")
        output_vm << "pop this " << index << endl;
    else if (segment == "THAT" || segment == "that")
        output_vm << "pop that " << index << endl;
    else if (segment == "POINTER" || segment == "pointer")
        output_vm << "pop pointer " << index << endl;
    else if (segment == "TEMP" || segment == "temp")
        output_vm << "pop temp " << index << endl;
    else
        throw "incorrect segment pop definition";
}


// ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
void VmWriter::writeArithmetic(const string& command)
{
    if (command == "+" || command == "add" || command == "ADD")
        output_vm << "add" << endl;
    else if (command == "-" || command == "sub" || command == "SUB")
        output_vm << "sub" << endl;
    else if (command == "-NEG" || command == "-neg")
        output_vm << "neg" << endl;
    else if (command == "=" || command == "eq" || command == "EQ")
        output_vm << "eq" << endl;
    else if (command == "&gt;")
        output_vm << "gt" << endl;
    else if (command == "&lt;")
        output_vm << "lt" << endl;
    else if (command == "&amp;")
        output_vm << "and" << endl;
    else if (command == "|")
        output_vm << "or" << endl;
    else if (command == "~" || command == "NOT" || command == "not")
        output_vm << "not" << endl;
    else
        throw "incorrect arithmetic command definition";
}


void VmWriter::writeLabel(const string& label)    // index on label will be appended by caller
{
    output_vm << "label " << label << endl;       // remove toupper() as label imported in uppercase
}


void VmWriter::writeGoto(const string& destination)
{
    output_vm << "goto " << destination << endl;
}


void VmWriter::writeIf(const string& destination)
{
    output_vm << "if-goto " << destination << endl;
}


void VmWriter::writeCall(const string& name, int n_args)
{
    output_vm << "call " << name << " " << n_args << endl;
}


void VmWriter::writeFunction(const string& name, int n_locals)
{
    output_vm << "function " << name << " " << n_locals << endl;
}


void VmWriter::writeReturn()
{
    output_vm << "return" << endl;
}


void VmWriter::close()
{
    output_vm.close();
}

#endif // VMWRITER_CPP_INCLUDED
