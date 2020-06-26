#ifndef SYMBOLTABLE_H_INCLUDED
#define SYMBOLTABLE_H_INCLUDED

#include <map>
#include <string>
#include <vector>

struct Table
{
    std::string name, type, kind;
    int entry_counter;

    Table(): entry_counter(0) {}
    Table(const std::string& name_ext, const std::string& type_ext, const std::string& kind_ext, int index_ext):
          name(name_ext), type(type_ext), kind(kind_ext), entry_counter(index_ext) {}
    ~Table(){}
};

class SymbolTable
{
public:
    SymbolTable(){}

    SymbolTable& operator=(const SymbolTable&);
    SymbolTable(const SymbolTable&);
    ~SymbolTable();

    void startSubroutine();
    void define(const std::string&, const std::string&, const std::string&);
    int varCount(const std::string&);
    std::string kindOf(const std::string&);
    std::string typeOf(const std::string&);
    int indexOf(const std::string&);
    std::map<std::string, Table> getSymbolTable() {return symbols;}

    // iterators
    typedef std::map<std::string, Table>::iterator iterator;
    iterator begin() {return symbols.begin();}
    iterator end() {return symbols.end();}
    typedef const std::map<std::string, Table>::const_iterator const_iterator;
    const_iterator begin() const {return symbols.begin();}
    const_iterator end() const {return symbols.end();}

    // find
    iterator find(const std::string&);

private:
    std::map<std::string, Table> symbols;   //class_symbols;
};

#include "SymbolTable.cpp"

#endif // SYMBOLTABLE_H_INCLUDED
