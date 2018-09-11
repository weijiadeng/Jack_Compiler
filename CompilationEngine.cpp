
#include "stdafx.h"
#include "CompilationEngine.h"



inline bool CompilationEngine::isSubroutineDec(int num)
{
    return tokens.Tokenlist[num].key == Token::_constructor ||
    tokens.Tokenlist[num].key == Token::_function ||
    tokens.Tokenlist[num].key == Token::_method;
}

inline bool CompilationEngine::isType(int num)
{
    return tokens.Tokenlist[num].key == Token::_int ||
           tokens.Tokenlist[num].key == Token::_char ||
           tokens.Tokenlist[num].key == Token::_boolean ||
           tokens.Tokenlist[num].gentype == Token::identifier;
}

inline bool CompilationEngine::isStatement(int num)
{
    return tokens.Tokenlist[num].key == Token::_let ||
           tokens.Tokenlist[num].key == Token::_if ||
           tokens.Tokenlist[num].key == Token::_while ||
           tokens.Tokenlist[num].key == Token::_do ||
           tokens.Tokenlist[num].key == Token::_return;
}

inline bool CompilationEngine::isExpression(int num)
{
    switch (tokens.Tokenlist[num].gentype)
    {
        case Token::_string:
        case Token::constant:
        case Token::keyword:
        case Token::identifier:
            return true;
        case Token::symbol:
            return tokens.Tokenlist[num].sym == Token::_not ||
                   tokens.Tokenlist[num].sym == Token::minus ||
                   tokens.Tokenlist[num].sym == Token::leftParenthese;
            break;
        default:
            return false;
            break;
    }

}

inline bool CompilationEngine::isOp(int num)
{
    switch (tokens.Tokenlist[num].sym)
    {
        case Token::plus:
        case Token::minus:
        case Token::multi:
        case Token::divide:
        case Token::_and:
        case Token::_or:
        case Token::lesser:
        case Token::greater:
        case Token::equal:
            return true;
            break;
        default:
            return false;
            break;
    }
}

inline Kind CompilationEngine::KindOf(int num) { return stable.KindOf(getToken(num)); }

inline string CompilationEngine::TypeOf(int num) { return stable.TypeOf(getToken(num)); }

inline int CompilationEngine::IndexOf(int num) { return stable.IndexOf(getToken(num)); }

void CompilationEngine::process(int num)
{
    if(getToken(num).gentype==Token::comment)
        advance(num);
    CompileClass(num);
    advance(num);
}
void CompilationEngine::CompileClass(int & num)
{
    CompileKeyword(num, Token::_class);
    className = getToken(num).text;
    defineVarName(num, getToken(num).text, Kind::CLASS);
    CompileSym(num, Token::leftBrace);
    while (isClassVarDec(num))
    {
        CompileClassVarDec(num);
    }
    while (isSubroutineDec(num))
    {
        CompileSubroutine(num);
    }
    CompileSym(num, Token::rightBrace);
}

void CompilationEngine::CompileClassVarDec(int & num)
{
    Kind varKind;
    if (getToken(num).key == Token::_field)
        CompileKeyword(num, Token::_field,varKind);
    else if (getToken(num).key == Token::_static)
        CompileKeyword(num, Token::_static,varKind);
    string type;
    typeConverter(num,type);
    defineVarName(num,type,varKind);
    while (getToken(num).sym==Token::dot)
    {
        CompileSym(num, Token::dot);
        defineVarName(num, type, varKind);
    }
    CompileSym(num, Token::semicolon);
}

void CompilationEngine::CompileSubroutine(int & num)
{
    Kind kd;
    if (getToken(num).key == Token::_function)
        CompileKeyword(num, Token::_function,kd);
    else if (getToken(num).key == Token::_method)
        CompileKeyword(num, Token::_method, kd);
    else if (getToken(num).key == Token::_constructor)
        CompileKeyword(num, Token::_constructor, kd);
    string type;
    typeConverter(num, type);
    string funcName;
    typeConverter(num, funcName);
    goBack(num);
    defineVarName(num, type, kd);
    CompileSym(num, Token::leftParenthese);
    CompileParmaeterList(num);
    CompileSym(num, Token::rightParenthese);
    CompileSubroutineBody(num, funcName);
}

