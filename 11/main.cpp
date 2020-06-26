/**                                                      **
*               Jack language compiler.                   *
*   Reads .jack input files and outputs .xml files and    *
*   more importantly, .vm files that can be processed     *
*   by the virtual machine built in chap.8.               *
*   Reads either (a) from directory   (b) from sequence   *
*   of files inserted as command line arguments           *
**                                                      **/
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

#include "JackTokenizer.h"
#include "SyntaxAnalyser.h"
#include "CompilationEngine.h"
#include "SymbolTable.h"

using std::string;          using std::vector;
using std::cout;            using std::endl;
using std::find;

int main(int argc, char **argv)
{
    // preliminary: there has to be at least one command line argument
    if (argc < 2)
    {
        cout << "usage: jack files <dir>\n";

        return -1;
    }

    // record argument filenames in a vector of strings
    vector<string> filename_list;

    // search for points in the program arguments' names to determine if files or directory
    vector<char*> find_point;
    for (int i = 1; i != argc; ++i)
        find_point.push_back( find(argv[i], argv[i] + strlen(argv[i]), '.') );

    // if there is no point '.' inside first argument filename, it must be a directory
    if ( find_point[0] == argv[1] + strlen(argv[1]) )
    {
        string path;
        HANDLE hFind;            // windows.h stuff
        WIN32_FIND_DATA data;    // windows.h stuff

        cout << "Jack files in " << argv[1] << ": \n";
        path = argv[1];
        path = path + "\\*.jack";

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
    else   // else a point inside argument name: the command line arguments are a sequence of '.jack' files
    {
        for (int i = 1; i != argc; ++i)
        {
            char* filename_char = new char[30];
            strncpy(filename_char, argv[i], find_point[i - 1] - argv[i]);//(destination, source, numbr of chars to be copied)
            filename_char[find_point[i - 1] - argv[i]] = '\0';  //manually add null as strncpy does not do it
            filename_list.push_back(filename_char);
            delete[] filename_char;
        }
    }

    // for each .jack input file construct and populate a JackTokenizer object for the SyntaxAnalyser
    // and a JackTokenizer for the CompilationEngine that will generate the .vm output file
    for (vector<string>::size_type i = 0; i != filename_list.size(); ++i)
    {
        JackTokenizer jack_input(filename_list[i], argv[1]);
        CompilationEngine vm_output(filename_list[i], argv[1]);
        JackTokenizer jack_input_for_xml(filename_list[i], argv[1]);
        SyntaxAnalyser xml_output(filename_list[i], argv[1]);

        jack_input.splitInTokens();
        jack_input_for_xml.splitInTokens();

        // extract needed info from JackTokenizer private data
        while ( jack_input.hasMoreTokens() )
        {
            vm_output.importToken( jack_input.getToken() );
            vm_output.compileClass(jack_input, filename_list[i], argv[1]);
            xml_output.importToken( jack_input_for_xml.getToken() );
            xml_output.analyseClass(jack_input_for_xml);
        }
    }

    return 0;
}
