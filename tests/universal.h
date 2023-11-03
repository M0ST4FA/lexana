#pragma once
#include <string>
#include <iostream>
#include <map>
#include <algorithm>

#include "fsm/DFA.h"
#include "utility/common.h"
#include "utility/Logger.h"
#include "universal.h"
#include <lexana/LADataStructs.h>

struct FSMSharedInfo {

protected:

	using FSMStateSetType = m0st4fa::fsm::FSMStateSetType;
	using FSMTableType = m0st4fa::fsm::FSMTable;
	using TranFn = m0st4fa::fsm::TransFn<FSMTableType>;
	using DFAType = m0st4fa::fsm::DeterFiniteAutomaton<TranFn>;
	using Result = m0st4fa::fsm::FSMResult;

	template<typename T = FSMTableType>
	static constexpr void initTranFn_ab(T& fun) {
		// corresponding regex: /aa(a*ba?)+b(a|b)*/
		// corresponding regex: /aa*bb*/

		fun(1, 'a') = 2;
		fun(1, 'b') = 3;
		fun(2, 'a') = 2;
		fun(2, 'b') = 3;
		fun(3, 'b') = 4;
		fun(4, 'b') = 4;
	}

	template<typename T>
	static constexpr void initTranFn_a(T& fun) {
		// corresponding regex: /aa(a*ba?)+b(a|b)*/
		// corresponding regex: /aa*bb*/
		
		fun(1, 'a')= 2;
		fun(2, 'a')= 2;

	}

	template<typename T>
	static constexpr void initTranFn_id_eq_num(T& fun) {
		// corresponding regex: /\w+(\w|\d)*|=|\d+/

		

		for (char c = 'a'; c <= 'z'; c++)
			fun(1, c) = 2;

		for (char c = 'a'; c <= 'z'; c++)
			fun(2, c) = 2;

		for (char c = '0'; c <= '9'; c++)
			fun(2, c) = 2;

		// /=/
		fun(1, '=') = 3;

		// /\d+/
		for (char c = '0'; c <= '9'; c++)
			fun(1, c) = fun(4, c) = 4;

	}

};

struct LASharedInfo {


protected:

	enum Terminal {
		T_A,
		T_B,
		T_ALPHA,
		T_NUM,
		T_ID,
		T_EQUAL,
		T_EOF,
		T_EPSILON
	};
	friend std::string toString(Terminal);
	friend std::string stringfy(Terminal term) {
		return toString(term);
	}

	using TokenType = m0st4fa::lexana::Token<Terminal, std::string_view>;

	static TokenType fact_ab(m0st4fa::fsm::FSMStateType state, std::string_view lexeme) {
		switch (state) {
		case 4:
			return { T_A, lexeme };

		default:
			fprintf(stderr, "Error: unexpected state %d\n", state);
			throw std::runtime_error("Unreachable");
		}
	};

	static TokenType fact_a(m0st4fa::fsm::FSMStateType state, std::string_view lexeme) {
		switch (state) {
		case 2:
			return { T_A, lexeme };

		default:
			fprintf(stderr, "Error: unexpected state %d\n", state);
			throw std::runtime_error("Unreachable");
		}
	};

	static TokenType fact_id_eq_num(m0st4fa::fsm::FSMStateType state, std::string_view lexeme) {
		switch (state) {
		case 2:
			return { T_ID, lexeme };
		case 3:
			return { T_EQUAL, lexeme };
		case 4:
			return { T_NUM, lexeme };

		default:
			m0st4fa::Logger{}.log(m0st4fa::LoggerInfo::FATAL_ERROR, std::format("Unexpected state: {}", state));
			std::abort();
			//throw std::runtime_error("Unreachable");
		}
	};


};