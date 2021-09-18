#include "NFA.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>

NFA::NFA(const std::set<int>& q, const std::map<int, std::map<char, std::set<int>>>& transitions,
	int q0, int f)
	: q(q), transitions(transitions), q0(q0), f(f)
{
	// ensure all states are present in the transition map, this is an assumption
	// some of the later methods make
	for (int state : q)
	{
		if (transitions.find(state) == transitions.end())
		{
			this->transitions.emplace(std::make_pair(state, std::map<char, std::set<int>>()));
		}
	}
}

int NFA::NumStates()
{
	return q.size();
}

std::set<int> NFA::EpsilonClosure(const std::set<int>& starts)
{
	std::set<int> ec;

	// maintain a stack of the chain of states we are searching down,
	// similar to DFS
	std::stack<int> stack;

	for (int startState : starts)	// for each start state
	{
		stack.push(startState);
		while (!stack.empty())		// until we have completed the traversal and returned to the start state
		{
			int state = stack.top(); // arrive at the next state in the epsilon closure
			stack.pop();

			ec.insert(state);		// insert this state into the epsilon closure

			auto epsilonTransitionIt = (std::find_if(transitions[state].begin(), transitions[state].end(), [&](auto& v) {
				return v.first == '\0';
			}));					// find the map containing the epsilon transitions for this state
			if (epsilonTransitionIt != transitions[state].end())
			{
				const std::set<int>& epsilonTransitions = epsilonTransitionIt->second;
				
				for (int s : epsilonTransitions) // for each state that can be reached with an epsilon arrow	
				{
					if (ec.find(s) == ec.end())  // add that state to the stack of states to be inspected later
					{
						stack.push(s);
					}
				}
			}
		}
	}

	return ec;
}

DFA NFA::ConvertToDFA()
{
	// variables to make up the output DFA
	std::set<int> dfaQ;
	std::map<int, std::map<char, int>> dfaTransitions;
	int dfaQ0 = 0;
	std::set<int> dfaF;

	std::vector<std::pair< std::set<int>, std::map<char, std::set<int>>>> subsetConstructionTable;

	// create the first row of the subset construction table, the epsilon closure of the
	// start state
	std::set<int> start = EpsilonClosure({ q0 });
	subsetConstructionTable.push_back(std::make_pair(start, std::map<char, std::set<int>>()));

	// loop over all elements of subset construction table
	int subsetIndex = 0;
	while (subsetIndex < subsetConstructionTable.size())
	{
		auto& it = *(subsetConstructionTable.begin() + subsetIndex);
		// for each state in this row header
		for (int state : it.first)
		{
			// for each input arrow defined for this state
			for (auto& transitionIt : transitions[state])
			{
				char input = transitionIt.first;

				// ignore if this input is epsilon
				if (input == '\0')
				{
					continue;
				}

				// add its destinations to that inputs column in the subset construction table
				if (it.second.find(input) == it.second.end())
				{
					it.second.emplace(std::make_pair(input, std::set<int>()));
				}
				
				std::set<int> setUnion;
				std::set_union(
					it.second[input].begin(),
					it.second[input].end(),
					transitions[state][input].begin(),
					transitions[state][input].end(),
					std::inserter(setUnion, setUnion.begin()));
				it.second[input] = setUnion;
			}

			// calculate epsilon closure for each input
			for (auto& input : it.second)
			{
				input.second = EpsilonClosure(input.second);
			}
		}

		std::vector<std::pair< std::set<int>, std::map<char, std::set<int>>>> addToConstruction;

		// add all new state combinations to the table
		for (auto& row : it.second)
		{
			// check if the subset construction table already contains the combination
			bool foundAlready = std::find_if(subsetConstructionTable.begin(), subsetConstructionTable.end(), [&](auto& val) {
				return val.first == row.second;
			}) != subsetConstructionTable.end();

			if (!foundAlready)
			{
				addToConstruction.emplace_back(std::make_pair(row.second, std::map<char, std::set<int>>()));
			}
		}

		subsetConstructionTable.insert(subsetConstructionTable.end(), addToConstruction.begin(), addToConstruction.end());
		subsetIndex++;
	}

	// construct the DFA
	for (int i = 0; i < subsetConstructionTable.size(); ++i)
	{
		// add the state
		dfaQ.insert(i);

		// add the transitions
		for (auto& mapIt : subsetConstructionTable[i].second)
		{
			int idx = std::find_if(subsetConstructionTable.begin(), subsetConstructionTable.end(), [&](auto& val) {
				return val.first == mapIt.second;
			}) - subsetConstructionTable.begin();

			if (dfaTransitions.find(i) == dfaTransitions.end())
			{
				dfaTransitions.emplace(std::make_pair(i, std::map<char, int>()));
			}
			dfaTransitions[i].emplace(std::make_pair(mapIt.first, idx));
		}

		// check if this is a final state
		if (subsetConstructionTable[i].first.find(f) != subsetConstructionTable[i].first.end())
		{
			dfaF.insert(i);
		}
	}

	return DFA(dfaQ, dfaTransitions, dfaQ0, dfaF);
}

