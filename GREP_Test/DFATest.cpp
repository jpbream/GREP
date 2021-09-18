#include "pch.h"
#include "CppUnitTest.h"
#include "../GREP/DFA.h"
#include "../GREP/Regex.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GREPTest
{
	TEST_CLASS(DFATest)
	{
	public:
		
		TEST_METHOD(TestDFA1)
		{
			std::set<int> q = { 1, 2, 3, 4 };
			int q0 = 1;
			std::set<int> f = { 4 };

			std::vector<std::tuple<int, char, int>> easyTransitions = {
				{ 1, '0', 1 },
				{ 1, '@', 1 },
				{ 1, '%', 2 },

				{ 2, '0', 3},
				{ 2, '@', 1},
				{ 2, '%', 2},

				{ 3, '0', 1},
				{ 3, '@', 1},
				{ 3, '%', 4},

				{ 4, '0', 4},
				{ 4, '@', 4},
				{ 4, '%', 4},
			};

			DFA dfa(q, DFA::MakeTransitionMap(easyTransitions), q0, f);
			std::string input1 = "000%%0@%0%@0%00@";
			std::string input2 = "0%@%@0%0@0%@%0";

			dfa.BeginSimulation();
			dfa.OnNextAll(input1);
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNextAll(input2);
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(false, result2);
		}

		TEST_METHOD(TestAnyTransition)
		{
			std::set<int> q = { 1, 2, 3 };
			int q0 = 1;
			std::set<int> f = { 3 };

			std::vector<std::tuple<int, char, int>> easyTransitions = {
				{ 1, '0', 1 },
				{ 1, Regex::ANY, 1 },
				{ 1, '%', 2 },

				{ 2, Regex::ANY, 3},

				{ 3, Regex::ANY, 3},
			};

			DFA dfa(q, DFA::MakeTransitionMap(easyTransitions), q0, f);
			std::string input1 = "ss%asdfasdf";
			std::string input2 = "000";

			dfa.BeginSimulation();
			dfa.OnNextAll(input1);
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNextAll(input2);
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(false, result2);
		}
	};
}
