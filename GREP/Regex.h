#pragma once
#include "NFA.h"
#include "DFA.h"
#include <string>
#include <vector>

class Regex
{
private:
	
	NFA nfa;
	DFA dfa;

	static NFA ParseExpression(const std::string& text, int& outLen);
	static NFA CheckOperators(const NFA& nfa, char nextChar, int& outNumSkipped);

public:

	static const char LINE_START = 128;
	static const char LINE_END = 129;
	static const char ANY = 130;

	Regex();

	/// <summary>
	/// Matches a string using this regular expression.
	/// </summary>
	/// <param name="text">The string to match</param>
	/// <returns>A list of matches. Each match contains the string
	/// that was matched and its index into the input string.</returns>
	std::vector<std::pair<std::string, int>> Match(const std::string& text);

	/// <summary>
	/// Creates a Regex object from a regular expression. The supported regular expression
	/// operations are parenthesis, |, *, +, ?, ^, $, and .
	/// </summary>
	/// <param name="regex"></param>
	/// <returns></returns>
	static Regex Parse(const std::string& regex);
};