void CompilationEngine::CompileSubroutineCall(int & num)
{
    string name;
    CompileIdentifier(num,name);

    if (getToken(num).sym == Token::leftParenthese)
    {
        string fullname = className + "." + name;
        CompileSym(num, Token::leftParenthese);
        int nVar = 0;
        vm.writePush("pointer", 0);
        CompileExpressionList(num, nVar, stable.KindOf(name));
        nVar++;
        writeSubroutineCall(fullname, nVar);
        CompileSym(num, Token::rightParenthese);
    }
    else if (getToken(num).sym == Token::period)
    {
        CompileSym(num, Token::period);
        string aftername;
        CompileIdentifier(num, aftername);
        string fullName ;
        CompileSym(num, Token::leftParenthese);
        int nVar = 0;
        switch (stable.KindOf(name))
        {
            case STATIC:
                vm.writePush("static", stable.IndexOf(name));
                break;
            case FIELD:
                vm.writePush("this", stable.IndexOf(name));
                break;
            case ARG:
                vm.writePush("argument", stable.IndexOf(name));
                break;
            case VAR:
                vm.writePush("local", stable.IndexOf(name));
                break;
            default:
                break;
        }

        CompileExpressionList(num, nVar, METHOD);

        if (stable.KindOf(name) == STATIC || stable.KindOf(name) == FIELD || stable.KindOf(name) == ARG || stable.KindOf(name) == VAR)
        {
            fullName = stable.TypeOf(name) + '.' + aftername;
            nVar++;
        }
        else
            fullName = name + '.' + aftername;
        writeSubroutineCall(fullName, nVar);
        CompileSym(num, Token::rightParenthese);
    }
}

void CompilationEngine::CompileParmaeterList(int & num)
{
    stable.startSubroutine();
    if (isType(num))
    {
        string type;
        typeConverter(num, type);
        defineVarName(num, type, ARG);
        while (getToken(num).sym == Token::dot)
        {
            CompileSym(num, Token::dot);
            typeConverter(num, type);
            defineVarName(num, type, ARG);
        }
    }
}

void CompilationEngine::CompileVarDec(int & num)
{
    CompileKeyword(num, Token::_var);
    string type;
    typeConverter(num, type);
    defineVarName(num, type, VAR);
    while (getToken(num).sym==Token::dot)
    {
        CompileSym(num, Token::dot);
        defineVarName(num, type, VAR);
    }
    CompileSym(num, Token::semicolon);
}

void CompilationEngine::CompileStatements(int & num)
{
    while (isStatement(num))
    {
        switch (getToken(num).key)
        {
            case Token::_let:
                CompileLet(num);
                break;
            case Token::_if:
                CompileIf(num);
                break;
            case Token::_while:
                CompileWhile(num);
                break;
            case Token::_do:
                CompileDo(num);
                break;
            case Token::_return:
                CompileReturn(num);
                break;
            default:
                break;
        }
    }
}

void CompilationEngine::CompileDo(int & num)
{
    CompileKeyword(num, Token::_do);
    CompileSubroutineCall(num);
    vm.writePop("temp", 0);
    CompileSym(num, Token::semicolon);
}

void CompilationEngine::CompileLet(int & num)
{
    CompileKeyword(num, Token::_let);
    string nul;
    int varNum = num;
    CompileIdentifier(num,nul);
    if (getToken(num).sym == Token::leftSquareBrace)
    {
        WritePushVarName(varNum);
        CompileSym(num, Token::leftSquareBrace);
        CompileExpression(num);
        CompileSym(num, Token::rightSquareBrace);
        vm.writeArth(Token::plus);
        if (getToken(num).sym == Token::equal)
            advance(num);
        else
            throw error_string_not_end();
        CompileExpression(num);
        vm.writePop("temp", 0);
        vm.writePop("pointer", 1);
        vm.writePush("temp", 0);
        vm.writePop("that", 0);
        CompileSym(num, Token::semicolon);
    }
    else
    {
        if (getToken(num).sym == Token::equal)
            advance(num);
        else
            throw error_string_not_end();
        CompileExpression(num);
        WritePopVarName(varNum);
        CompileSym(num, Token::semicolon);
    }

}

