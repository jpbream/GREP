#include "pch.h"
#include "CppUnitTest.h"
#include "../GREP/Regex.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GREPTest
{
	TEST_CLASS(RegexTest)
	{
	public:

		TEST_METHOD(TestRegex)
		{
			Regex regex = Regex::Parse("asdf.a");
			std::vector<std::pair<std::string, int>> matches = regex.Match("asdfxa");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("asdfxa"), matches[0].first);

			matches = regex.Match("asd");

			Assert::AreEqual(0, (int)matches.size());
		}

		TEST_METHOD(TestRegexUnion)
		{
			Regex regex = Regex::Parse("asdf|fdsa");
			std::vector<std::pair<std::string, int>> matches = regex.Match("asdf");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("asdf"), matches[0].first);

			matches = regex.Match("fdsa");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("fdsa"), matches[0].first);

			matches = regex.Match("asd");

			Assert::AreEqual(0, (int)matches.size());
		}

		TEST_METHOD(TestRegexMultiUnion)
		{
			Regex regex = Regex::Parse("^asdf$|fdsa|qwer");
			std::vector<std::pair<std::string, int>> matches = regex.Match("asdf");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("asdf"), matches[0].first);

			matches = regex.Match("fdsa");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("fdsa"), matches[0].first);

			matches = regex.Match("qqwery");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("qwer"), matches[0].first);

			matches = regex.Match("asd");

			Assert::AreEqual(0, (int)matches.size());
		}

		TEST_METHOD(TestRegexKleeneStar)
		{
			Regex regex = Regex::Parse("ab*c");
			std::vector<std::pair<std::string, int>> matches = regex.Match("abbbc");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("abbbc"), matches[0].first);

			matches = regex.Match("ac");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("ac"), matches[0].first);

			matches = regex.Match("abc");

			Assert::AreEqual(1, (int)matches.size());
			Assert::AreEqual(std::string("abc"), matches[0].first);

			matches = regex.Match("ab");

			Assert::AreEqual(0, (int)matches.size());
		}

		TEST_METHOD(TestRegexKleeneStarUnion)
		{
			Regex regex = Regex::Parse("ab*c|qwer");
			std::vector<std::pair<std::string, int>> matches = regex.Match("abbbcxxxxqwerabc");

			Assert::AreEqual(3, (int)matches.size());
			Assert::AreEqual(std::string("abbbc"), matches[0].first);
			Assert::AreEqual(std::string("qwer"), matches[1].first);
			Assert::AreEqual(std::string("abc"), matches[2].first);
		}
	};
}
