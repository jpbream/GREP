#pragma once
#include <set>
#include <map>
#include <string>
#include <vector>

class DFA
{
private:
	std::set<int> q;
	std::map<int, std::map<char, int>> transitions;
	int q0;
	std::set<int> f;

	int currentState = -1;

	// switched to true when dfa enters an unrecoverable error state
	bool inErrorState = false;

public:

	/// <summary>
	/// Constructs a new DFA
	/// </summary>
	/// <param name="q">Set of states in the DFA</param>
	/// <param name="transitions">The transition map. Double map that should map state+input to the next state.</param>
	/// <param name="q0">The start state</param>
	/// <param name="f">The set of final states</param>
	DFA(const std::set<int>& q, const std::map<int, std::map<char, int>>& transitions,
		int q0, const std::set<int>& f);

	/// <summary>
	/// Returns the number of unique states in this DFA
	/// </summary>
	/// <returns></returns>
	int NumStates();

	/// <summary>
	/// Starts a simulation. After calling, the DFA will be ready to accept input
	/// </summary>
	void BeginSimulation();

	/// <summary>
	/// Sends one character of input to the DFA for processing
	/// </summary>
	/// <param name="input"></param>
	void OnNext(char input);

	/// <summary>
	/// Sends a string of input to the DFA for processing.
	/// </summary>
	/// <param name="input"></param>
	void OnNextAll(const std::string& input);

	/// <summary>
	/// Returns true if the DFA is currently in a final state
	/// </summary>
	/// <returns></returns>
	bool HasAccepted();

	/// <summary>
	/// Returns true if the DFA has encountered an input for which no transition was defined. This puts
	/// the DFA in an unrecoverable error state until the simulation is ended.
	/// </summary>
	/// <returns></returns>
	bool HasFailed();

	/// <summary>
	/// Ends the simulation.
	/// </summary>
	/// <returns>True if the DFA is in an accept state</returns>
	bool EndSimulation();

	/// <summary>
	/// Generates a DFA that accepts the empty string
	/// </summary>
	/// <returns></returns>
	static DFA GenerateEmpty();

	/// <summary>
	/// Creates a transition map.
	/// </summary>
	/// <param name="easyList">A list of tuples. The first element of the tuple is the current state. The next 
	/// element is the input. The final element is the state to transition to if the input is received at the
	/// current state. It is usually easier to write an initializer list for this data structure</param>
	/// <returns>The transition list converted to a data structure the DFA expects</returns>
	static std::map<int, std::map<char, int>> MakeTransitionMap(const std::vector<std::tuple<int, char, int>>& easyList);
};
