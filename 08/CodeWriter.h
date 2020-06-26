#ifndef CODEWRITER_H_INCLUDED
#define CODEWRITER_H_INCLUDED

#include <string>
#include <fstream>

class CodeWriter
{
public:
    CodeWriter(){}
    CodeWriter(const std::string&, const char*);

    void write(const std::string&);
    void writeArithmetic(const std::string&);
        // auxiliaries to writeArithmetic
        void add();
        void sub();
        void neg();
        void eq(int&, int&);
        void gt(int&, int&);
        void lt(int&, int&);
        void and_b();
        void or_b();
        void not_b();
    void writePushPop(const std::string&, const std::string&, const int&, const std::string&);
        // auxiliaries to writePushPop
        void constant(const int&);
        void localPush(const int&);
        void localPop(const int&);
        void argumentPush(const int&);
        void argumentPop(const int&);
        void thisPush(const int&);
        void thisPop(const int&);
        void thatPush(const int&);
        void thatPop(const int&);
        void staticPush(const int&, const std::string&);
        void staticPop(const int&, const std::string&);
        void tempPush(const int&);
        void tempPop(const int&);
        void pointerPush(const int&);
        void pointerPop(const int&);
    void writeLabel(const std::string&, const std::string&);
        std::string labelUpper(const std::string&, const std::string&);
    void writeGoto(const std::string&, const std::string&);
    void writeIf(const std::string&, const std::string&);
    void writeFunction(const std::string&, const int&);
    void writeReturn();
    void writeCall(const std::string&, const int&);

    void writeInit();
    void close();



private:
    std::ofstream output_file;
};


#include "CodeWriter.cpp"

#endif // CODEWRITER_H_INCLUDED
