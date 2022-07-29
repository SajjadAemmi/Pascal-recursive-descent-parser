#include <iostream>
#include <set>
#include "state.hpp"

using namespace std;

State::State(std::set<State*> NFAState, int nID)
{
    NFAStates = NFAState;
    state_id = nID;
    accept = false;
    GroupID = 0;
    StateIterator iter;
    for (iter = NFAState.begin(); iter != NFAState.end(); ++iter)
    {
        if ((*iter)->accept)
        {
            accept = true;
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
    transition.clear();
}

void State::addTransition(char inputCh, State* pState)
{
    transition.insert(std::make_pair(inputCh, pState));
}

void State::getTransition(char inputCh, Table& States)
{
    States.clear();
    multimap<char, State*>::iterator iter;
    for (iter = transition.lower_bound(inputCh); iter != transition.upper_bound(inputCh); ++iter)
    {
        State* pState = iter->second;
        States.push_back(pState);
    }
}

State& State::operator=(const State& other)
{
    this->transition = other.transition;
    this->state_id = other.state_id;
    this->NFAStates = other.NFAStates;
    return *this;
}

bool State::operator==(const State& other)
{
    if (NFAStates.empty())
        return(state_id == other.state_id);
    else
        return(NFAStates == other.NFAStates);
}

string State::getStringID()
{
    stringstream out;
    out << state_id;
    return out.str();
}

set<State*>& State::getNFAState()
{
    return NFAStates;
}
