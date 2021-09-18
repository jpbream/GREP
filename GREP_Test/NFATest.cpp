#include "pch.h"
#include "CppUnitTest.h"
#include "../GREP/NFA.h"
#include "../GREP/DFA.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GREPTest
{
	TEST_CLASS(NFATest)
	{
	public:
		TEST_METHOD(TestNFAToDFA)
		{
			std::set<int> q = { 1, 2, 3, 4 };
			int q0 = 1;
			int f = 4;

			std::vector<std::tuple<int, char, std::set<int>>> easyTransitions = {
				{ 1, '0', {1, 2} },
				{ 1, '1', {1} },

				{ 2, '0', {3} },
				{ 2, '\0', {3} },

				{ 3, '0', {4} },

				{ 4, '0', {4} },
				{ 4, '1', {4} },
			};

			NFA nfa(q, NFA::MakeTransitionMap(easyTransitions), q0, f);
			DFA dfa = nfa.ConvertToDFA();

			Assert::AreEqual(4, dfa.NumStates());

			std::string input1 = "100110010111000110101010001";
			std::string input2 = "1010101011010110111110";

			dfa.BeginSimulation();
			dfa.OnNextAll(input1);
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);
		

			dfa.BeginSimulation();
			dfa.OnNextAll(input2);
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(false, result2);
		}

		TEST_METHOD(TestSingle)
		{
			NFA single = NFA::GenerateSingle('b');
			DFA singleDFA = single.ConvertToDFA();
			singleDFA.BeginSimulation();
			singleDFA.OnNext('b');
			bool result1 = singleDFA.EndSimulation();

			Assert::AreEqual(true, result1);

			singleDFA.BeginSimulation();
			singleDFA.OnNext('c');
			bool result2 = singleDFA.EndSimulation();

			Assert::AreEqual(false, result2);
		}

		TEST_METHOD(TestUnion)
		{
			NFA a = NFA::GenerateSingle('a');
			NFA b = NFA::GenerateSingle('b');
			NFA both = NFA::Union(a, b);

			Assert::AreEqual(6, both.NumStates());

			DFA dfa = both.ConvertToDFA();
			dfa.BeginSimulation();
			dfa.OnNext('a');
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNext('b');
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(true, result2);

			dfa.BeginSimulation();
			dfa.OnNext('c');
			bool result3 = dfa.EndSimulation();

			Assert::AreEqual(false, result3);

			NFA c = NFA::GenerateSingle('c');
			NFA three = NFA::Union(both, c);
			DFA dfaThree = three.ConvertToDFA();

			dfaThree.BeginSimulation();
			dfaThree.OnNext('c');
			bool result4 = dfaThree.EndSimulation();

			Assert::AreEqual(true, result4);
		}

		TEST_METHOD(TestConcatenate)
		{
			NFA a = NFA::GenerateSingle('a');
			NFA b = NFA::GenerateSingle('b');
			NFA both = NFA::Concatenate(a, b);

			Assert::AreEqual(4, both.NumStates());

			DFA dfa = both.ConvertToDFA();
			dfa.BeginSimulation();
			dfa.OnNextAll("ab");
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNext('a');
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(false, result2);

			dfa.BeginSimulation();
			dfa.OnNextAll("ba");
			bool result3 = dfa.EndSimulation();

			Assert::AreEqual(false, result3);

			NFA c = NFA::GenerateSingle('c');
			NFA three = NFA::Concatenate(both, c);
			DFA dfaThree = three.ConvertToDFA();

			dfaThree.BeginSimulation();
			dfaThree.OnNextAll("abc");
			bool result4 = dfaThree.EndSimulation();

			Assert::AreEqual(true, result4);

			dfaThree.BeginSimulation();
			dfaThree.OnNextAll("ac");
			bool result5 = dfaThree.EndSimulation();

			Assert::AreEqual(false, result5);
		}

		TEST_METHOD(TestKleeneStar)
		{
			NFA a = NFA::GenerateSingle('a');
			NFA aStar = NFA::KleeneStar(a);
			DFA dfa = aStar.ConvertToDFA();

			std::string input1 = "aaaaaaaaaaa";
			std::string input2 = "a";
			std::string input3 = "";
			std::string input4 = "b";

			dfa.BeginSimulation();
			dfa.OnNextAll(input1);
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNextAll(input2);
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(true, result2);

			dfa.BeginSimulation();
			dfa.OnNextAll(input3);
			bool result3 = dfa.EndSimulation();

			Assert::AreEqual(true, result3);

			dfa.BeginSimulation();
			dfa.OnNextAll(input4);
			bool result4 = dfa.EndSimulation();

			Assert::AreEqual(false, result4);

			NFA b = NFA::GenerateSingle('b');
			NFA both = NFA::Concatenate(a, b);
			NFA bothStar = NFA::KleeneStar(both);
			DFA bothDfa = bothStar.ConvertToDFA();

			bothDfa.BeginSimulation();
			bothDfa.OnNextAll("abababab");
			bool result5 = bothDfa.EndSimulation();

			Assert::AreEqual(true, result5);
		}

		TEST_METHOD(TestOptional)
		{
			NFA a = NFA::GenerateSingle('a');
			NFA aOptional = NFA::Optional(a);
			DFA dfa = aOptional.ConvertToDFA();

			dfa.BeginSimulation();
			dfa.OnNext('a');
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(true, result2);

			dfa.BeginSimulation();
			dfa.OnNext('b');
			bool result3 = dfa.EndSimulation();

			Assert::AreEqual(false, result3);
		}

		TEST_METHOD(TestOneOrMore)
		{
			NFA a = NFA::Concatenate(NFA::GenerateSingle('a'), NFA::GenerateSingle('b'));
			NFA aOptional = NFA::OneOrMore(a);
			DFA dfa = aOptional.ConvertToDFA();

			dfa.BeginSimulation();
			dfa.OnNextAll("ab");
			bool result1 = dfa.EndSimulation();

			Assert::AreEqual(true, result1);

			dfa.BeginSimulation();
			dfa.OnNextAll("ababababab");
			bool result2 = dfa.EndSimulation();

			Assert::AreEqual(true, result2);

			dfa.BeginSimulation();
			dfa.OnNextAll("aa");
			bool result3 = dfa.EndSimulation();

			Assert::AreEqual(false, result3);
		}
	};
}
