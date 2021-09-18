#include "DFA.h"
#include "Regex.h"
#include <stdexcept>

DFA::DFA(const std::set<int>& q, const std::map<int, std::map<char, int>>& transitions,
	int q0, const std::set<int>& f)
	: q(q), transitions(transitions), q0(q0), f(f)
{
	// ensure all states are present in the transition map
	for (int state : q)
	{
		if (transitions.find(state) == transitions.end())
		{
			this->transitions.emplace(std::make_pair(state, std::map<char, int>()));
		}
	}
}

int DFA::NumStates()
{
	return q.size();
}

void DFA::BeginSimulation()
{
	currentState = q0;
}

void DFA::OnNext(char input)
{
	if (transitions[currentState].find(input) == transitions[currentState].end())
	{
		// check if this state has an ANY transition, if so, take it
		if (transitions[currentState].find(Regex::ANY) != transitions[currentState].end()
			&& input != Regex::LINE_START && input != Regex::LINE_END)
		{
			input = Regex::ANY;
		}
		else
		{
			inErrorState = true;
		}
	}

	if (!inErrorState)
	{
		currentState = transitions[currentState][input];
	}
}

void DFA::OnNextAll(const std::string& input)
{
	for (char i : input)
	{
		OnNext(i);
	}
}

bool DFA::HasAccepted()
{
	return f.find(currentState) != f.end();
}

bool DFA::HasFailed()
{
	return inErrorState;
}

bool DFA::EndSimulation()
{
	bool result = !inErrorState && f.find(currentState) != f.end();
	currentState = -1;
	inErrorState = false;

	return result;
}

DFA DFA::GenerateEmpty()
{
	std::set<int> q = { 0 };
	std::map<int, std::map<char, int>> transitions;
	int q0 = 0;
	std::set<int> f = { 0 };

	return DFA(q, transitions, q0, f);
}

std::map<int, std::map<char, int>> DFA::MakeTransitionMap(const std::vector<std::tuple<int, char, int>>& easyList)
{
	std::map<int, std::map<char, int>> transitions;

	for (auto& it : easyList)
	{
		
		if (transitions.find(std::get<0>(it)) == transitions.end())
		{
			transitions.emplace(std::get<0>(it), std::map<char, int>());
		}
		transitions[std::get<0>(it)].emplace(std::make_pair(std::get<1>(it), std::get<2>(it)));
	}

	return transitions;
}
