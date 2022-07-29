#pragma once

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

	stack<Table>		thompson_stack;
	set<char>           input_set;
	string              postfix_re;
	char                CurPreProcChar;
	int                 next_state_id;
	vector<string>		regular_expressions;
	string				infix_re;
	string				buffer;
	vector<Token> 		tokens;

	void readInfixREFromFile();
	void preProcessInfixRE();
	string removeBracket(string);
	void convertInfixREToPostfixRE();
	bool takesPrecedence(char, char);
	void writePostfixREToFile();
	bool convertPostfixREToNFA();
	bool match(string);
	void createOneCharNFA(char);
	bool popTable(Table& NFATable);

	bool concat();
	bool closure();
	bool or();

	bool isOperator(char inputCh);
	void epsilonClosure(set<State*> startSet, set<State*>& result);
	void move(char chInput, set<State*> NFAState, set<State*>& Result);
	void convertNFAToDFA();
	void writeNFATableToFile();
	void readProgramFromFile();
	void scanner();
	void writeTokensToFile();
};
