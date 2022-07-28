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

using namespace std;


class State
{
public:
	set<State*> NFAStates;

	typedef vector<State*> Table;
	typedef set<State*>::iterator StateIterator;
	multimap<char, State*> Transition;
	int StateID;
	bool Accept;
	bool Marked;
	int  GroupID;

	State() : StateID(-1), Accept(false) {};
	State(int nID) : StateID(nID), Accept(false), GroupID(0) {};
	State(std::set<State*> NFAState, int nID);
	State(const State& other);
	~State();
	void AddTransition(char inputCh, State* pState);
	void GetTransition(char inputCh, Table& States);
	State& operator=(const State& other);
	bool operator==(const State& other);
	string getStringID();
	set<State*>& GetNFAState();

};