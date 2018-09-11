//
// Created by dwj on 9/29/17.
//

///Parse the Jack file into an ordered list of Tokens.
#ifndef JACKANALYZER_JACKTOKENIZER_H
#define JACKANALYZER_JACKTOKENIZER_H

#include "stdafx.h"

class error_string_not_end
{};

//Trim the string
string & trim(string & s);

string IntToString(int num);

int StringToInt(string const & string);

int find_token_end(const string & line, int startPos);

//Description of a token
struct Token
{
    enum Type{error,keyword,symbol,constant,_string,identifier,comment};
    enum Keyword{not_keyword,_class,_constructor,_function,_method,_field,_static,_var,_int,_char,_boolean,_void,_true,_false,_null,_this,_let,_do,_if,_else,_while,_return,
    };
    enum Symbol{not_symbol,leftBrace, rightBrace, leftParenthese, rightParenthese, leftSquareBrace, rightSquareBrace, dot, period, semicolon, plus, minus, multi, divide, slant, _and, _or, lesser, greater, equal, _not,neg
    };
    int num;
    string text;
    Type gentype;
    Keyword  key;
    Symbol sym;
    Token():num(0),gentype(error),key(not_keyword),sym(not_symbol){text="";}
};

//The class that parse text into token
class JackTokenizer
{
private:
    //Original code
    vector<string> code;
    //Original comment
    vector<string> comment;
    //All the string const variables.
    vector<string> strings;

    //The list of all possible keywords
    unordered_map<string, Token::Keyword > keyWordList;
    //The list of all possible symbols
    unordered_map<char, Token::Symbol> symbolList;

    //Extract the beginning part of a comment
    void extractStartComment(string &word, int startPos, int &status);

    //Extract the ending part of a comment
    void extractEndComment(string &word, int startPos, int &status);

    //Initialize the JackTokenizer class. Fill the key word list and
    //symbol list with pre-defined values.
    void initialize();

    //Convert comments into token and extract them from input
    void convertComments();

    //The following functions process corresponding kinds of texts.
    void processLine(string const & line,int startPos);
    void processDigit(string const & line, int startPos);
    void processAlpha(string const & line, int startPos);
    void processPunct(string const & line, int startPos);
    void processComment(string const & line, int startPos);
    void processString(string const & line, int startPos);



public:
    vector<Token> Tokenlist;
    explicit JackTokenizer(istream & is);

    //Iterate through all lines and convert each line into tokens.
    void process();

};



#endif //JACKANALYZER_JACKTOKENIZER_H