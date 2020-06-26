#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <vector>
#include <string>

class Parser
{
    typedef std::vector<std::string>::size_type size_type;      // contents length
public:
    Parser(): counter(0) {}         // default constructor
    Parser(const std::string&, const char*);     // constructor reading 'contents' directly from .vm file

    // auxiliary and accessor functions
    void read(const std::string&);
    size_type size() {return contents.size();}
    std::vector<std::string> getContents() {return contents;}

    // Parser functions as described in chapter 7 API
    bool hasMoreCommands();
    void advance();
    void splitCommand();   // applies split() to current_command -> constructor of split_current_command
    std::string commandType();
    std::string arg1();
    int arg2();

private:
    std::vector<std::string> contents;      // .vm file contents to translate in assembly
    std::string current_command;            // current line in contents
    std::vector<std::string> split_current_command;     // current_command split in words by splitCommand
    size_type counter;                      // current line number
};


#include "Parser.cpp"

#endif // PARSER_H_INCLUDED
