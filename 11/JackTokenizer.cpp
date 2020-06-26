#ifndef JACKTOKENIZER_CPP_INCLUDED
#define JACKTOKENIZER_CPP_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> // copy, find, search
#include <cctype>    // isgraph, isdigit, isalnum

#include "JackTokenizer.h"

using std::ifstream;            using std::ofstream;
using std::vector;              using std::string;
using std::endl;                using std::copy;
using std::find;                using std::search;
using std::cout;

// static variables that cannot be defined in header file, only declared
const char JackTokenizer::symbols[] = "{}()[].,;+-*/&|<>=~";
const size_t symbols_size = sizeof(JackTokenizer::symbols) / sizeof(*JackTokenizer::symbols);

const string JackTokenizer::keywords[] = {"class", "constructor", "function", "method", "field", "static", "var", "int",
                           "char", "boolean", "bool", "void", "true", "false", "null", "this", "let",
                           "do", "if", "else", "while", "return"};
const size_t keywords_size = sizeof(JackTokenizer::keywords) / sizeof(*JackTokenizer::keywords);

// auxiliary functions external to class
vector<string> split(const string& s)
{
    vector<string> ret;
    string::size_type i = 0;

    // invariant: we have processed characters [original value of i, i)
    while (i != s.size())
    {
        // ignore leading blanks
        while ( i != s.size() && isspace(s[i]) )
            ++i;

        // find end of next word
        string::size_type j = i;

        // if current character is a Jack symbol
        const char* is_symbol = find(JackTokenizer::symbols, JackTokenizer::symbols + symbols_size, s[j]);
        if (i != s.size() && is_symbol != JackTokenizer::symbols + symbols_size)
        {
            ret.push_back( string(s, j, 1) );
            i++;
        }
        // else if it is quotations save characters until next quotation as 1 token
        else if (i != s.size() && s[j] == '"')
        {
            string::const_iterator open_quotation = s.begin() + j;
            string::const_iterator close_quotation = find(open_quotation + 1, s.end(), '"');
            ret.push_back( string(open_quotation, close_quotation + 1) );
            i += close_quotation - open_quotation + 1;
        }
        // else if we are not at endline, current character is not empty, and is no symbol(latter redundant)
        else
        {
            while (j != s.size() && !isspace(s[j]) &&
                   find(JackTokenizer::symbols, JackTokenizer::symbols + symbols_size, s[j]) ==
                   JackTokenizer::symbols + symbols_size)
                j++;
            // if we find some nonwhitespace characters
            if (i != j)
            {
                //copy from s starting at i and taking j-i chars
                string word;
                for (string::size_type index = i; index != j; ++index)
                {
                    string aux(1, s[index]);
                    word += aux;
                }
                ret.push_back(word);
                i = j;
            }
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
    const char comment[2] = {'/', '/'};
    string::iterator search_comment = search(line.begin(), line.end(), comment, comment + 2);

    if ( search_comment == line.begin() )
        return false;                        // comment spans whole line, so skip it altogether

    else if ( search_comment != line.end() ) // ...otherwise eliminate the comments:
    {
        string aux_line(line.size(), ' ');      // auxiliary string cannot be dynamically sized
        copy( line.begin(), search_comment, aux_line.begin() );   // copy line w.o. comments over auxiliary string
        string::reverse_iterator first_trailing_space = find_if(aux_line.rbegin(), aux_line.rend(), isgraph);
        aux_line.erase( first_trailing_space.base(), aux_line.end() );  // trim aux.string by erasing trailing spaces
        if ( !aux_line.empty() )
            line = aux_line;        // copy back results to current line
        else
            return false;           // if erasing comment somehow leaves string empty, skip altogether
    }

    return true;
}


// used by 'read()' to cancel API comment lines with asterisks
vector<string> cleanApiComments(const vector<string>& contents)
{
    const char comment_star[2] = {'/', '*'};
    const char end_comment_star[2] = {'*', '/'};

    // loop copies non-API commented lines into an auxiliary vector<string>
    vector<string> contents_aux;
    for (vector<string>::size_type i = 0; i != contents.size(); ++i)
    {
        string::const_iterator search_comment_star = search(contents[i].begin(), contents[i].end(),
                                                               comment_star, comment_star + 2);
        if ( search_comment_star == contents[i].end() )   // no begin-API comment (/*) found in line
            contents_aux.push_back(contents[i]);
        else        // in case line is an API comment:
        {
            for (vector<string>::size_type j = i; j != contents.size(); ++j)  // look for end-API comment(*/)
            {
                string::const_iterator search_end_comment_star = search(contents[j].begin(), contents[j].end(),
                                                                   end_comment_star, end_comment_star + 2);
                if ( search_end_comment_star != contents[j].end() )  // end-API comment is found, API lines are skipped
                {
                    i = j;
                    break;
                }
            }
        }
    }

    return contents_aux;
}


// constructor reading 'contents' directly from .jack file
JackTokenizer::JackTokenizer(const string& filename, const char* argv)
{
    string argument(argv);
    size_t found_jack = argument.find(".jack");

    if (found_jack == string::npos)
        read(argument + '\\' + filename + ".jack"); // reads from directory
    else
        read(filename + ".jack");   // from single .jack file

    counter_contents = 0;
}


// helps constructor above
void JackTokenizer::read(const string& filename)
{
    ifstream is(filename);
    string current_line;
    // get Jack code from input and clean white spaces and normal comments(//)
    while (is)
    {
        getline(is, current_line);
        if ( cleanLine(current_line) )
            contents.push_back(current_line);
    }

    // clean API comments (/* ... */) from 'contents'
    contents = cleanApiComments(contents);  // multi-line api comments, thus not included in cleanLine()
}


void JackTokenizer::splitInTokens()
{
    vector<string> contents_aux(0);
    ofstream tokenizer_xml_output;
    tokenizer_xml_output.open("squareInTokens.xml");
    for (vector<string>::size_type i = 0; i != contents.size(); ++i)
    {
        cout << contents[i] << endl;
        vector<string> splitted_line = split(contents[i]);
        for (vector<string>::size_type j = 0; j != splitted_line.size(); ++j)//{    // debugging
            contents_aux.push_back(splitted_line[j]);
            //cout << splitted_line[j] << endl;}        // debugging prints to cout
            //tokenizer_xml_output << splitted_line[j] << endl;}    // debugging alternative to above prints to file instead
    }

    contents = contents_aux;
    tokenizer_xml_output.close();
}


// Are there more tokens in the input?
bool JackTokenizer::hasMoreTokens()
{
    bool ret;

    counter_contents < size() ? ret = true : ret = false;

    return ret;
}


void JackTokenizer::advance()
{
    if ( counter_contents != getContents().size() )
    {
        current_item = getContents()[counter_contents];
        ++counter_contents;
    }
}


void JackTokenizer::stepBack()
{
    --counter_contents;
    current_item = getContents()[counter_contents];
}


void JackTokenizer::tokenType()
{
    const char* is_symbol = find(symbols, symbols + symbols_size, current_item[0]);
    const string* is_keyword = find(keywords, keywords + keywords_size, current_item);

    if (is_symbol != symbols + symbols_size)
    {
        token.type = "SYMBOL";     // assign token.type
        token.type_xml = "symbol"; // token type as printed in xml files
        symbol(is_symbol);         // assign symbol as token.value
    }
    else if ( isdigit(current_item[0]) )
    {
        token.type = "INT_CONST";
        token.type_xml = "integerConstant";
        intVal(current_item);
    }
    else if (current_item[0] == '"')
    {
        token.type = "STRING_CONST";
        token.type_xml = "stringConstant";
        strVal(current_item);
    }
    else if (is_keyword != keywords + keywords_size)
    {
        token.type = "KEYWORD";
        token.type_xml = "keyword";
        keyWord(current_item);
    }
    else if ( isalnum(current_item[0]) || (current_item[0] = '_') )
    {
        token.type = "IDENTIFIER";
        token.type_xml = "identifier";
        identifier(current_item);
    }
    else
        cout << "Tokenizer Error: " << current_item << " unrecognised token type" << endl;
}


void JackTokenizer::symbol(const char* is_symbol)
{
    if (*is_symbol == '<')
    {
        token.value = "&lt;";
    }
    else if (*is_symbol == '>')
        token.value = "&gt;";
    else if (*is_symbol == '\"')
        token.value = "&quot;";
    else if (*is_symbol == '&')
        token.value = "&amp;";
    else
        token.value = *is_symbol;
}


void JackTokenizer::intVal(const string& current)
{
    token.value = current;
}


void JackTokenizer::strVal(const string& current)
{
    string without_quotations(current.begin() + 1, current.end() - 1);
    token.value = without_quotations;
}


void JackTokenizer::keyWord(const string& current)
{
    token.value = current;
}


void JackTokenizer::identifier(const string& current)
{
    token.value = current;
}


ofstream JackTokenizer::openOutput(const string& filename)
{
    ofstream output(filename + ".xml");
    output << "<tokens>" << endl;

    return output;
}


void JackTokenizer::writeOnOutput(ofstream& output)
{
    output << "<" << getTokenTypeXml() << "> " << getTokenValue()
           << " </" << getTokenTypeXml() << ">" << endl;
}


void JackTokenizer::closeOutput(ofstream& output)
{
    output << "</tokens>" << endl;
    output.close();
}

#endif // JACKTOKENIZER_CPP_INCLUDED
