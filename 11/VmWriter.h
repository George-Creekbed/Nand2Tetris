#ifndef VMWRITER_H_INCLUDED
#define VMWRITER_H_INCLUDED

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class VmWriter
{
public:
    VmWriter(){}
    VmWriter(const std::string&, const char*);

    void writePush(const std::string&, int);
    void writePop(const std::string&, int);
    void writeArithmetic(const std::string&);
    void writeLabel(const std::string&);
    void writeGoto(const std::string&);
    void writeIf(const std::string&);
    void writeCall(const std::string&, int);
    void writeFunction(const std::string&, int);
    void writeReturn();
    void close();

private:
    std::ofstream output_vm;
};

#include "VmWriter.cpp"

#endif // VMWRITER_H_INCLUDED
