#pragma once

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include "token.hpp"
#include "state.hpp"

using namespace std;

typedef vector<State*> Table;
typedef Table::reverse_iterator TableReverseIterator;
typedef Table::iterator TableIterator;
typedef set<State*>::iterator StateIterator;

class LexAnalyser
{
public:

	stack<Table>		ThompsonStack;
	stack<char>         ExpressionStack;
	set<char>           InputSet;
	string              strText;
	string              PostfixRE;
	char                CurPreProcChar;
	int                 NextStateID;
	vector<string>		RegularExpressions;
	string				InfixRE;
	string				Buffer;
	vector<Token> 		Tokens;

	void InputFileInfixRE();
	void PreProcessInfixRE();
	string RemoveBracket(string);
	void ConvertInfixREToPostfixRE();
	bool TakesPrecedence(char, char);
	void OutputFilePostfixRE();
	bool ConvertPostfixREToNFA();
	bool Match(string);
	void CreateOneCharNFA(char);
	bool PopTable(Table& NFATable);

	bool Concat();
	bool Closure();
	bool Or();

	bool IsOperator(char inputCh);
	void EpsilonClosure(set<State*> startSet, set<State*>& result);
	void Move(char chInput, set<State*> NFAState, set<State*>& Result);
	void ConvertNFAtoDFA();
	void OutputFileNFATable();
	void InputFileProgram();
	void Scanner();
	void OutputFileTokens();
};