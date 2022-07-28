#pragma once

#include <iostream>

using namespace std;

class Token
{
public:
	string value;
	string type;

	Token();
	Token(string value);
	string TypeDetection(string s);
};
