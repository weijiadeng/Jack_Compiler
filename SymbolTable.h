//
// Created by dwj on 9/29/16.
//

///Symbol Table helps to parse the Jack file.
#pragma once
#include"stdafx.h"
#include"JackTokenizer.h"

enum Kind
{
    NOTKIND,STATIC,FIELD,VAR,ARG,CLASS,CONSTRUCTOR,FUNCTION,METHOD
};


//Convert enum value to string value.
string TypeConverter(Token const & token);

//Information of each variable.
struct Varinfo
{
    //Index of the variable, namely the ordinal number of the variable
    int index;
    //Type of the variable. For example, int, char and so on are types.
    string type;
    //Kind of the variable. For example, static and field are types.
    Kind kind;
    Varinfo(int num = 0, string const & word = "", Kind kd=NOTKIND):index(num),type(word),kind(kd){}
};

class SymbolTable
{
    //Class-wise variable information
    unordered_map<string, Varinfo> classScope;
    //Block-wise variable information
    unordered_map<string, Varinfo> subScope;


public:
    SymbolTable():numStat(0),numField(0),numArg(0),numVar(0){}
    ~SymbolTable(){}
    //Start the analyse of sub block
    void startSubroutine();

    //Add the index of sub block
    void addSubScopeIndex();

    //Analyse token and add it to corresponding list.
    void Define(Token const & token, string const & type, Kind kind);

    //Return the number of variables in corresponding kind.
    int VarCount(Kind kind);

    //Return the kind of certain variable
    Kind KindOf(string const & str);

    //Return the type of certain variable
    string TypeOf(string const & str);

    //Return the index of certain variable
    int IndexOf(string const & str);

    //Return the kind of certain token
    Kind KindOf(Token & kind);

    //Return the type of certain token
    string TypeOf(Token & kind);

    //Return the index of certain token
    int IndexOf(Token & kind);
    int numStat;
    int numField;
    int numArg;
    int numVar;

};
