#include "Regex.h"
#include <string_view>

Regex::Regex()
	: nfa(NFA::GenerateEmpty()), dfa(DFA::GenerateEmpty())
{ }

NFA Regex::CheckOperators(const NFA& nfa, char nextChar, int& outNumSkipped)
{
	if (nextChar == '*')
	{
		outNumSkipped = 1;
		return NFA::KleeneStar(nfa);
	}
	else if (nextChar == '+')
	{
		outNumSkipped = 1;
		return NFA::OneOrMore(nfa);
	}
	else if (nextChar == '?')
	{
		outNumSkipped = 1;
		return NFA::Optional(nfa);
	}

	outNumSkipped = 0;
	return nfa;
}

NFA Regex::ParseExpression(const std::string& text, int& outLen)
{
	NFA nfa1 = NFA::GenerateEmpty();
	NFA nfa2 = NFA::GenerateEmpty();
	bool shouldUnion = false;

	NFA* currentNfa = &nfa1;
	NFA nextGroup = NFA::GenerateEmpty();

	int i = 0;
	while (i < text.size())
	{
		char input = text[i];

		if (input == '(')
		{
			// increment i past the open-paren
			++i;

			// recursively call ParseExpression on this new
			// parenthesis group
			int len;
			NFA output = ParseExpression(text.substr(i), len);

			// increment i past the expression and close-paren
			i += len + 1;

			// concatenate the parenthesis group to the current
			// expression, after checking for repetition operators
			int numSkipped;
			*currentNfa = NFA::Concatenate(
				*currentNfa, 
				CheckOperators(output, text[i], numSkipped));
			i += numSkipped;
		}
		else if (input == ')')
		{
			// recursive base case, end of parenthesis group
			break;
		}
		else if (input == '|')
		{
			// complete a union if one is pending
			if (shouldUnion)
			{
				nfa1 = NFA::Union(nfa1, nfa2);
				nfa2 = NFA::GenerateEmpty();
			}

			// swap the nfas
			currentNfa = &nfa2;
			shouldUnion = true;

			i++;
		}
		else
		{
			// this is the non-operator case, so adding
			// a character to match

			// check for special characters
			if (input == '^')
			{
				input = LINE_START;
			}
			else if (input == '$')
			{
				input = LINE_END;
			}
			else if (input == '.')
			{
				input = ANY;
			}
			else if (input == '\\')
			{
				i++;
				input = text[i];
			}

			// a regular character
			NFA single = NFA::GenerateSingle(input);

			++i;

			// concatenate this character to the expression
			// after checking for repetition operators
			int numSkipped;
			*currentNfa = NFA::Concatenate(
				*currentNfa, 
				CheckOperators(single, text[i], numSkipped));

			i += numSkipped;
		}
	}

	// returns the length of the parenthesis expression
	outLen = i;

	// complete a union if one is pending
	if (shouldUnion)
	{
		nfa1 = NFA::Union(nfa1, nfa2);
	}

	return nfa1;
}

Regex Regex::Parse(const std::string& regex)
{
	Regex r;

	int dummy;
	r.nfa = ParseExpression(regex, dummy);

	r.dfa = r.nfa.ConvertToDFA();

	return r;
}

std::vector<std::pair<std::string, int>> Regex::Match(const std::string& text)
{
	std::vector<std::pair<std::string, int>> matches;
	std::string fullText = (char)LINE_START + text + (char)LINE_END;

	for (int start = 0; start < fullText.size(); ++start)
	{
		dfa.BeginSimulation();

		int i;
		for (i = start; i < fullText.size(); ++i)
		{
			dfa.OnNext(fullText[i]);

			// 'failure' means dfa is in unrecoverable error state, abort now
			if (dfa.HasFailed())
			{
				break;
			}

			// if dfa is in accept state, log the match
			if (dfa.HasAccepted())
			{
				std::string_view match = fullText;
				match = match.substr(start, i - start + 1);

				if (match.size() > 0 && match[0] == (char)LINE_START)
				{
					match.remove_prefix(1);
				}
				if (match.size() > 0 && match[match.size() - 1] == (char)LINE_END)
				{
					match.remove_suffix(1);
				}

				if (match != "")
				{
					// subtract off one to cancel out the start-of-line character
					matches.push_back(std::make_pair(std::string(match), start - 1));
				}
			}
		}
		
		dfa.EndSimulation();
	}
	
	return matches;
}
