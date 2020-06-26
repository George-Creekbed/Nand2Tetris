#ifndef PARSER_CPP_INCLUDED
#define PARSER_CPP_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>  // i(o)stream_iterator<string>
#include <algorithm> // copy, find
#include <cctype>    // isgraph

#include "Parser.h"

using std::ifstream;            using std::ofstream;
using std::vector;              using std::string;
using std::cin;                 using std::cout;
using std::endl;                using std::copy;
using std::find;                using std::ostream_iterator;


// auxiliary functions external to class Parser
vector<string> split(const string& s)
{
    vector<string> ret;
    typedef string::size_type string_size;
    string_size i = 0;

    // invariant: we have processed characters [original value of i, i)
    while (i != s.size())
    {
        // ignore leading blanks
        // invariant: characters in range [original i, current i) are all spaces
        while ( i != s.size() && isspace(s[i]) )
            ++i;

        // find end of next word
        string_size j = i;
        // invariant: none of the characters in range [original j, current j) is a space
        while ( j != s.size() && !isspace(s[j]) )
            j++;
        // if we found some nonwhitespace characters
        if (i != j)
        {
            //copy from s starting at i and taking j-i chars
            //ret.push_back(s.substring(i, j-i));
            string word;
            for (string_size index = i; index != j; ++index)
            {
                string aux(1, s[index]);
                word += aux;
            }

            ret.push_back(word);
            i = j;
        }
    }

    return ret;
}


// used by read() to eliminate comments or empty lines
bool cleanLine(string& line)
{
    // first check whether the current line is totally empty
    bool ok = false;

    for (string::const_iterator iter = line.begin(); iter != line.end(); ++iter)
    {
        if ( isgraph(*iter) )  // isgraph identifies any char that is not a space
            ok = true;
    }

    if (line.empty() || ok == false)
        return false;

    // Now check if the whole line or part of it is a comment; eliminate the comment
    char comment[2] = {'/', '/'};
    string::iterator search_comment = search(line.begin(), line.end(), comment, comment + 1);
    string aux_line;

    if ( search_comment == line.begin() )  // comment spans whole line, so skip it altogether
        return false;
    else if ( search_comment != line.end() )
    {
        copy( line.begin(), search_comment, aux_line.begin() );
        line = aux_line;
    }

    return true;
}


// constructor reading 'contents' directly from .vm file
Parser::Parser(const string& filename, const char* argv)
{
    string argument(argv);
    size_t found_vm = argument.find(".vm");

    if (found_vm == string::npos)
        read(argument + '\\' + filename + ".vm");         // reads from directory
    else
        read(filename + ".vm");   // from single .vm file

    counter = 0;

//    // eliminate this is for debugging purposes
//    ofstream os("output.txt");
//    copy( contents.begin(), contents.end(), ostream_iterator<string>(os,"\n") );
}


// helps constructor above
void Parser::read(const string& filename)
{
    ifstream is(filename);
    string current_line;

    while (is)
    {
        getline(is, current_line);
        if ( cleanLine(current_line) )
            contents.push_back(current_line);
    }
}


// Are there more commands in the input?
bool Parser::hasMoreCommands()
{
    bool ret;

    counter < size() ? ret = true : ret = false;

    return ret;
}


// Reads next command from input
void Parser::advance()
{
    current_command = getContents()[counter];
    ++counter;
}


// adjusts current_command and then splits it
void Parser::splitCommand()
{
    // convert current_command in lowercase if is not such already
    transform(current_command.begin(), current_command.end(), current_command.begin(), tolower);

    split_current_command = split(current_command);
}


// Determines the type of vm command
string Parser::commandType()
{
    if (split_current_command.size() == 1)
        return "C_ARITHMETIC";
    else if (split_current_command[0] == "push")
        return "C_PUSH";
    else if (split_current_command[0] == "pop")
        return "C_POP";
    else if (split_current_command[0] == "label")
        return "C_LABEL";
    else if (split_current_command[0] == "goto")
        return "C_GOTO";
    else if (split_current_command[0] == "if-goto")
        return "C_IF";
    else if (split_current_command[0] == "function")
        return "C_FUNCTION";
    else if (split_current_command[0] == "call")
        return "C_CALL";
    else if (split_current_command[0] == "return")
        return "C_RETURN";

    return 0;
}


// yields first argument of command if not arithmetic
string Parser::arg1()
{
    if (split_current_command.size() == 1)
        return split_current_command[0];
    else
        return split_current_command[1];
}


// yields second argument of command
int Parser::arg2()
{
    return stoi(split_current_command[2]);
}

#endif // PARSER_CPP_INCLUDED
