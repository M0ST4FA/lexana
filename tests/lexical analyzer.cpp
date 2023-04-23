#include "gtest/gtest.h"
#include "./fsm.h"
#include "./universal.h"
#include "../LexicalAnalyzer.h"

using m0st4fa::LexicalAnalyzer;

class LATests : public ::testing::Test, public FSMSharedInfo, public LASharedInfo {

protected:
	using enum m0st4fa::FSM_MODE;
	using enum Terminal;
	typedef LexicalAnalyzer<TokenType, m0st4fa::FSMTable, std::string_view> LexicalAnalyzerType;
	using Result = m0st4fa::LexicalAnalyzerResult<TokenType, std::string_view>;

	void testLAResultPositive(const Result& res, bool foundToken = false, const TokenType& token = TokenType{}, m0st4fa::Indecies indecies = m0st4fa::Indecies{}, size_t lineNumber = 0, std::source_location srcLoc = std::source_location::current()) const {

		std::string errmsg = std::format(ANSI_ERR_COLOR"{}" ANSI_RESET_ALL, m0st4fa::toString(srcLoc, true));
		/*if (not res.input) {
			errmsg += " input string -> " + std::string(input);
		};*/

		ASSERT_EQ(res.foundToken, foundToken) << errmsg;
		ASSERT_EQ(res.indecies, indecies) << errmsg << std::format("\n{}", res.indecies.toString());;
		ASSERT_EQ(res.token, res.token) << errmsg;
		ASSERT_EQ(res.lineNumber, lineNumber) << errmsg;

	};

	void testLAResultNegative(const Result& res, bool foundToken = false, const TokenType& token = TokenType{}, m0st4fa::Indecies indecies = m0st4fa::Indecies{}, size_t lineNumber = 0, std::source_location srcLoc = std::source_location::current()) const {

		std::string errmsg = std::format(ANSI_ERR_COLOR"{}" ANSI_RESET_ALL, m0st4fa::toString(srcLoc, true));
		/*if (not res.input) {
			errmsg += " input string -> " + std::string(input);
		};*/

		bool condition = (res.foundToken == foundToken) or (res.indecies == indecies) or (res.token == token) or (res.lineNumber == lineNumber);

		ASSERT_FALSE(not condition) << errmsg << std::format("\nres.indecies: {}, indecies: {}\n res.token: {}, input token: {}\n res.lineNumber: {}, input lineNumber: {}", res.indecies.toString(), indecies.toString(), res.token.toString(), token.toString(), res.lineNumber, lineNumber);
		/*ASSERT_EQ(res.foundToken, !foundToken) << errmsg;
		ASSERT_FALSE(res.indecies == indecies) << errmsg << std::format("\n{}", res.indecies.toString());
		ASSERT_FALSE(res.token == token) << errmsg << std::format("\n{} == {}", res.token.toString(), res.token.toString());
		ASSERT_FALSE(res.lineNumber == lineNumber) << errmsg << std::format("{} == {}", res.lineNumber, lineNumber);*/
	};
};


TEST_F(LATests, id_eq_num) {

	// Data structures	
	typename FSMTableType table{};
	this->initTranFn_id_eq_num(table);
	typename TranFn tranFn{ table };
	DFAType testFSM{ {2, 3, 4}, tranFn };


	// strings
	std::string_view str1 = "x = 10";
	std::string_view str2 = "newid \n newidverylong \n = \n 555";
	std::string_view str3 = "x = 10\ny = 50\nz = x + y";
	std::string_view str4 = "aabb\naabbb\naabb\nabb";

	// POSITIVE TESTS
	{
		SCOPED_TRACE("LA POSITIVE TESTS");

		LexicalAnalyzerType la1{ testFSM, this->fact_id_eq_num, str1 };
		LexicalAnalyzerType la2{ testFSM, this->fact_id_eq_num, str2 };
		LexicalAnalyzerType la3{ testFSM, this->fact_id_eq_num, str3 };

		this->testLAResultPositive(la1.peak(), true, TokenType{ Terminal::T_ID, "x" }, { 0, 1 }, 0);
		this->testLAResultPositive(la1.getNextToken(), true, TokenType{ Terminal::T_ID, "x" }, { 0, 1 }, 0);

		this->testLAResultPositive(la1.peak(), true, TokenType{ T_EQUAL, "=" }, {2, 3}, 0);
		this->testLAResultPositive(la1.getNextToken(), true, TokenType{ T_EQUAL, "=" }, {2, 3}, 0);

		this->testLAResultPositive(la1.peak(), true, TokenType{ T_NUM, "10" }, { 4, 6 }, 0);
		this->testLAResultPositive(la1.getNextToken(), true, TokenType{ T_NUM, "10" }, { 4, 6 }, 0);

		this->testLAResultPositive(la2.peak(), true, TokenType{ Terminal::T_ID, "newid" }, { 0, 5 }, 0);
		this->testLAResultPositive(la2.getNextToken(), true, TokenType{ Terminal::T_ID, "newid" }, { 0, 5 }, 0);

		this->testLAResultPositive(la2.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_ID, "newidverylong" }, { 1, 14 }, 1);
		this->testLAResultPositive(la2.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_ID, "newidverylong" }, { 1, 14 }, 1);

		this->testLAResultPositive(la2.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ T_EQUAL, "=" }, { 1, 2 }, 2);
		this->testLAResultPositive(la2.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ T_EQUAL, "=" }, { 1, 2 }, 2);

		this->testLAResultPositive(la2.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ T_NUM, "555" }, { 1, 4 }, 3);
		this->testLAResultPositive(la2.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ T_NUM, "555" }, { 1, 4 }, 3);

	}


	// NEGATIVE TESTS
	{
		SCOPED_TRACE("LA NEGATIVE TESTS");

	}

}