void CompilationEngine::CompileWhile(int & num)
{
    CompileKeyword(num, Token::_while);
    string no;
    no = IntToString(num);    //No. of while loop
    vm.writeLabel("MAIN_LOOP_START."+ no + '.' + className);
    CompileSym(num, Token::leftParenthese);
    CompileExpression(num);
    vm.writeIf("COMPUTE." + no + '.' + className);
    vm.writeGoto("LOOP_END." + no + '.' + className);
    vm.writeLabel("COMPUTE." + no + '.' + className);
    CompileSym(num, Token::rightParenthese);
    CompileSym(num, Token::leftBrace);
    CompileStatements(num);
    CompileSym(num, Token::rightBrace);
    vm.writeGoto("MAIN_LOOP_START." + no + '.' + className);
    vm.writeLabel("LOOP_END." + no + '.' + className);

}

void CompilationEngine::CompileReturn(int & num)
{
    CompileKeyword(num, Token::_return);
    if (isExpression(num))
        CompileExpression(num);
    else
        vm.writePush("constant", 0);
    CompileSym(num, Token::semicolon);
    vm.writeReturn();
}

void CompilationEngine::CompileIf(int & num)
{
    string ifNo = IntToString(num);
    CompileKeyword(num, Token::_if);
    CompileSym(num, Token::leftParenthese);
    CompileExpression(num);
    CompileSym(num, Token::rightParenthese);
    vm.writeIf("IF_GOTO_TRUE." + ifNo + "." + className);
    vm.writeGoto("IF_GOTO_FALSE." + ifNo + "." + className);
    vm.writeLabel("IF_GOTO_TRUE." + ifNo + "." + className);
    CompileSym(num, Token::leftBrace);
    CompileStatements(num);
    CompileSym(num, Token::rightBrace);
    vm.writeGoto("END_IF." + ifNo + "." + className);
    if (getToken(num).key == Token::_else)
    {
        vm.writeLabel("IF_GOTO_FALSE." + ifNo + "." + className);
        CompileKeyword(num, Token::_else);
        CompileSym(num, Token::leftBrace);
        CompileStatements(num);
        CompileSym(num, Token::rightBrace);
    }
    else
        vm.writeLabel("IF_GOTO_FALSE." + ifNo + "." + className);
    vm.writeLabel("END_IF." + ifNo + "." + className);

}

void CompilationEngine::CompileExpression(int & num)
{
    CompileTerm(num);

    //Use a stack to support the normal order of arithmetic
    //That is, this program calculate a+b*c as a+(b*c)
    std::stack<int> opstk;
    while (isOp(num))
    {

        while (!opstk.empty()&&priority(num) <= priority(opstk.top()))
        {
            int op = opstk.top();
            CompileOp(op);
            opstk.pop();
        }


        opstk.push(num++);
        CompileTerm(num);
    }
    while (!opstk.empty())
    {
        int op = opstk.top();
        CompileOp(op);
        opstk.pop();
    }
}

void CompilationEngine::CompileTerm(int & num)
{
    switch (getToken(num).gentype)
    {
        case Token::_string:CompileString(num);
            break;
        case Token::constant:CompileConstant(num);
            break;
        case Token::keyword:CompileKeyword(num, getToken(num).key);
            break;
        case Token::identifier:
            advance(num);
            if (getToken(num).sym == Token::leftSquareBrace)
            {
                goBack(num);
                WritePushVarName(num);
                CompileSym(num, Token::leftSquareBrace);
                CompileExpression(num);
                CompileSym(num, Token::rightSquareBrace);
                vm.writeArth(Token::plus);
                vm.writePop("pointer", 1);
                vm.writePush("that", 0);
            }
            else if (getToken(num).sym == Token::leftParenthese|| getToken(num).sym == Token::period)
            {
                goBack(num);
                CompileSubroutineCall(num);
            }
            else
            {
                goBack(num);
                WritePushVarName(num);
            }

            break;
        case Token::symbol:
            if (getToken(num).sym == Token::_not)
            {
                int symNum = num;
                advance(num);
                CompileTerm(num);
                CompileSym(symNum, getToken(symNum).sym);
            }
            else if (getToken(num).sym == Token::minus)
            {
                advance(num);
                CompileTerm(num);
                vm.writeArth(Token::neg);
            }
            else if (getToken(num).sym == Token::leftParenthese)
            {
                CompileSym(num, Token::leftParenthese);
                if (getToken(num).sym == Token::rightParenthese)
                    CompileSym(num, Token::rightParenthese);
                else
                {
                    CompileExpression(num);
                    CompileSym(num, Token::rightParenthese);
                }
            }
            else
                throw error_string_not_end();
            break;
        default:
            throw error_string_not_end();
            break;
    }
}



