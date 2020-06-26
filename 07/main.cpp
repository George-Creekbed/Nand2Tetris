// reads .vm input files and outputs one coral .asm file
// reads either (a) from directory   (b) from sequence of files
// given as command line arguments
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <windows.h>

#include "Parser.h"
#include "CodeWriter.h"

using std::string;          using std::vector;
using std::cout;            using std::endl;
using std::find;            using std::strlen;

int main(int argc, char **argv)
{
    // preliminary: there has to be at least one command line argument
    if (argc < 2)
    {
        cout << "usage: vmfiles <dir>\n";

        return -1;
    }

    // record argument filenames in a vector of strings
    vector<string> filename_list;

    // if there is no point '.' inside first argument filename, it must be a directory
    vector<char*> find_point;
    for (int i = 1; i != argc; ++i)
    {
        find_point.push_back( find(argv[i], argv[i] + strlen(argv[i]), '.') );
    }
    //char* find_point = find(argv[1], argv[1] + strlen(argv[1]), '.'); //
    if ( find_point[0] == argv[1] + strlen(argv[1]) )
    {
        string path;
        HANDLE hFind;   // windows.h stuff
        WIN32_FIND_DATA data;    // windows.h stuff

        cout << "VM files in " << argv[1] << ": \n";
        path = argv[1];
        path = path + "\\*.vm";

        hFind = FindFirstFile(path.c_str(), &data);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                cout << data.cFileName << endl;
                string filename(data.cFileName);
                size_t point = filename.find(".");
                filename_list.push_back( filename.substr(0, point) );
            } while ( FindNextFile(hFind, &data) );
            FindClose(hFind);
        }
    }
    else   // else the command line arguments are a sequence of '.vm' files
    {
        for (int i = 1; i != argc; ++i)
        {
            char* filename_char = new char[30];
            strncpy(filename_char, argv[i], find_point[i - 1] - argv[i]);
            filename_list.push_back(filename_char);
            delete[] filename_char;
        }
    }

    // construct a CodeWriter object, that will treat all input files, named as the first input file
    CodeWriter asm_output(filename_list[0], argv[1]);

    // for each .vm input file construct and populate a Parser object
    for (vector<string>::size_type i = 0; i != filename_list.size(); ++i)
    {
        // construct the Parser objects, and check their contents
        Parser vm_input(filename_list[i], argv[1]);

        // extract needed info from Parser private data
        while ( vm_input.hasMoreCommands() )
        {
            vm_input.advance();
            vm_input.splitCommand();
            // cout << "command type: " << vm_input.commandType() << endl;    // debugging
            if (vm_input.commandType() != "C_RETURN")
            {
                // cout << "arg1: " << vm_input.arg1() << endl;    // debugging

                if ( (vm_input.commandType() == "C_PUSH") || (vm_input.commandType() == "C_POP") ||
                     (vm_input.commandType() == "C_FUNCTION") || (vm_input.commandType() == "C_CALL") )
                    {
                         // cout << "arg2: " << vm_input.arg2() << endl;    // debugging
                         asm_output.writePushPop( vm_input.commandType(), vm_input.arg1(),
                                                  vm_input.arg2(), filename_list[i]       );
                    }
                else if (vm_input.commandType() == "C_ARITHMETIC")
                    asm_output.writeArithmetic( vm_input.arg1() );
            }
        }
    }

    asm_output.close();

    return 0;
}