TEST_F(LATests, ab) {

	// Data structures	
	typename FSMTableType table{};
	this->initTranFn_ab(table);
	typename TranFn tranFn{ table };
	DFAType testFSM{ {4}, tranFn };


	// strings
	std::string_view str1 = "aaabb";
	std::string_view str2 = "asbsaabbb\naabb";
	std::string_view str3 = "sabb";
	std::string_view str4 = "aabb\naabbb\naabb\nabb";

	// POSITIVE TESTS
	{
		SCOPED_TRACE("LA POSITIVE TESTS");

		LexicalAnalyzerType la1{ testFSM, this->fact_ab, str1 };
		LexicalAnalyzerType la2{ testFSM, this->fact_ab, str2.substr(4) };
		LexicalAnalyzerType la3{ testFSM, this->fact_ab, str4 };

		this->testLAResultPositive(la1.peak(), true, TokenType{ Terminal::T_A, "aaabb" }, { 0, 5 }, 0);
		this->testLAResultPositive(la1.getNextToken(), true, TokenType{ Terminal::T_A, "aaabb" }, { 0, 5 }, 0);

		this->testLAResultPositive(la1.peak(), false, TokenType{});
		this->testLAResultPositive(la1.getNextToken(), false, TokenType{});

		this->testLAResultPositive(la2.peak(), true, TokenType{ Terminal::T_A, "aabbb" }, { 0, 5 }, 0);
		this->testLAResultPositive(la2.getNextToken(), true, TokenType{ Terminal::T_A, "aabbb" }, { 0, 5 }, 0);

		this->testLAResultPositive(la2.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 1);
		this->testLAResultPositive(la2.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 1);

		this->testLAResultPositive(la3.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 0);
		this->testLAResultPositive(la3.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 0);

		this->testLAResultPositive(la3.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aaabb" }, { 0, 5 }, 1);
		this->testLAResultPositive(la3.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aaabb" }, { 0, 5 }, 1);

		this->testLAResultPositive(la3.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 2);
		this->testLAResultPositive(la3.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 4 }, 2);
	}
	

	// NEGATIVE TESTS
	{
		SCOPED_TRACE("LA NEGATIVE TESTS");

		LexicalAnalyzerType la1{ testFSM, this->fact_ab, str1 };
		LexicalAnalyzerType la2{ testFSM, this->fact_ab, str2.substr(4) };
		LexicalAnalyzerType la3{ testFSM, this->fact_ab, str4 };

		this->testLAResultNegative(la1.peak(), true, TokenType{ Terminal::T_A, "aaab" }, { 0, 4 }, 0);
		this->testLAResultNegative(la1.getNextToken(), true, TokenType{ Terminal::T_A, "aabb" }, { 0, 5 }, 0);

	}

}

TEST_F(LATests, a) {

	// Data structures	
	typename FSMTableType table{};
	this->initTranFn_a(table);
	typename TranFn tranFn{ table };
	DFAType testFSM{ {2}, tranFn };

	// STRINGS
	std::string_view str1 = "a";
	std::string_view str2 = "a\naa";
	std::string_view str3 = "babaa";
	
	LexicalAnalyzerType la1{ testFSM, this->fact_a, str1 };
	LexicalAnalyzerType la2{ testFSM, this->fact_a, str2 };

	// POSITIVE TESTS
	{
		SCOPED_TRACE("LA POSITIVE TESTS");

		this->testLAResultPositive(la1.peak(), true, TokenType{ Terminal::T_A, "a" }, { 0, 1 }, 0);
		this->testLAResultPositive(la1.getNextToken(), true, TokenType{ Terminal::T_A, "a" }, { 0, 1 }, 0);

		this->testLAResultPositive(la1.peak(), false, TokenType{});
		this->testLAResultPositive(la1.getNextToken(), false, TokenType{});

		this->testLAResultPositive(la2.peak(), true, TokenType{ Terminal::T_A, "a" }, { 0, 1 }, 0);
		this->testLAResultPositive(la2.getNextToken(), true, TokenType{ Terminal::T_A, "a" }, { 0, 1 }, 0);

		this->testLAResultPositive(la2.peak((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aa" }, { 0, 2 }, 1);
		this->testLAResultPositive(la2.getNextToken((size_t)m0st4fa::LA_FLAG::LAF_ALLOW_NEW_LINE), true, TokenType{ Terminal::T_A, "aa" }, { 0, 2 }, 1);
	}


	// NEGATIVE TESTS
	{
		SCOPED_TRACE("LA NEGATIVE TESTS");

	}
}