inline string CompilationEngine::typeConverter(int & num, string & result) {
    if (isType(num) || getToken(num).key == Token::_void)
        result = TypeConverter(tokens.Tokenlist[num]);
    else
        throw error_string_not_end();
    advance(num); return result;
}



void CompilationEngine::CompileExpressionList(int & num, int & nVar, Kind dest)
{
    int start;
    if (dest == METHOD )
    {
        start = 1;
    }
    else
        start = 0;
    nVar = 0;
    if (isExpression(num))
    {
        CompileExpression(num);
        nVar++;
        while (getToken(num).sym == Token::dot)
        {
            CompileSym(num, Token::dot);
            CompileExpression(num);
            nVar++;
        }
    }

}

void CompilationEngine::CompileSym(int & num, Token::Symbol  sym)
{
    if (getToken(num).sym == sym)
    {
        switch (sym)
        {
            case Token::leftBrace:
                break;
            case Token::rightBrace:
                break;
            case Token::leftParenthese:
                break;
            case Token::rightParenthese:
                break;
            case Token::leftSquareBrace:
                break;
            case Token::rightSquareBrace:
                break;
            case Token::dot:
                break;
            case Token::period:
                break;
            case Token::semicolon:
                break;
            case Token::plus:
            case Token::minus:
            case Token::multi:
            case Token::divide:
            case Token::_and:
            case Token::_or:
            case Token::slant:
            case Token::lesser:
            case Token::greater:
            case Token::equal:
            case Token::_not:
                vm.writeArth(sym);
                break;
        }
        advance(num);
    }
    else
        throw error_string_not_end();
}

void CompilationEngine::CompileIdentifier(int & num, string & str)
{
    if (getToken(num).gentype == Token::identifier)
        str = getToken(num).text;
    else
        throw error_string_not_end();
    advance(num);
}

void CompilationEngine::defineVarName(int & num,  string const & type, Kind  kind)
{
    if (getToken(num).gentype == Token::identifier)
    {
        stable.Define(getToken(num), type, kind);
        if(kind==CLASS)
            vm.setClassName(getToken(num).text);
    }
    else
        throw error_string_not_end();
    advance(num);
}

bool CompilationEngine::isDefinedVarName(int & num)
{
    switch (stable.KindOf(getToken(num)))
    {
        case VAR:
        case STATIC:
        case ARG:
        case FIELD:
            return true;
        default:
            return false;
    }
}

inline void CompilationEngine::WritePushVarName(int & num)
{
    string seg;
    if (isDefinedVarName(num))
    {
        switch (KindOf(num))
        {
            case VAR:seg = "local";
                break;
            case ARG:seg = "argument";
                break;
            case FIELD:seg = "this";
                break;
            case STATIC:seg = "static";
                break;
            default:
                break;
        }
        vm.writePush(seg, IndexOf(num));
        advance(num);
    }

    else throw error_string_not_end();
}

void CompilationEngine::WritePopVarName(int & num)
{
    string seg;
    if (isDefinedVarName(num))
    {
        switch (KindOf(num))
        {
            case VAR:seg = "local";
                break;
            case ARG:seg = "argument";
                break;
            case FIELD:seg = "this";
                break;
            case STATIC:seg = "static";
                break;
            default:
                break;
        }
        vm.writePop(seg, IndexOf(num));
    }

    else throw error_string_not_end();
}


void CompilationEngine::CompileKeyword(int & num, Token::Keyword  key)
{
    if (getToken(num).key == key)
    {
        switch (key)
        {

            case Token::_class:
                break;
            case Token::_constructor:
                break;
            case Token::_function:
                break;
            case Token::_method:
                break;
            case Token::_field:
                break;
            case Token::_static:
                break;
            case Token::_var:
                break;
            case Token::_int:
                break;
            case Token::_char:
                break;
            case Token::_boolean:
                break;
            case Token::_void:
                break;
            case Token::_true:
                vm.writePush("constant", 1);
                vm.writeArth(Token::neg);
                break;
            case Token::_false:
                vm.writePush("constant", 0);
                break;
            case Token::_null:
                vm.writePush("constant", 0);
                break;
            case Token::_this:
                vm.writePush("pointer",0);
                break;
            case Token::_let:
                break;
            case Token::_do:
                break;
            case Token::_if:
                break;
            case Token::_else:
                break;
            case Token::_while:
                break;
            case Token::_return:
                break;
            default:
                break;
        }
        advance(num);
    }
    else
        throw error_string_not_end();
}

