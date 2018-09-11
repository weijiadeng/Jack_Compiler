#pragma once


#include"JackTokenizer.h"
#include"SymbolTable.h"
#include"VMWriter.h"


//The Engine of the whole compiler
class CompilationEngine
{
    JackTokenizer tokens;
    SymbolTable stable;
    VMWriter vm;
    string className;
    void CompileClass(int & num);
    void CompileClassVarDec(int & num);
    void CompileSubroutine(int & num);
    void CompileSubroutineCall(int & num);
    void CompileParmaeterList(int & num);
    void CompileVarDec(int & num);
    void CompileStatements(int & num);
    void CompileDo(int & num);
    void CompileLet(int & num);
    void CompileWhile(int & num);
    void CompileReturn(int & num);
    void CompileIf(int & num);
    void CompileExpression(int & num);
    void CompileTerm(int & num);
    string typeConverter(int & num,string & tem);
    void CompileExpressionList(int & num, int & nVar, Kind dest=NOTKIND);
    void CompileSym(int & num, Token::Symbol  sym);
    //mod=0:
    void CompileIdentifier(int & num,string & str);
    void defineVarName(int & num, string const & type, Kind  kind);
    //input is command index
    bool isDefinedVarName(int & num);
    //input is command index
    void WritePushVarName(int & num);
    //input is command index
    void WritePopVarName(int & num);
    void CompileKeyword(int & num, Token::Keyword  key);
    void CompileKeyword(int & num, Token::Keyword  key,Kind & kd);
    void CompileSubroutineBody(int & num, string const & funcName);
    void CompileString(int & num);
    void CompileConstant(int & num);
    void CompileOp(int & num);
    void writeSubroutineCall(string const & name, int numVar);

    void advance(int & num);//skip comments
    void goBack(int & num);//skip comments

    //Deal with arithmetical operation priority
    int priority(int num);//return op priority
    bool isClassVarDec(int num);
    bool isSubroutineDec(int num);
    bool isType(int num);
    bool isStatement(int num);
    bool isExpression(int num);
    bool isOp(int num);

    Kind KindOf(int num);//input is command index
    string TypeOf(int num);
    int IndexOf(int num);

public:
    CompilationEngine(istream & is, ostream & os);
    ~CompilationEngine(){}
    void process(int num=0);
    Token & getToken(int num) { return tokens.Tokenlist[num]; }


};