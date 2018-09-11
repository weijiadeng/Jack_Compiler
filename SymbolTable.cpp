#include "stdafx.h"
#include "SymbolTable.h"

void SymbolTable::startSubroutine() { subScope.clear();  numArg = 0; numVar = 0; }

void SymbolTable::addSubScopeIndex()
{
    for (auto &x : subScope)
    {
        if (x.second.kind == ARG)
            ++x.second.index;
    }
}

void SymbolTable::Define(Token const & token, string const & type, Kind kind)
{
    int indexNum = 0;
    switch (kind)
    {
        case NOTKIND:
            break;
        case STATIC:
            indexNum = numStat++;
            break;
        case FIELD:
            indexNum = numField++;
            break;
        case VAR:
            indexNum = numVar++;
            break;
        case ARG:
            indexNum = numArg++;
            break;
        default:
            break;
    }
    switch (kind)
    {
        case NOTKIND:
            throw error_string_not_end();
            break;
        case STATIC:
        case FIELD:
            classScope.emplace(make_pair(token.text, Varinfo(indexNum, type, kind)));
            break;
        case VAR:
        case ARG:
            subScope.emplace(make_pair(token.text, Varinfo(indexNum, type, kind)));
            break;
        default:
            classScope.emplace(make_pair(token.text, Varinfo(-1, type, kind)));
            break;
    }
}

int SymbolTable::VarCount(Kind kind)
{
    switch (kind)
    {
        case NOTKIND:
            throw error_string_not_end();
            break;
        case STATIC:
            return numStat;
            break;
        case FIELD:
            return numField;
            break;
        case VAR:
            return numVar;
            break;
        case ARG:
            return numArg;
            break;
        default:
            break;
    }
}

Kind SymbolTable::KindOf(string const & str)
{
    if (classScope.count(str) != 0)
        return classScope[str].kind;
    else 	if (subScope.count(str) != 0)
        return subScope[str].kind;

    else
        return NOTKIND;
}

string SymbolTable::TypeOf(string const & str)
{
    if (classScope.count(str) != 0)
        return classScope[str].type;
    else 	if (subScope.count(str) != 0)
        return subScope[str].type;

    else
        return "";
}

int SymbolTable::IndexOf(string const & str)
{
    if (classScope.count(str) != 0)
        return classScope[str].index;
    else 	if (subScope.count(str) != 0)
        return subScope[str].index;

    else
        return -1;
}



Kind SymbolTable::KindOf(Token & kind)
{
    return KindOf(kind.text);
}

string SymbolTable::TypeOf(Token & kind)
{
    return TypeOf(kind.text);
}

int SymbolTable::IndexOf(Token & kind)
{
    return IndexOf(kind.text);
}



string TypeConverter(Token const & token)
{
    switch (token.gentype)
    {
        case Token::keyword:
            switch (token.key)
            {
                case Token::_int:
                    return "int";
                case Token::_char:
                    return "char";
                case Token::_boolean:
                    return "boolean";
                case Token::_void:
                    return "void";
                default:
                    return "";
            }
        case Token::identifier:
            return token.text;
        default:
            return "";
    }
}