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
#include "state.hpp"

using namespace std;

State::State(std::set<State*> NFAState, int nID)
{
    NFAStates = NFAState;
    StateID = nID;
    Accept = false;
    GroupID = 0;
    StateIterator iter;
    for (iter = NFAState.begin(); iter != NFAState.end(); ++iter)
    {
        if ((*iter)->Accept)
        {
            Accept = true;
        }
    }
}
State::State(const State& other)
{
    *this = other;
}

State::~State()
{
    NFAStates.clear();
    Transition.clear();
}

void State::AddTransition(char inputCh, State* pState)
{
    Transition.insert(std::make_pair(inputCh, pState));
}

void State::GetTransition(char inputCh, Table& States)
{
    States.clear();
    multimap<char, State*>::iterator iter;
    for (iter = Transition.lower_bound(inputCh); iter != Transition.upper_bound(inputCh); ++iter)
    {
        State* pState = iter->second;
        States.push_back(pState);
    }
}

State& State::operator=(const State& other)
{
    this->Transition = other.Transition;
    this->StateID = other.StateID;
    this->NFAStates = other.NFAStates;
    return *this;
}

bool State::operator==(const State& other)
{
    if (NFAStates.empty())
        return(StateID == other.StateID);
    else
        return(NFAStates == other.NFAStates);
}

string State::getStringID()
{
    stringstream out;
    out << StateID;
    return out.str();
}

set<State*>& State::GetNFAState()
{
    return NFAStates;
}
