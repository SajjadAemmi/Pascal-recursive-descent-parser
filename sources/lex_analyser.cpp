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

bool LexAnalyser::Match(string S)
{
	State* pState = DFATable[0];
	vector<State*>  Transition;
	for (int i = 0; i < S.size(); i++)
	{
		char CurrChar = S[i];
		pState->GetTransition(CurrChar, Transition);
		if (Transition.empty())
			return false;
		pState = Transition[0];
	}
	if (pState->Accept)
		return true;

	return false;
}

bool LexAnalyser::ConvertPostfixREToNFA()
{
	//Thompson NFA

	for (int i = 0; i < (int)PostfixRE.size(); i++)
	{
		char Ch = PostfixRE[i];

		if (!IsOperator(Ch))
		{
			if (Ch == '~')
			{
				i++;
				Ch = PostfixRE[i];
			}
			CreateOneCharNFA(Ch);
		}
		else if (IsOperator(Ch))
		{
			if (Ch == '.')
				Concat();
			else if (Ch == '*')
				Closure();
			else if (Ch == '+')
				Or();
		}
	}
	PopTable(NFATable);
	NFATable[NFATable.size() - 1]->Accept = true;
	return true;
}

void LexAnalyser::CreateOneCharNFA(char ch)
{
	State* s0 = new State(NextStateID++);
	State* s1 = new State(NextStateID++);

	s0->AddTransition(ch, s1);

	Table NewSubTable;
	NewSubTable.push_back(s0);
	NewSubTable.push_back(s1);
	ThompsonStack.push(NewSubTable);

	InputSet.insert(ch);
}

bool LexAnalyser::PopTable(Table& NFATable)
{
	if (ThompsonStack.size() > 0)
	{
		NFATable = ThompsonStack.top();
		ThompsonStack.pop();
		return true;
	}
	return false;
}

bool LexAnalyser::Concat()
{
	Table LeftTable, RightTable;

	if (!PopTable(RightTable) || !PopTable(LeftTable))
		return false;

	(*(LeftTable.rbegin()))->AddTransition(EPSILON, *(RightTable.begin()));
	LeftTable.insert(LeftTable.end(), RightTable.begin(), RightTable.end());
	ThompsonStack.push(LeftTable);

	return true;
}

bool LexAnalyser::Closure()
{
	Table PrevTable;

	if (!PopTable(PrevTable))
		return false;

	State* LeftTable = new State(NextStateID++);
	State* RightTable = new State(NextStateID++);

	LeftTable->AddTransition(EPSILON, RightTable);
	LeftTable->AddTransition(EPSILON, *(PrevTable.begin()));
	(*(PrevTable.rbegin()))->AddTransition(EPSILON, RightTable);
	(*(PrevTable.rbegin()))->AddTransition(EPSILON, *(PrevTable.begin()));

	PrevTable.insert(PrevTable.begin(), LeftTable);
	PrevTable.push_back(RightTable);

	ThompsonStack.push(PrevTable);

	return true;
}

bool LexAnalyser::Or()
{
	Table  UpperTable, LowerTable;

	if (!PopTable(LowerTable) || !PopTable(UpperTable))
		return false;

	State* LeftTable = new State(NextStateID++);
	State* RightTable = new State(NextStateID++);

	LeftTable->AddTransition(EPSILON, *(UpperTable.begin()));
	LeftTable->AddTransition(EPSILON, *(LowerTable.begin()));
	(*(UpperTable.rbegin()))->AddTransition(EPSILON, RightTable);
	(*(LowerTable.rbegin()))->AddTransition(EPSILON, RightTable);

	LowerTable.push_back(RightTable);
	UpperTable.insert(UpperTable.begin(), LeftTable);
	UpperTable.insert(UpperTable.end(), LowerTable.begin(), LowerTable.end());

	ThompsonStack.push(UpperTable);
	return true;
}

bool LexAnalyser::IsOperator(char Ch)
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

void LexAnalyser::EpsilonClosure(set<State*> startSet, set<State*>& result)
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
		curState->GetTransition(EPSILON, epsilonStates);

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

void LexAnalyser::Move(char chInput, set<State*> NFAState, set<State*>& Result)
{
	Result.clear();
	StateIterator iter;
	for (iter = NFAState.begin(); iter != NFAState.end(); ++iter)
	{
		Table States;
		(*iter)->GetTransition(chInput, States);
		for (int index = 0; index < (int)States.size(); ++index)
		{
			Result.insert(States[index]);
		}
	}
}

