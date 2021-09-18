#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Regex.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: grep <regex> <file>" << std::endl;
		return 0;
	}

	std::string regex = argv[1];
	std::ifstream file(argv[2]);
	
	Regex r = Regex::Parse(regex);

	std::string input;
	while (std::getline(file, input))
	{
		std::vector<std::pair<std::string, int>> matches = r.Match(input);
		if (matches.size() > 0)
		{
			// capitalize occurrences of the matches in the line
			for (auto& match : matches)
			{
				// convert matches to uppercase
				int idx = match.second;
				std::string& text = match.first;
				std::transform(
					input.begin() + idx, 
					input.begin() + idx + text.length(),
					input.begin() + idx,
					std::toupper);
			}

			std::cout << input << "\n";
		}
	}

	return 0;
}