#ifndef JACKTOKENIZER_H_INCLUDED
#define JACKTOKENIZER_H_INCLUDED

#include <fstream>
#include <vector>
#include <string>

struct Token
{
    Token(): type(""), type_xml(""), value("") {}

    Token& operator=(const Token& t)
    {
        if (&t != this)
        {
            type = t.type;
            type_xml = t.type_xml;
            value = t.value;
        }
        return *this;
    }

    Token(const Token& t)
    {
        *this = t;
    }

    ~Token()
    {
        type.std::string::~string();
        type.std::string::~string();
        value.std::string::~string();
    }
    std::string type, type_xml, value;
};

class JackTokenizer
{
public:
    const static char symbols[];
    const static std::string keywords[];
    typedef std::vector<std::string>::size_type size_type;      // contents length

    JackTokenizer(): counter_contents(0) {}
    JackTokenizer(const std::string&, const char*);

    void read(const std::string&);
    size_type size() {return contents.size();}
    std::vector<std::string> getContents() {return contents;}
    void advanceCounter() {++counter_contents;}
    std::string getTokenTypeXml() {return token.type_xml;}
    std::string getTokenValue() {return token.value;}
    Token getToken() {return token;}

    // Tokenizer functions as described in chapter 10 API
    bool hasMoreTokens();
    void advance();
    void stepBack();        // goes back to the previous token; needed in CompilationEngine
    void splitInTokens();   // applies split() to contents -> constructor of split_current_command
    void tokenType();
    void symbol(const char*);
    void intVal(const std::string&);
    void strVal(const std::string&);
    void keyWord(const std::string&);
    void identifier(const std::string&);

    std::ofstream openOutput(const std::string&);
    void writeOnOutput(std::ofstream&);
    void closeOutput(std::ofstream&);

private:
    std::vector<std::string> contents;
    size_type counter_contents;
    std::string current_item;
    Token token;
};

#include "JackTokenizer.cpp"

#endif // JACKTOKENIZER_H_INCLUDED
