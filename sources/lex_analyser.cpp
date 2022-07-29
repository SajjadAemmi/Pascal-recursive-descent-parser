#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <set>
#include <list>
#include <string>
#include <map>
#include "token.hpp"
#include "state.hpp"
#include "lex_analyser.hpp"

using namespace std;

#define EPSILON			-1
#define CLOSURE			'*'
#define OPEN_PAREN		'('
#define CLOSE_PAREN		')'
#define OR				'+'
#define CONCAT			'.'

typedef vector<State*> Table;
typedef Table::reverse_iterator TableReverseIterator;
typedef Table::iterator TableIterator;
typedef set<State*>::iterator StateIterator;

Table NFATable;
Table DFATable;

bool LexAnalyser::match(string S)
{
	State* pState = DFATable[0];
	vector<State*>  Transition;
	for (int i = 0; i < S.size(); i++)
	{
		char CurrChar = S[i];
		pState->getTransition(CurrChar, Transition);
		if (Transition.empty())
			return false;
		pState = Transition[0];
	}
	if (pState->accept)
		return true;

	return false;
}

bool LexAnalyser::convertPostfixREToNFA()
{
	//Thompson NFA

	for (int i = 0; i < (int)postfix_re.size(); i++)
	{
		char Ch = postfix_re[i];

		if (!isOperator(Ch))
		{
			if (Ch == '~')
			{
				i++;
				Ch = postfix_re[i];
			}
			createOneCharNFA(Ch);
		}
		else if (isOperator(Ch))
		{
			if (Ch == '.')
				concat();
			else if (Ch == '*')
				closure();
			else if (Ch == '+')
				or();
		}
	}
	popTable(NFATable);
	NFATable[NFATable.size() - 1]->accept = true;
	return true;
}

void LexAnalyser::createOneCharNFA(char ch)
{
	State* s0 = new State(next_state_id++);
	State* s1 = new State(next_state_id++);

	s0->addTransition(ch, s1);

	Table NewSubTable;
	NewSubTable.push_back(s0);
	NewSubTable.push_back(s1);
	thompson_stack.push(NewSubTable);

	input_set.insert(ch);
}

bool LexAnalyser::popTable(Table& NFATable)
{
	if (thompson_stack.size() > 0)
	{
		NFATable = thompson_stack.top();
		thompson_stack.pop();
		return true;
	}
	return false;
}

bool LexAnalyser::concat()
{
	Table LeftTable, RightTable;

	if (!popTable(RightTable) || !popTable(LeftTable))
		return false;

	(*(LeftTable.rbegin()))->addTransition(EPSILON, *(RightTable.begin()));
	LeftTable.insert(LeftTable.end(), RightTable.begin(), RightTable.end());
	thompson_stack.push(LeftTable);

	return true;
}

bool LexAnalyser::closure()
{
	Table PrevTable;

	if (!popTable(PrevTable))
		return false;

	State* LeftTable = new State(next_state_id++);
	State* RightTable = new State(next_state_id++);

	LeftTable->addTransition(EPSILON, RightTable);
	LeftTable->addTransition(EPSILON, *(PrevTable.begin()));
	(*(PrevTable.rbegin()))->addTransition(EPSILON, RightTable);
	(*(PrevTable.rbegin()))->addTransition(EPSILON, *(PrevTable.begin()));

	PrevTable.insert(PrevTable.begin(), LeftTable);
	PrevTable.push_back(RightTable);

	thompson_stack.push(PrevTable);

	return true;
}

bool LexAnalyser::or()
{
	Table  UpperTable, LowerTable;

	if (!popTable(LowerTable) || !popTable(UpperTable))
		return false;

	State* LeftTable = new State(next_state_id++);
	State* RightTable = new State(next_state_id++);

	LeftTable->addTransition(EPSILON, *(UpperTable.begin()));
	LeftTable->addTransition(EPSILON, *(LowerTable.begin()));
	(*(UpperTable.rbegin()))->addTransition(EPSILON, RightTable);
	(*(LowerTable.rbegin()))->addTransition(EPSILON, RightTable);

	LowerTable.push_back(RightTable);
	UpperTable.insert(UpperTable.begin(), LeftTable);
	UpperTable.insert(UpperTable.end(), LowerTable.begin(), LowerTable.end());

	thompson_stack.push(UpperTable);
	return true;
}

