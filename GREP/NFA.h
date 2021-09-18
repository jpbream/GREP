#pragma once
#include "DFA.h"

#include <set>
#include <map>
#include <string>

class NFA
{
private:
	std::set<int> q;
	std::map<int, std::map<char, std::set<int>>> transitions;
	int q0;
	int f;

	/// <summary>
	/// Calculates the epsilon closure of a set of states. This is the set of states that can
	/// be reached from any of the start states if only epsilon arrows are taken.
	/// </summary>
	/// <param name="starts">The set of start states</param>
	/// <returns></returns>
	std::set<int> EpsilonClosure(const std::set<int>& starts);

	/// <summary>
	/// Copies the transition map of an NFA, but remaps the states to new integer range.
	/// </summary>
	/// <param name="n">The NFA whose transition map to copy.</param>
	/// <param name="base">The base of the integer range to remap the staets to. The
	/// limit of this range will be base + n.num_states</param>
	/// <param name="outMap">Output parameter that will contain a mapping of states on the input
	/// NFA to thier remapped state in the copy.</param>
	/// <param name="outTransitions">Output parameter that will contain the remapped copy of the transition
	/// map on the original NFA.</param>
	static void RemapTransitions(const NFA& n, int base, std::map<int, int>& outMap, std::map<int, std::map<char, std::set<int>>>& outTransitions);

	/// <summary>
	/// Combines the transition maps of two NFA's into a new one, and remaps their states to ensure
	/// no duplicate states.
	/// </summary>
	/// <param name="n1">First of two NFA's to merge</param>
	/// <param name="n2">Second of two NFA's to merge</param>
	/// <param name="outM1">Output parameter that will contain a mapping of n1 states to its remapped states</param>
	/// <param name="outM2">Output parameter that will contain a mapping of n2 states to its remapped states</param>
	/// <returns>The transition maps of the two input NFA's combined and remapped</returns>
	static std::map<int, std::map<char, std::set<int>>> CombineTransitions(const NFA& n1, const NFA& n2, std::map<int, int>& outM1, std::map<int, int>& outM2);

public:

	/// <summary>
	/// Constructs a new NFA
	/// </summary>
	/// <param name="q">Set of states in the NFA</param>
	/// <param name="transitions">The transition map. Double map that should map state+input to the set of states that
	/// can be reached with that input combination.</param>
	/// <param name="q0">The start state</param>
	/// <param name="f">The final state. Only one is allowed.</param>
	NFA(const std::set<int>& q, const std::map<int, std::map<char, std::set<int>>>& transitions,
		int q0, int f);

	/// <summary>
	/// Converts the NFA to an equivalent DFA using the subset construction algorithm
	/// </summary>
	/// <returns></returns>
	DFA ConvertToDFA();

	/// <summary>
	/// Returns the number of unique states in this NFA
	/// </summary>
	/// <returns></returns>
	int NumStates();

	/// <summary>
	/// Generates an NFA that accepts a single character
	/// </summary>
	/// <param name="input">The character to accept</param>
	/// <returns></returns>
	static NFA GenerateSingle(char input);

	/// <summary>
	/// Generates an NFA that accepts the empty string
	/// </summary>
	/// <returns></returns>
	static NFA GenerateEmpty();

	/// <summary>
	/// Generates an NFA that accepts the input of n1, or n2
	/// </summary>
	/// <param name="n1"></param>
	/// <param name="n2"></param>
	/// <returns></returns>
	static NFA Union(const NFA& n1, const NFA& n2);

	/// <summary>
	/// Generates an NFA that accepts the input of n1 followed by the
	/// input of n2
	/// </summary>
	/// <param name="n1"></param>
	/// <param name="n2"></param>
	/// <returns></returns>
	static NFA Concatenate(const NFA& n1, const NFA& n2);

	/// <summary>
	/// Generates an NFA that accepts the input of n repeated 0
	/// or more times
	/// </summary>
	/// <param name="n1"></param>
	/// <param name="n2"></param>
	/// <returns></returns>
	static NFA KleeneStar(const NFA& n);

	/// <summary>
	/// Generates an NFA that accepts the input of n repeated 0
	/// or one times
	/// </summary>
	/// <param name="n1"></param>
	/// <param name="n2"></param>
	/// <returns></returns>
	static NFA Optional(const NFA& n);

	/// <summary>
	/// Generates an NFA that accepts the input of n repeated one
	/// or more times.
	/// </summary>
	/// <param name="n1"></param>
	/// <param name="n2"></param>
	/// <returns></returns>
	static NFA OneOrMore(const NFA& n);

	/// <summary>
	/// Creates a transition map.
	/// </summary>
	/// <param name="easyList">A list of tuples. The first element of the tuple is the current state. The next 
	/// element is the input. The final element is the set of states that could be transitioned to if the input is 
	/// received at the current state. It is usually easier to write an initializer list for this data structure</param>
	/// <returns>The transition list converted to a data structure the NFA expects</returns>
	static std::map<int, std::map<char, std::set<int>>> MakeTransitionMap(const std::vector<std::tuple<int, char, std::set<int>>>& easyList);
};
