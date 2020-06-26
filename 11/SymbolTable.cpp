#ifndef SYMBOLTABLE_CPP_INCLUDED
#define SYMBOLTABLE_CPP_INCLUDED

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "SymbolTable.h"

using std::cout;            using std::endl;
using std::map;             using std::string;
using std::count;           using std::vector;

SymbolTable& SymbolTable::operator=(const SymbolTable& rhs)
{
    if (&rhs != this)
        symbols = rhs.symbols;

    return *this;
}


SymbolTable::SymbolTable(const SymbolTable& from)
{
    *this = from;
}


SymbolTable::~SymbolTable()
{
    symbols.clear();
    symbols.~map();
}


// resets the scope
void SymbolTable::startSubroutine()
{
    symbols.clear();
}


int SymbolTable::varCount(const string& wanted_kind)        // returns number of occurrences of 'kind' inside the symbol table
{
    vector<string> vect_kind;
    for (SymbolTable::iterator iter = begin(); iter != end(); ++iter)
    {
        vect_kind.push_back( (*iter).second.kind );
    }
    int count_kind = count(vect_kind.begin(), vect_kind.end(), wanted_kind);      // ->second.kind

    return count_kind;
}


void SymbolTable::define(const string& name, const string& type, const string& kind)
{

    if ( symbols.find(name) == symbols.end() )
    {
        Table values;

        int count_kind = varCount(kind); //count(symbols.begin()->second.kind, symbols.end()->second.kind, kind);
        values.entry_counter = count_kind;
        values.name = name;
        values.type = type;
        values.kind = kind;
        //++values.entry_counter;

        symbols.insert(make_pair(values.name, values));   // main symbol table
    }
}


string SymbolTable::kindOf(const string& name)           // return the kind of entry 'name'
{
    map<string, Table>::iterator find_kind = symbols.find(name);

    string ret;
    if (find_kind == symbols.end() )
        ret = "NONE";
    else
        ret = find_kind->second.kind;

    return ret;
}


string SymbolTable::typeOf(const string& name)           // returns entry type
{
    map<string, Table>::iterator find_type = symbols.find(name);

    string ret;
    if (find_type != symbols.end() )
        ret = find_type->second.type;
    else
    {
        string error(name + " does not have a type assigned");
        throw error;
    }

    return ret;
}


int SymbolTable::indexOf(const string& name)        // returns index number of entry
{
    map<string, Table>::iterator find_index = symbols.find(name);

    int ret;
    if (find_index != symbols.end() )
        ret = find_index->second.entry_counter;
    else
    {
        string error(name + " does not have a type assigned");
        throw error;
    }

    return ret;
}


SymbolTable::iterator SymbolTable::find(const string& key)
{
    return symbols.find(key);
}

#endif // SYMBOLTABLE_CPP_INCLUDED