void CompilationEngine::CompileKeyword(int & num, Token::Keyword  key,Kind & kd)
{
    if (getToken(num).key == key)
    {
        switch (key)
        {

            case Token::_class:
                kd = CLASS;
                break;
            case Token::_constructor:
                kd = CONSTRUCTOR;
                break;
            case Token::_function:
                kd = FUNCTION;
                break;
            case Token::_method:
                kd = METHOD;
                break;
            case Token::_field:
                kd = FIELD;
                break;
            case Token::_static:
                kd = STATIC;
                break;
            case Token::_var:
                kd = VAR;
                break;
            case Token::_int:
                break;
            case Token::_char:
                break;
            case Token::_boolean:
                break;
            case Token::_void:
                break;
            case Token::_true:
                break;
            case Token::_false:
                break;
            case Token::_null:
                break;
            case Token::_this:
                break;
            case Token::_let:
                break;
            case Token::_do:
                break;
            case Token::_if:
                break;
            case Token::_else:
                break;
            case Token::_while:
                break;
            case Token::_return:
                break;
            default:
                break;
        }
        advance(num);
    }
    else
        throw error_string_not_end();
}



void CompilationEngine::CompileSubroutineBody(int & num, string const & funcName)
{

    CompileSym(num, Token::leftBrace);
    while (getToken(num).key== Token::_var)
        CompileVarDec(num);
    vm.writeFunction(className + '.' + funcName, stable.numVar);
    if (stable.KindOf(funcName) == CONSTRUCTOR)
    {
        vm.writePush("constant", stable.numField);
        vm.writeCall("Memory.alloc", 1);
        vm.writePop("pointer", 0);
    }
    else if (stable.KindOf(funcName) == METHOD)
    {
        vm.writePush("argument", 0);
        vm.writePop("pointer", 0);
        stable.addSubScopeIndex();
    }
    CompileStatements(num);

    CompileSym(num, Token::rightBrace);
}

void CompilationEngine::CompileString(int & num)
{
    vm.writePush("constant", getToken(num).text.size());
    vm.writeCall("String.new", 1);
    for (int i = 0; i < getToken(num).text.size(); i++)
    {
        //vm.writePop("argument", 0);
        vm.writePush("constant", getToken(num).text[i]);
        //vm.writePop("argument", 1);
        vm.writeCall("String.appendChar", 2);
    }
    advance(num);
}

void CompilationEngine::CompileConstant(int & num)
{
    vm.writePush("constant", getToken(num).num);
    advance(num);
}

void CompilationEngine::CompileOp(int & num)
{
    if (isOp(num))
        CompileSym(num, getToken(num).sym);
    else
        throw error_string_not_end();
}

void CompilationEngine::writeSubroutineCall(string const & name, int numVar)
{

    vm.writeCall(name, numVar);

}

void CompilationEngine::advance(int & num)
{
    if(num<tokens.Tokenlist.size()-1)
        num++;
    while (num<tokens.Tokenlist.size()&&getToken(num).gentype == Token::comment)
    {
        vm.writeComment( getToken(num).text);
        num++;
    }
}

void CompilationEngine::goBack(int & num)
{
    if (num>=0)
        num--;
    while (num>=0 && getToken(num).gentype == Token::comment)
        num--;
}

int CompilationEngine::priority(int num)
{
    switch (getToken(num).sym)
    {
        case Token::plus:
        case Token::minus:
            return 1;
        case Token::multi:
        case Token::divide:
            return 2;
    }
    return 0;
}

inline bool CompilationEngine::isClassVarDec(int num)
{
    if (tokens.Tokenlist[num].key == Token::_static || tokens.Tokenlist[num].key == Token::_field)
        return true;
    else
        return false;
}

CompilationEngine::CompilationEngine(istream &is, ostream &os) :vm(os), tokens(is) { tokens.process(); }
