#include "stdafx.h"
#include "VMWriter.h"




VMWriter::~VMWriter()
{
}

void VMWriter::writePush(string const & seg, int idx)
{
    os << "push " << seg << " " << idx << endl;
}

void VMWriter::writePop(string const & seg, int idx)
{
    os << "pop " << seg << " " << idx << endl;
}

void VMWriter::writeArth(Token::Symbol command)
{
    switch (command)
    {
        case Token::not_symbol:
            break;
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
            os << "add\n";
            break;
        case Token::minus:
            os << "sub\n";
            break;
        case Token::multi:
            writeCall("Math.multiply", 2);
            break;
        case Token::divide:
            writeCall("Math.divide", 2);
            break;
        case Token::slant:
            break;
        case Token::_and:
            os << "and\n";
            break;
        case Token::_or:
            os << "or\n";
            break;
        case Token::lesser:
            os << "lt\n";
            break;
        case Token::greater:
            os << "gt\n";
            break;
        case Token::equal:
            os << "eq\n";
            break;
        case Token::_not:
            os << "not\n";
            break;
        case Token::neg:
            os << "neg\n";
            break;
        default:
            break;
    }
}

void VMWriter::writeLabel(string const & label)
{
    os << "label " << label << endl;
}

void VMWriter::writeGoto(string const & label)
{
    os << "goto " << label << endl;
}

void VMWriter::writeIf(string const & label)
{
    os << "if-goto " << label << endl;

}

void VMWriter::writeCall(string const & name, int nArg)
{
    os << "call " << name <<" "<<nArg<< endl;
}

void VMWriter::writeFunction(string const & label, int nArg)
{
    os << "function " << label << " " << nArg << endl;
}

void VMWriter::writeComment(string const & comment)
{
    os << "//" << comment << endl;
}

void VMWriter::writeReturn()
{
    os << "return"<<endl;
}