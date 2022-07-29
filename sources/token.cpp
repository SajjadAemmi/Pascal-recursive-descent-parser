#include <iostream>
#include "token.hpp"

using namespace std;

Token::Token()
{

}

Token::Token(string value)
{
    this->value = value;
    this->type = typeDetection(this->value);
}

string Token::typeDetection(string value)
{
	if (value == "main" || value == "if" || value == "then" || value == "else" || value == "while" || value == "do" || value == "begin" || value == "end" || value == "var" || value == "integer" || value == "real" || value == "for" || value == "function" || value == "array" || value == "procedure" || value == "result" || value == "program")
	{
		return "KEYWORD";
	}
	else if (value == "=" || value == "<>" || value == "<=" || value == ">=" || value == ">" || value == "<")
	{
		return "RELOP";
	}
	else if (value == "+" || value == "-" || value == "or")
	{
		return "ADDOP";
	}
	else if (value == "*" || value == "/" || value == "div" || value == "mod" || value == "and")
	{
		return "MULOP";
	}
	else if (value == ":=")
	{
		return "ASSIGNOP";
	}
	else if (value == "(" || value == ")" || value == "," || value == ":" || value == ";")
	{
		return "SPECIALSYMBOL";
	}
	else if (value[0] <= '9' && value[0] >= '0')
	{
		return "CONSTANT";
	}
	else
	{
		return "IDENTIFIER";
	}
}
