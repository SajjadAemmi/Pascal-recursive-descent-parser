#pragma once

#include <iostream>
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
	multimap<char, State*> transition;
	int state_id;
	bool accept;
	bool Marked;
	int  GroupID;

	State() : state_id(-1), accept(false) {};
	State(int nID) : state_id(nID), accept(false), GroupID(0) {};
	State(std::set<State*> NFAState, int nID);
	State(const State& other);
	~State();
	void addTransition(char inputCh, State* pState);
	void getTransition(char inputCh, Table& States);
	State& operator=(const State& other);
	bool operator==(const State& other);
	string getStringID();
	set<State*>& getNFAState();

};