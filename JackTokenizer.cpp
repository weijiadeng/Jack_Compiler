#include"stdafx.h"

#include"JackTokenizer.h"

void JackTokenizer::process() {
    for (auto & line : code)
        processLine(line, 0);
}

//Status == 0 means current process cleared
//Status == 1 means process within one line remaining
//Status == 2 means process for multiple lines remaining

void
JackTokenizer::extractStartComment(string &word, int startPos, int &status) {
    if (startPos >= word.size())
        return;
    int i = startPos;
    for (; i < word.size() - 1; i++) {
        if (word[i] == '/')
        {
            if (word[i + 1] == '/') {
                //Current line processing unfinished, status = 1
                status = 1;
                break;
            }
            else if (word[i + 1] == '*') {
                //Current multiple lines processing unfinished, status = 2
                status = 2;
                break;
            }
        }

    }
    string temp;
    for (int k = 0; k<i; k++)
        temp.push_back(word[k]);
    string newComment;
    for (int k = i; k<word.size(); k++)
        newComment.push_back(word[k]);
    if (status == 1)
    {
        comment.emplace_back(newComment);
        temp += '\n';
        temp += IntToString(int(comment.size() - 1));
        temp += '\n';
        word = temp;
        //Process finished, status back to 0
        status = 0;
    }
    else if (status == 2)
        //Process the remaining comments
        extractEndComment(word, i + 1, status);


}

void
JackTokenizer::extractEndComment(string &word, int startPos, int &status) {
    if (startPos >= word.size())
        return;
    int i = startPos, newStart = 0;
    int previousStatus = status;

    //Multiple lines processing unfinished, means the comment must ends with an */
    if (status == 2) {
        for (; i < word.size() - 1; i++) {
            if (word[i] == '*')
                if (word[i + 1] == '/') {
                    status = 0;
                    break;
                }
        }
    }
    string temp, newString;
    for (int m = 0; m<startPos - 1; m++)
        temp.push_back(word[m]);

    //Save the truncated comments
    if (previousStatus == 2)
    {
        for (int j = startPos - 1; j<i + 2 && j<word.size(); j++)
            newString.push_back(word[j]);
        if (status == 2)
            newString.push_back('\n');
        comment.emplace_back(newString);
        temp += '\n';
        temp += IntToString(comment.size() - 1);
        temp += '\n';
        for (int j = i + 2; j<word.size(); j++)
            temp.push_back(word[j]);
        newStart = startPos + 1 + IntToString(comment.size() - 1).size();

    }
    word = temp;

    //Comment process finished, find the next comment.
    if (status == 0) {
        extractStartComment(word, newStart, status);
    }

}


JackTokenizer::JackTokenizer(istream &is) {
    initialize();
    string temp;
    getline(is, temp);
    while (is)
    {
        trim(temp);
        if (!temp.empty())
            code.emplace_back(temp);
        getline(is, temp);
    }
    convertComments();
}



void JackTokenizer::convertComments() {

    int status = 0;//0:no comment,1:comment,2:string
    for (auto & word : code)
    {
        switch (status) {
            case 0:
                //find starts of comments, replace comments by /n num(string num), replace strings by /0 num(string no.)
                extractStartComment(word, 0, status);
                break;
            case 1:
            case 2:
                //find ends of comments, replace comments by /n num(comment num), replace strings by /0 num(string no.)
                extractEndComment(word, 0, status);
                break;
        }
    }
}

//Convert certain line to tokens
void JackTokenizer::processLine(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;
    else if (isdigit(line[startPos]))
        processDigit(line, startPos);
    else if (isalpha(line[startPos]) || line[startPos] == '_')
        processAlpha(line, startPos);
    else if (line[startPos] == '\"')
        processString(line, startPos);
    else if (ispunct(line[startPos]))
        processPunct(line, startPos);
    else if (line[startPos] == '\n')
        processComment(line, startPos);
    //Process the next word
    else if (isblank(line[startPos]))
        processLine(line, ++startPos);
}

//Convert digits to token
void JackTokenizer::processDigit(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;

    //Find the range of current token
    int endPos = find_token_end(line,startPos);

    if (isdigit(line[endPos]))
        endPos++;
    string num;
    for (int i = startPos; i < endPos; i++)
        num.push_back(line[i]);
    Tokenlist.emplace_back(Token());
    Tokenlist.back().gentype = Token::constant;
    Tokenlist.back().num = StringToInt(num);

    //Process the next word
    processLine(line, endPos);
}

inline void JackTokenizer::processAlpha(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;
    //Find the range of current token
    int endPos = find_token_end(line,startPos);

    if ((isalpha(line[endPos])) || (isdigit(line[endPos])) || (line[endPos] == '_'))
        endPos++;
    string word;
    for (int i = startPos; i < endPos; i++)
        word.push_back(line[i]);
    //Not a keyword, must be a identifier
    if (keyWordList.count(word) == 0)
    {
        Tokenlist.emplace_back(Token());
        Tokenlist.back().gentype = Token::identifier;
        Tokenlist.back().text = word;
    }
    else
    {
        Tokenlist.emplace_back(Token());
        Tokenlist.back().gentype = Token::keyword;
        Tokenlist.back().key = keyWordList[word];
    }
    processLine(line, endPos);
}