NFA NFA::GenerateSingle(char input)
{
	std::set<int> q = { 0, 1 };
	std::map<char, std::set<int>> transition = {
		{ input, { 1 }}
	};
	std::map<int, std::map<char, std::set<int>>> transitions = {
		{ 0, transition }
	};

	int q0 = 0;
	int f = 1;

	return NFA(q, transitions, q0, f);
}

NFA NFA::GenerateEmpty()
{
	std::set<int> q = { 0, 1 };
	std::map<char, std::set<int>> transition = {
		{ '\0', { 1 }}
	};
	std::map<int, std::map<char, std::set<int>>> transitions = {
		{ 0, transition }
	};

	int q0 = 0;
	int f = 1;

	return NFA(q, transitions, q0, f);
}

void NFA::RemapTransitions(const NFA& n, int base, std::map<int, int>& outMap, std::map<int, std::map<char, std::set<int>>>& outTransitions)
{
	// remap states from base to base+n
	int count = 0;
	for (int state : n.q)
	{
		outMap.emplace(std::make_pair(state, count + base));
		count++;
	}

	// loop through each transition in the nfa
	for (auto& keyVal1 : n.transitions)
	{
		// translate the start state
		int startState = keyVal1.first;
		int newStartState = outMap[startState];
		outTransitions.emplace(std::make_pair(newStartState, std::map<char, std::set<int>>()));

		// loop through each input for this state
		for (auto& transitionKeyVal : n.transitions.at(startState))
		{
			char input = transitionKeyVal.first;
			const std::set<int>& destinations = transitionKeyVal.second;
			outTransitions[newStartState].emplace(std::make_pair(input, std::set<int>()));

			// translate each of the inputs destination states
			std::transform(
				destinations.begin(),
				destinations.end(),
				std::inserter(outTransitions[newStartState][input], outTransitions[newStartState][input].begin()),
				[&](int val) {
					return outMap[val];
				});
		}
	}
}

std::map<int, std::map<char, std::set<int>>> NFA::CombineTransitions(const NFA& n1, const NFA& n2, std::map<int, int>& outM1, std::map<int, int>& outM2)
{
	std::map<int, std::map<char, std::set<int>>> newTransitions;

	RemapTransitions(n1, 1, outM1, newTransitions);
	RemapTransitions(n2, 1 + n1.q.size(), outM2, newTransitions);

	return newTransitions;
}

NFA NFA::Union(const NFA& n1, const NFA& n2)
{
	std::map<int, int> n1Map;
	std::map<int, int> n2Map;
	std::map<int, std::map<char, std::set<int>>> transitions = CombineTransitions(n1, n2, n1Map, n2Map);

	int q0 = 0;

	// n1 states remapped from 1-n, n2 remapped from n+1-m, new final state is one past that
	int f = n1.q.size() + n2.q.size() + 1;

	// add epsilon arrow from q0 to start state of n1 and n2
	std::map<char, std::set<int>> epsilonStartArrows = {
		{ '\0', { n1Map[n1.q0], n2Map[n2.q0] } }
	};
	transitions.emplace(std::make_pair(q0, epsilonStartArrows));

	// add epsilon transition from final state of n1 to f
	if (transitions.find(n1Map[n1.f]) == transitions.end())
	{
		transitions.emplace(std::make_pair(n1Map[n1.f], std::map<char, std::set<int>>()));
	}
	if (transitions[n1Map[n1.f]].find('\0') == transitions[n1Map[n1.f]].end())
	{
		transitions[n1Map[n1.f]].emplace(std::make_pair('\0', std::set<int>()));
	}
	transitions[n1Map[n1.f]]['\0'].insert(f);

	// add epsilon transition from final state of n2 to f
	if (transitions.find(n2Map[n2.f]) == transitions.end())
	{
		transitions.emplace(std::make_pair(n2Map[n2.f], std::map<char, std::set<int>>()));
	}
	if (transitions[n2Map[n2.f]].find('\0') == transitions[n2Map[n2.f]].end())
	{
		transitions[n2Map[n2.f]].emplace(std::make_pair('\0', std::set<int>()));
	}
	transitions[n2Map[n2.f]]['\0'].insert(f);

	// set up the states of the new NFA
	std::set<int> q;
	for (int i = 0; i <= f; ++i)
	{
		q.insert(i);
	}

	return NFA(q, transitions, q0, f);
}

