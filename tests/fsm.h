#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <map>

#include "fsm/FiniteStateMachine.h"
#include "fsm/DFA.h"
#include "utility/common.h"
#include "universal.h"

template<typename FSMType>
class FSMTests : public testing::Test, public FSMSharedInfo {
	using Base = FSMSharedInfo;

protected:
	//using FSMStateType = m0st4fa::fsm::FSMStateType;
	using FSMStateSetType = m0st4fa::fsm::FSMStateSetType;
	using TableType = m0st4fa::fsm::FSMTable;
	using TranFn = m0st4fa::fsm::TransFn<TableType>;
	using DFAType = m0st4fa::fsm::DeterFiniteAutomaton<TranFn>;
	using Result = m0st4fa::fsm::FSMResult;

	void SetUp() override {

	}

	void TearDown() override {

	}

public:

	void testFSMResultPositive(const Result& res, bool accepted, std::pair<size_t, size_t> indicies, std::source_location srcLoc = std::source_location::current()) const {
		std::string errmsg = std::format(ANSI_ERR_COLOR"{}" ANSI_RESET_ALL, m0st4fa::toString(srcLoc, true));
		if (not res.input.empty()) {
			errmsg += " input string -> " + std::string(res.input);
		};

		EXPECT_TRUE(res.accepted == accepted) << errmsg;
		EXPECT_EQ(res.indicies.start, indicies.first) << errmsg;
		EXPECT_EQ(res.indicies.end, indicies.second) << errmsg;

	}

	void testFSMResultNegative(const Result& res, bool accepted, std::pair<size_t, size_t> indicies, std::source_location srcLoc = std::source_location::current()) const {
		std::string errmsg = std::format(ANSI_ERR_COLOR"{}" ANSI_RESET_ALL, m0st4fa::toString(srcLoc, true));
		if (not res.input.empty()) {
			errmsg += " input string -> " + std::string(res.input);
		};

		bool condition = (res.accepted == accepted) and (res.indicies.start == indicies.first) and (res.indicies.end == indicies.second);

		EXPECT_FALSE(condition) << errmsg;
	/*	EXPECT_FALSE(res.indicies.start == indicies.first) << errmsg;
		EXPECT_FALSE(res.indicies.end == indicies.second) << errmsg;*/
	}

};

TYPED_TEST_SUITE_P(FSMTests);

template<typename T>
std::ostream& operator<<(std::ostream& os, const typename FSMTests<T>::Token token) {
	std::cout << std::format("Lexeme: {}", token.lexeme);
	return os;
};

TYPED_TEST_P(FSMTests, simulate) {

	using enum m0st4fa::fsm::FSM_MODE;

	// Data structures
	typename TestFixture::TranFn tranFn;
	this->Base::initTranFn_ab(tranFn);
	TypeParam testFSM{ {4}, tranFn };

	// strings
	std::string_view str1 = "baaabb";
	std::string_view str2 = "asbsaabbbaabb";
	std::string_view str3 = "sabb";
	std::string_view str4 = "asbsaabbaaabbb";

	// POSITIVE TESTS
	{
		SCOPED_TRACE("POSITIVE TESTS");
		this->testFSMResultPositive(testFSM.simulate(str1, MM_LONGEST_SUBSTRING), true, { 1, 6 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_LONGEST_SUBSTRING), true, { 4, 9 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_LONGEST_SUBSTRING), true, { 1, 4 });
		this->testFSMResultPositive(testFSM.simulate(str4, MM_LONGEST_SUBSTRING), true, { 8, 14 });
		this->testFSMResultPositive(testFSM.simulate(str1.substr(2, 5), MM_LONGEST_SUBSTRING), true, { 0, 4 });
		this->testFSMResultPositive(testFSM.simulate(str2.substr(3, 11), MM_LONGEST_SUBSTRING), true, { 1, 6 });
		this->testFSMResultPositive(testFSM.simulate(str3.substr(1, 3), MM_LONGEST_SUBSTRING), true, { 0, 3 });

		this->testFSMResultPositive(testFSM.simulate(str1, MM_WHOLE_STRING), false, { 0, 0 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_WHOLE_STRING), false, { 0, 0 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_WHOLE_STRING), false, { 0, 0 });
		this->testFSMResultPositive(testFSM.simulate(str1.substr(2, 5), MM_WHOLE_STRING), true, { 0, 4 });
		this->testFSMResultPositive(testFSM.simulate(str2.substr(4, 5), MM_WHOLE_STRING), true, { 0, 5 });
		this->testFSMResultPositive(testFSM.simulate(str3.substr(1), MM_WHOLE_STRING), true, { 0, 3 });

		this->testFSMResultPositive(testFSM.simulate(str1.substr(1), MM_LONGEST_PREFIX), true, { 0, 5 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_LONGEST_PREFIX), false, { 0, 0 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_LONGEST_PREFIX), false, { 0, 0 });
	}

	// NEGATIVE TESTS
	{
		SCOPED_TRACE("NEGATIVE TESTS");

		this->testFSMResultNegative(testFSM.simulate(str1.substr(2), MM_LONGEST_SUBSTRING), false, {1, 6});
		this->testFSMResultNegative(testFSM.simulate(str2.substr(1), MM_LONGEST_SUBSTRING), false, {4, 9});
		this->testFSMResultNegative(testFSM.simulate(str3, MM_LONGEST_SUBSTRING), true, { 1, 10 });
		this->testFSMResultNegative(testFSM.simulate(str4, MM_LONGEST_SUBSTRING), true, {4, 8});

		this->testFSMResultNegative(testFSM.simulate(str1.substr(1), MM_LONGEST_PREFIX), true, { 0, 6 });
	}

};

TYPED_TEST_P(FSMTests, simulate2) {

	using enum m0st4fa::fsm::FSM_MODE;
	using m0st4fa::fsm::FSMStateType;

	// Data structures
	typename TestFixture::TableType table{};
	this->Base::initTranFn_a(table);
	typename TestFixture::TranFn tranFn{ table };
	TypeParam testFSM{ std::set<FSMStateType>{ 2}, tranFn };

	// STRINGS
	std::string_view str1 = "a";
	std::string_view str2 = "aaa";
	std::string_view str3 = "babaa";

	// POSITIVE TESTS
	{
		SCOPED_TRACE("POSITIVE TESTS");
		this->testFSMResultPositive(testFSM.simulate(str1, MM_LONGEST_SUBSTRING), true, { 0, 1 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_LONGEST_SUBSTRING), true, { 0, 3 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_LONGEST_SUBSTRING), true, { 3, 5 });

		this->testFSMResultPositive(testFSM.simulate(str1, MM_WHOLE_STRING), true, { 0, 1 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_WHOLE_STRING), true, { 0, 3 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_WHOLE_STRING), false, { 0, 0 });

		this->testFSMResultPositive(testFSM.simulate(str1, MM_LONGEST_PREFIX), true, { 0, 1 });
		this->testFSMResultPositive(testFSM.simulate(str2, MM_LONGEST_PREFIX), true, { 0, 3 });
		this->testFSMResultPositive(testFSM.simulate(str3, MM_LONGEST_PREFIX), false, { 0, 0 });
	}


}

REGISTER_TYPED_TEST_SUITE_P(FSMTests, simulate, simulate2);