//Process punctuations like ";", "," and so on
inline void JackTokenizer::processPunct(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;
    if (symbolList.count(line[startPos]) == 0)
        throw error_string_not_end();
    Tokenlist.emplace_back(Token());
    Tokenlist.back().gentype = Token::symbol;
    Tokenlist.back().sym = symbolList[line[startPos]];
    processLine(line, ++startPos);
}

//Process comments that mixed within certain clause
inline void JackTokenizer::processComment(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;

    //Find the range of current token
    int endPos = find_token_end(line,startPos);

    string num;

    for (int i = startPos; i < endPos; i++)
        num.push_back(line[i]);
    Tokenlist.emplace_back(Token());
    Tokenlist.back().gentype = Token::comment;
    Tokenlist.back().text = comment[StringToInt(num)];
    processLine(line, endPos + 1);
}

//Process const string value
inline void JackTokenizer::processString(string const & line, int startPos)
{
    if (startPos >= line.size())
        return;

    int endPos = startPos + 1;
    for (; endPos < line.size(); endPos++)
        if (line[endPos] == '\"')
            break;
    if (line[endPos] != '\"')
        throw error_string_not_end();
    endPos++;
    string text;
    for (int i = startPos+1; i < endPos-1; i++)
        text.push_back(line[i]);
    Tokenlist.emplace_back(Token());
    Tokenlist.back().gentype = Token::_string;
    Tokenlist.back().text = text;
    processLine(line, endPos);
}

void JackTokenizer::initialize()
{
    keyWordList.emplace(make_pair("class", Token::_class));
    keyWordList.emplace(make_pair("constructor", Token::_constructor));
    keyWordList.emplace(make_pair("function", Token::_function));
    keyWordList.emplace(make_pair("method", Token::_method));
    keyWordList.emplace(make_pair("field", Token::_field));
    keyWordList.emplace(make_pair("static", Token::_static));
    keyWordList.emplace(make_pair("var", Token::_var));
    keyWordList.emplace(make_pair("int", Token::_int));
    keyWordList.emplace(make_pair("char", Token::_char));
    keyWordList.emplace(make_pair("boolean", Token::_boolean));
    keyWordList.emplace(make_pair("void", Token::_void));
    keyWordList.emplace(make_pair("true", Token::_true));
    keyWordList.emplace(make_pair("false", Token::_false));
    keyWordList.emplace(make_pair("null", Token::_null));
    keyWordList.emplace(make_pair("this", Token::_this));
    keyWordList.emplace(make_pair("let", Token::_let));
    keyWordList.emplace(make_pair("do", Token::_do));
    keyWordList.emplace(make_pair("if", Token::_if));
    keyWordList.emplace(make_pair("else", Token::_else));
    keyWordList.emplace(make_pair("while", Token::_while));
    keyWordList.emplace(make_pair("return", Token::_return));
    symbolList.emplace(make_pair('{', Token::leftBrace));
    symbolList.emplace(make_pair('}', Token::rightBrace));
    symbolList.emplace(make_pair('(', Token::leftParenthese));
    symbolList.emplace(make_pair(')', Token::rightParenthese));
    symbolList.emplace(make_pair('[', Token::leftSquareBrace));
    symbolList.emplace(make_pair(']', Token::rightSquareBrace));
    symbolList.emplace(make_pair('.', Token::period));
    symbolList.emplace(make_pair(',', Token::dot));
    symbolList.emplace(make_pair(';', Token::semicolon));
    symbolList.emplace(make_pair('+', Token::plus));
    symbolList.emplace(make_pair('-', Token::minus));
    symbolList.emplace(make_pair('*', Token::multi));
    symbolList.emplace(make_pair('/', Token::divide));
    symbolList.emplace(make_pair('&', Token::_and));
    symbolList.emplace(make_pair('|', Token::_or));
    symbolList.emplace(make_pair('<', Token::lesser));
    symbolList.emplace(make_pair('>', Token::greater));
    symbolList.emplace(make_pair('=', Token::equal));
    symbolList.emplace(make_pair('~', Token::_not));

}


string &trim(string &s) {
    if (s.empty()) {
        return s;
    }

    string::iterator c;
    // Erase whitespace before the string

    for (c = s.begin(); c != s.end() && isspace(*c++);); s.erase(s.begin(), --c);

    // Erase whitespace after the string

    for (c = s.end(); c != s.begin() && isspace(*--c);); s.erase(++c, s.end());

    return s;
}

string IntToString(int num)
{
    ostringstream ofs;
    ofs << num;
    return ofs.str();

}

int StringToInt(string const & string)
{
    istringstream ifs(string);
    int num;
    ifs >> num;
    return num;

}

//Find the range of current token
int find_token_end(const string &line, int startPos) {
    int endPos = startPos + 1;
    for (; endPos < line.size(); endPos++)
        if (line[endPos] == '\"')
            break;
    return endPos;
}