bool LexAnalyser::isOperator(char Ch)
{
	if (Ch == CLOSURE || Ch == OR || Ch == OPEN_PAREN || Ch == CLOSE_PAREN || Ch == CONCAT)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LexAnalyser::epsilonClosure(set<State*> startSet, set<State*>& result)
{
	stack<State*> UnVisitedStates;
	result = startSet;
	StateIterator iter;
	for (iter = startSet.begin(); iter != startSet.end(); ++iter)
	{
		UnVisitedStates.push(*iter);
	}

	while (!UnVisitedStates.empty())
	{
		State* curState = UnVisitedStates.top();
		UnVisitedStates.pop();

		Table epsilonStates;
		curState->getTransition(EPSILON, epsilonStates);

		TableIterator epsilonItr;

		for (epsilonItr = epsilonStates.begin(); epsilonItr != epsilonStates.end(); ++epsilonItr)
		{
			if (result.find(*epsilonItr) == result.end())
			{
				result.insert(*epsilonItr);
				UnVisitedStates.push(*epsilonItr);
			}
		}
	}
}

void LexAnalyser::move(char chInput, set<State*> NFAState, set<State*>& Result)
{
	Result.clear();
	StateIterator iter;
	for (iter = NFAState.begin(); iter != NFAState.end(); ++iter)
	{
		Table States;
		(*iter)->getTransition(chInput, States);
		for (int index = 0; index < (int)States.size(); ++index)
		{
			Result.insert(States[index]);
		}
	}
}

void LexAnalyser::convertNFAToDFA()
{
	set<State*> NFAStartStateSet;
	NFAStartStateSet.insert(NFATable[0]);

	set<State*> DFAStartStateSet;
	epsilonClosure(NFAStartStateSet, DFAStartStateSet);

	next_state_id = 0;
	State* DFAStartState = new State(DFAStartStateSet, next_state_id++);

	DFATable.push_back(DFAStartState);

	Table UnVisitedStates;
	UnVisitedStates.push_back(DFAStartState);
	while (!UnVisitedStates.empty()) {

		State* CurDFAState = UnVisitedStates[UnVisitedStates.size() - 1];
		UnVisitedStates.pop_back();

		std::set<char>::iterator iter;
		for (iter = input_set.begin(); iter != input_set.end(); ++iter) {
			std::set<State*> moveRes, epsilonClosureRes;

			move(*iter, CurDFAState->getNFAState(), moveRes);
			epsilonClosure(moveRes, epsilonClosureRes);

			StateIterator moveResItr;
			StateIterator epsilonClosureResItr;

			bool bFound = false;
			State* s = NULL;
			for (int i = 0; i < (int)DFATable.size(); ++i) {
				s = DFATable[i];
				if (s->getNFAState() == epsilonClosureRes) {
					bFound = true;
					break;
				}
			}
			if (!bFound) {
				State* U = new State(epsilonClosureRes, next_state_id++);
				UnVisitedStates.push_back(U);
				DFATable.push_back(U);
				CurDFAState->addTransition(*iter, U);
			}
			else {
				CurDFAState->addTransition(*iter, s);
			}
		}
	}
}

void LexAnalyser::convertInfixREToPostfixRE()
{
	stack<char> OperatorStack;
	char TopSymbol, Symbol;
	int k;

	for (k = 0; k < infix_re.size(); k++)
	{
		Symbol = infix_re[k];

		if (Symbol == '~')
		{
			postfix_re = postfix_re + Symbol;
			k++;
			Symbol = infix_re[k];
			postfix_re = postfix_re + Symbol;
		}
		else if (Symbol == '(')
		{
			OperatorStack.push(Symbol);
		}
		else if (Symbol == ')')
		{
			while (OperatorStack.top() != '(')
			{
				TopSymbol = OperatorStack.top();
				OperatorStack.pop();
				postfix_re = postfix_re + TopSymbol;
			}
			OperatorStack.pop();
		}
		else if (isOperator(Symbol))
		{
			if (OperatorStack.empty() || (!(OperatorStack.empty()) && takesPrecedence(OperatorStack.top(), Symbol)))
			{
				OperatorStack.push(Symbol);
			}
			else
			{
				while (!(OperatorStack.empty()) && !takesPrecedence(OperatorStack.top(), Symbol))
				{
					TopSymbol = OperatorStack.top();
					OperatorStack.pop();
					postfix_re = postfix_re + TopSymbol;
				}
				OperatorStack.push(Symbol);
			}
		}
		else
		{
			postfix_re = postfix_re + Symbol;
		}
	}
	while (!OperatorStack.empty())
	{
		TopSymbol = OperatorStack.top();
		OperatorStack.pop();
		postfix_re = postfix_re + TopSymbol;
	}
	postfix_re += "\0";
}

void LexAnalyser::writePostfixREToFile()
{
	fstream f;
	f.open("IO/output/postfix_regular_expression.txt", ios::out);
	f << postfix_re;
	f.close();
}

string LexAnalyser::removeBracket(string s)
{
	string::size_type startPos, endPos, separatorPos;
	string ReplacedStr;

	startPos = s.find_first_of("[");
	endPos = s.find_first_of("]");
	separatorPos = s.find_first_of("-");

	if (startPos == string::npos || endPos == string::npos)
		return s;

	ReplacedStr += s.substr(0, startPos);
	ReplacedStr.push_back('(');
	string result = s.substr(startPos + 1, endPos - startPos - 1);
	char first = result[0];
	char last = result[result.size() - 1];

	if (separatorPos != string::npos)
	{
		while (first < last)
		{
			ReplacedStr.push_back(first);
			ReplacedStr += "+";
			first++;
		}
		ReplacedStr.push_back(first);
	}
	else
	{
		startPos++;
		while (startPos != endPos - 1)
		{
			ReplacedStr.push_back(s[startPos]);
			ReplacedStr += "+";
			startPos++;
		}
		ReplacedStr.push_back(s[endPos - 1]);
	}
	ReplacedStr.push_back(')');
	ReplacedStr += s.substr(endPos + 1, s.size() - endPos);


	return ReplacedStr;
}

void LexAnalyser::readInfixREFromFile()
{
	fstream file;
	string temp;
	file.open("IO/input/regular_expression.txt", ios::in);

	if (!file)
	{
		cout << "no file available\n";
		exit(1);
	}

	while (!file.eof())
	{
		file >> temp;
		this->regular_expressions.push_back(temp);
	}
}

void LexAnalyser::preProcessInfixRE()
{
	int i;
	for (i = 0; i < regular_expressions.size() - 1; i++)
	{
		infix_re += "(" + regular_expressions[i] + ")" + "+";
	}
	infix_re += "(" + regular_expressions[i] + ")";

	while (infix_re.find("[") != string::npos)
	{
		infix_re = removeBracket(infix_re);
	}
}

void LexAnalyser::writeNFATableToFile()
{
	fstream f;

	f.open("IO/output/NFA.dat", ios::out);

	if (!f)
	{
		cout << "no file available\n";
		system("pause");
		exit(0);
	}
	for (int i = 0; i < (int)NFATable.size(); ++i)
	{
		State* pState = NFATable[i];
		if (pState->accept)
		{
			f << pState->getStringID() << "\n";
		}
	}
	for (int i = 0; i < (int)NFATable.size(); ++i)
	{
		State* pState = NFATable[i];
		vector<State*> State;
		pState->getTransition(EPSILON, State);
		for (int j = 0; j < (int)State.size(); ++j)
		{
			f << pState->getStringID() << "  " << State[j]->getStringID() << "  epsilon\n";
		}
		set<char>::iterator iter;
		for (iter = input_set.begin(); iter != input_set.end(); ++iter)
		{
			pState->getTransition(*iter, State);
			for (int j = 0; j < (int)State.size(); ++j)
			{
				f << pState->getStringID() + "  " << State[j]->getStringID();
				stringstream out;
				out << *iter;
				f << "  " << out.str() << "\n";
			}
		}
	}
	f.close();
}

void LexAnalyser::readProgramFromFile()
{
	fstream file;
	string temp;

	file.open("IO/input/program.pas", ios::in);

	if (!file)
	{
		cout << "no file available\n";
		exit(1);
	}

	std::stringstream buffer_stringstream;
	buffer_stringstream << file.rdbuf();
	this->buffer = buffer_stringstream.str();
}

void LexAnalyser::scanner()
{
	int first = 0;
	int follow = 0;
	int save = 0;

	while (first < this->buffer.size() - 1)
	{
		follow = first;
		while (follow < buffer.size())
		{
			if (match(buffer.substr(first, follow - first + 1)))
				save = follow;

			follow++;
		}
		follow = save;

		string substring = buffer.substr(first, follow - first + 1);
		Token token(substring);
		tokens.push_back(token);

		first = follow + 1;

		while ((buffer[first] == ' ' || buffer[first] == '\n' || buffer[first] == '\t') && buffer[first] != '\0')
			first++;
	}
}

bool LexAnalyser::takesPrecedence(char topStack, char ch)
{
	if (topStack == '(')
	{
		return true;
	}
	char operators[6] = { '+', '.', '*', '(' };

	int i, j;
	for (i = 0; i < strlen(operators); i++)
	{
		if (operators[i] == topStack)
			break;
	}
	for (j = 0; j < strlen(operators); j++)
	{
		if (operators[j] == ch)
			break;
	}

	if (i >= j)
		return false;
	else
		return true;

	// true
	// اولويت کاراکتر ورودي بيشتر است

	// false
	// اولويت کاراکتر بالاي پشته بيشتر است
}

void LexAnalyser::writeTokensToFile()
{
	fstream f;
	f.open("IO/output/tokens.txt", ios::out);
	for (int i = 0; i < tokens.size(); i++)
	{
		f << tokens[i].value << "\t\t\t" << tokens[i].type << "\n";
	}
	f.close();
}