#include "gtest/gtest.h"

#include "./universal.h"

#include "../NFA.h"

class NFATest : testing::Test {

	template<typename T>
	constexpr void initTranFn_ab_NFA(T& fun) {

		/*
		  -> a -> a
		s    |
		  -> b
		*/

		fun[1]['a'] = { 2 };
		fun[1]['b'] = { 3 };

		fun[2]['a'] = { 2, 4 };
		fun[2]['b'] = { 3 };

		fun[3]['a'] = { 2, 4 };
		fun[3]['b'] = { 3 };

		fun[4]['a'] = { 2, 4 };
		fun[4]['b'] = { 2 };

	};

};

#include "fsm.h"

using FSMStateSetType = m0st4fa::FSMStateSetType;
using TableType = m0st4fa::FSMTable;
using TranFn = m0st4fa::TransFn<TableType>;
using NFA = m0st4fa::NonDeterFiniteAutomatan<TranFn>;
using Result = m0st4fa::FSMResult;

INSTANTIATE_TYPED_TEST_CASE_P(NFATests, FSMTests, NFA);