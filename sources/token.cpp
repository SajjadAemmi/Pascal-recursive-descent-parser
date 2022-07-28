#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "token.hpp"

using namespace std;

Token::Token()
{

}

Token::Token(string value)
{
    this->value = value;
    this->type = TypeDetection(this->value);
}

string Token::TypeDetection(string s)
{
	if (s == "main" || s == "if" || s == "then" || s == "else" || s == "while" || s == "do" || s == "begin" || s == "end" || s == "var" || s == "integer" || s == "real" || s == "for" || s == "function" || s == "array" || s == "procedure" || s == "result" || s == "program")
	{
		return "KEYWORD";
	}
	else if (s == "=" || s == "<>" || s == "<=" || s == ">=" || s == ">" || s == "<")
	{
		return "RELOP";
	}
	else if (s == "+" || s == "-" || s == "or")
	{
		return "ADDOP";
	}
	else if (s == "*" || s == "/" || s == "div" || s == "mod" || s == "and")
	{
		return "MULOP";
	}
	else if (s == ":=")
	{
		return "ASSIGNOP";
	}
	else if (s == "(" || s == ")" || s == "," || s == ":" || s == ";")
	{
		return "SPECIALSYMBOL";
	}
	else if (s[0] <= '9' && s[0] >= '0')
	{
		return "CONSTANT";
	}
	else
	{
		return "IDENTIFIER";
	}
}