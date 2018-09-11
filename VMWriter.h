//
// Created by dwj on 9/29/17.
//

#pragma once
#include"JackTokenizer.h"
#include"SymbolTable.h"

//The class that writes the vm code.
class VMWriter
{
    //Class name of current handling class
    string className;
    ostream & os;
public:
    VMWriter(ostream & oss) :os(oss) {}

    //Change class name
    void setClassName(string const & text) { className = text; }
    ~VMWriter();

    //The following function write different kinds of commands.
    void writePush(string const & seg, int idx);
    void writePop(string const & seg, int idx);
    void writeArth(Token::Symbol command);
    void writeLabel(string const & label);
    void writeGoto(string const & label);
    void writeIf(string const & label);
    void writeCall(string const & name,int nArg);
    void writeFunction(string const & label, int nArg);
    void writeComment(string const & comment);
    void writeReturn();


};