NFA NFA::Concatenate(const NFA& n1, const NFA& n2)
{
	std::map<int, int> n1Map;
	std::map<int, int> n2Map;
	std::map<int, std::map<char, std::set<int>>> transitions = CombineTransitions(n1, n2, n1Map, n2Map);

	// start state of n1 is new start state
	int q0 = n1Map[n1.q0];

	// add epsilon transition from final state of n1 to start state of n2
	if (transitions.find(n1Map[n1.f]) == transitions.end())
	{
		transitions.emplace(std::make_pair(n1Map[n1.f], std::map<char, std::set<int>>()));
	}
	if (transitions[n1Map[n1.f]].find('\0') == transitions[n1Map[n1.f]].end())
	{
		transitions[n1Map[n1.f]].emplace(std::make_pair('\0', std::set<int>()));
	}
	transitions[n1Map[n1.f]]['\0'].insert(n2Map[n2.q0]);

	// final state of n2 is new final state
	int f = n2Map[n2.f];

	// set up the states of the new NFA
	std::set<int> q;
	for (int i = 1; i <= f; ++i)
	{
		q.insert(i);
	}

	return NFA(q, transitions, q0, f);
}

NFA NFA::KleeneStar(const NFA& n)
{
	// remap states starting at 1
	std::map<int, int> map;
	std::map<int, std::map<char, std::set<int>>> transitions;
	RemapTransitions(n, 1, map, transitions);

	int q0 = 0;
	int f = n.q.size();

	// add epsilon arrow from q0 to n start, and from q0 to f
	std::map<char, std::set<int>> q0Transitions = {
		{ '\0', { map[n.q0], f } }
	};
	transitions.emplace(std::make_pair(q0, q0Transitions));

	// add epsilon arrows from n end to n start and f
	if (transitions.find(map[n.f]) == transitions.end()) 
	{
		transitions.emplace(std::make_pair(map[n.f], std::map<char, std::set<int>>()));
	}
	if (transitions[map[n.f]].find('\0') == transitions[map[n.f]].end())
	{
		transitions[map[n.f]].emplace(std::make_pair('\0', std::set<int>()));
	}
	transitions[map[n.f]]['\0'].insert({map[n.q0], f});

	// set up the states of the new NFA
	std::set<int> q;
	for (int i = 0; i <= f; ++i)
	{
		q.insert(i);
	}

	return NFA(q, transitions, q0, f);
}

NFA NFA::Optional(const NFA& n)
{
	// clone transitions without remapping
	std::map<int, int> map;
	std::map<int, std::map<char, std::set<int>>> transitions;
	RemapTransitions(n, 0, map, transitions);

	int q0 = 0;
	int f = map[n.f];

	// add epsilon arrow from start state to end state
	if (transitions.find(0) == transitions.end())
	{
		transitions.emplace(std::make_pair(0, std::map<char, std::set<int>>()));
	}
	if (transitions[0].find('\0') == transitions[0].end())
	{
		transitions[0].emplace(std::make_pair('\0', std::set<int>()));
	}
	transitions[0]['\0'].insert(map[n.f]);

	// set up the states of the new NFA
	std::set<int> q;
	for (int i = 0; i <= f; ++i)
	{
		q.insert(i);
	}

	return NFA(q, transitions, q0, f);
}

NFA NFA::OneOrMore(const NFA& n)
{
	// return the nfa contatenated with its kleene star
	return Concatenate(n, KleeneStar(n));
}

std::map<int, std::map<char, std::set<int>>> NFA::MakeTransitionMap(const std::vector<std::tuple<int, char, std::set<int>>>& easyList)
{
	std::map<int, std::map<char, std::set<int>>> transitions;

	for (auto& it : easyList)
	{

		if (transitions.find(std::get<0>(it)) == transitions.end())
		{
			transitions.emplace(std::get<0>(it), std::map<char, std::set<int>>());
		}
		transitions[std::get<0>(it)].emplace(std::make_pair(std::get<1>(it), std::get<2>(it)));
	}

	return transitions;
}