void LexAnalyser::ConvertNFAtoDFA()
{
	set<State*> NFAStartStateSet;
	NFAStartStateSet.insert(NFATable[0]);

	set<State*> DFAStartStateSet;
	EpsilonClosure(NFAStartStateSet, DFAStartStateSet);

	NextStateID = 0;
	State* DFAStartState = new State(DFAStartStateSet, NextStateID++);

	DFATable.push_back(DFAStartState);

	Table UnVisitedStates;
	UnVisitedStates.push_back(DFAStartState);
	while (!UnVisitedStates.empty()) {

		State* CurDFAState = UnVisitedStates[UnVisitedStates.size() - 1];
		UnVisitedStates.pop_back();

		std::set<char>::iterator iter;
		for (iter = InputSet.begin(); iter != InputSet.end(); ++iter) {
			std::set<State*> MoveRes, EpsilonClosureRes;

			Move(*iter, CurDFAState->GetNFAState(), MoveRes);
			EpsilonClosure(MoveRes, EpsilonClosureRes);

			StateIterator MoveResItr;
			StateIterator EpsilonClosureResItr;

			bool bFound = false;
			State* s = NULL;
			for (int i = 0; i < (int)DFATable.size(); ++i) {
				s = DFATable[i];
				if (s->GetNFAState() == EpsilonClosureRes) {
					bFound = true;
					break;
				}
			}
			if (!bFound) {
				State* U = new State(EpsilonClosureRes, NextStateID++);
				UnVisitedStates.push_back(U);
				DFATable.push_back(U);
				CurDFAState->AddTransition(*iter, U);
			}
			else {
				CurDFAState->AddTransition(*iter, s);
			}
		}
	}
}

void LexAnalyser::ConvertInfixREToPostfixRE()
{
	stack<char> OperatorStack;
	char TopSymbol, Symbol;
	int k;

	for (k = 0; k < InfixRE.size(); k++)
	{
		Symbol = InfixRE[k];

		if (Symbol == '~')
		{
			PostfixRE = PostfixRE + Symbol;
			k++;
			Symbol = InfixRE[k];
			PostfixRE = PostfixRE + Symbol;
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
				PostfixRE = PostfixRE + TopSymbol;
			}
			OperatorStack.pop();
		}
		else if (IsOperator(Symbol))
		{
			if (OperatorStack.empty() || (!(OperatorStack.empty()) && TakesPrecedence(OperatorStack.top(), Symbol)))
			{
				OperatorStack.push(Symbol);
			}
			else
			{
				while (!(OperatorStack.empty()) && !TakesPrecedence(OperatorStack.top(), Symbol))
				{
					TopSymbol = OperatorStack.top();
					OperatorStack.pop();
					PostfixRE = PostfixRE + TopSymbol;
				}
				OperatorStack.push(Symbol);
			}
		}
		else
		{
			PostfixRE = PostfixRE + Symbol;
		}
	}
	while (!OperatorStack.empty())
	{
		TopSymbol = OperatorStack.top();
		OperatorStack.pop();
		PostfixRE = PostfixRE + TopSymbol;
	}
	PostfixRE += "\0";
}

void LexAnalyser::OutputFilePostfixRE()
{
	fstream f;
	f.open("IO/output/postfix_regular_expression.txt", ios::out);
	f << PostfixRE;
	f.close();
}

string LexAnalyser::RemoveBracket(string s)
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

void LexAnalyser::InputFileInfixRE()
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
		this->RegularExpressions.push_back(temp);
	}
}

void LexAnalyser::PreProcessInfixRE()
{
	int i;
	for (i = 0; i < RegularExpressions.size() - 1; i++)
	{
		InfixRE += "(" + RegularExpressions[i] + ")" + "+";
	}
	InfixRE += "(" + RegularExpressions[i] + ")";

	while (InfixRE.find("[") != string::npos)
	{
		InfixRE = RemoveBracket(InfixRE);
	}
}

void LexAnalyser::OutputFileNFATable()
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
		if (pState->Accept)
		{
			f << pState->getStringID() << "\n";
		}
	}
	for (int i = 0; i < (int)NFATable.size(); ++i)
	{
		State* pState = NFATable[i];
		vector<State*> State;
		pState->GetTransition(EPSILON, State);
		for (int j = 0; j < (int)State.size(); ++j)
		{
			f << pState->getStringID() << "  " << State[j]->getStringID() << "  epsilon\n";
		}
		set<char>::iterator iter;
		for (iter = InputSet.begin(); iter != InputSet.end(); ++iter)
		{
			pState->GetTransition(*iter, State);
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

void LexAnalyser::InputFileProgram()
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
	this->Buffer = buffer_stringstream.str();
}

void LexAnalyser::Scanner()
{
	int first = 0;
	int follow = 0;
	int save = 0;

	while (first < this->Buffer.size() - 1)
	{
		follow = first;
		while (follow < Buffer.size())
		{
			if (Match(Buffer.substr(first, follow - first + 1)))
				save = follow;

			follow++;
		}
		follow = save;

		string substring = Buffer.substr(first, follow - first + 1);
		Token token(substring);
		Tokens.push_back(token);

		first = follow + 1;

		while ((Buffer[first] == ' ' || Buffer[first] == '\n' || Buffer[first] == '\t') && Buffer[first] != '\0')
			first++;
	}
}

bool LexAnalyser::TakesPrecedence(char topStack, char ch)
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

void LexAnalyser::OutputFileTokens()
{
	fstream f;
	f.open("IO/output/tokens.txt", ios::out);
	for (int i = 0; i < Tokens.size(); i++)
	{
		f << Tokens[i].value << "\t\t\t" << Tokens[i].type << "\n";
	}
	f.close();
}