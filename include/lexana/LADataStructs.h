#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "fsm/FiniteStateMachine.h"

namespace m0st4fa {

	/**
	* @brief The flags for lexical analyzer that control its behavior.
	**/
	enum LA_FLAG {
		//! @brief Default behavior.
		LAF_DEFAULT                    = 0b0000,
		//! @brief Do not skip whitespace characters.
		LAF_ALLOW_WHITE_SPACE_CHARS = 0b0001,
		//! @brief Do not skip only "new line" (does not allow other whitespace characters).
		LAF_ALLOW_NEW_LINE          = 0b0010,
		//! @brief The number of flags.
		LAF_COUNT = 3
	};


	/**
	* @brief Token used by the lexical analyzer.
	* 
	**/
	template <typename TerminalT, typename AttrT = std::string_view>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	struct Token {
		TerminalT name = TerminalT::T_EOF;
		AttrT attribute;

		static const Token<TerminalT, AttrT> EPSILON;
		static const Token<TerminalT, AttrT> TEOF;

		bool operator==(const Token& other) const {
			return name == other.name && attribute == other.attribute;
		};
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {
			return "<" + stringfy(this->name) + ", " + (std::string)this->attribute + ">";
		}
	};

	template <typename TerminalT, typename AttrT>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	const Token<TerminalT, AttrT> Token<TerminalT, AttrT>::EPSILON = { TerminalT::T_EPSILON };
	template <typename TerminalT, typename AttrT>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	const Token<TerminalT, AttrT> Token<TerminalT, AttrT>::TEOF = { TerminalT::T_EOF };


	template <typename TerminalT>
		requires requires (TerminalT a) { toString(a); }
	std::ostream& operator<<(std::ostream& os, const m0st4fa::Token<TerminalT>& token) {
		return os << token.toString();
	}
	

	template<typename TokenT, typename InputT = std::string_view>
	//													state      lexeme
	using TokenFactoryType = std::function<TokenT(m0st4fa::fsm::FSMStateType, InputT)>;
	

	// RESULT
	/**
	* @brief represents the result of a single `getNextToken()` call.
	**/
	template<typename TokenT, typename InputT = std::string_view>
	struct LexicalAnalyzerResult {
		using TokenFactoryType = m0st4fa::TokenFactoryType<TokenT, InputT>;

		// whether the token is found or not
		bool foundToken = false;
		// the token itself
		TokenT token = TokenT{};
		// the indicies of the lexeme of the token within the actual input
		fsm::Indicies indicies = fsm::Indicies{0, 0};
		// the number of the line at which the lexeme has been found
		size_t lineNumber = 0;
		// the remaining input after the lexeme was extracted
		//std::string_view remainingInput{};

		LexicalAnalyzerResult() = default;
		LexicalAnalyzerResult(const TokenT& token, fsm::Indicies indicies, const size_t lineNumber, const size_t colNumber) : token{ token }, indicies{ indicies + colNumber }, lineNumber{ lineNumber }, foundToken { true } {};
		LexicalAnalyzerResult(const fsm::FSMResult& fsmres, const size_t lineNumber, const size_t colNumber, const TokenFactoryType factory) : foundToken{ true }, token{ factory((fsm::FSMStateType)fsmres.finalState, fsmres.getMatch()) }, indicies{ fsmres.indicies + colNumber }, lineNumber{lineNumber} {};


		void reset() {
			this->foundToken = false;
			this->token = TokenT{};
		}

		explicit operator TokenT() const {
			return this->token;
		}

		std::string toString() const {
			return std::format("{{\nfoundToken: {},\ntoken: {},\nindicies: {},\nlineNumber: {}\n}}", foundToken, token.toString(), indicies.toString(), lineNumber);
		}

		operator std::string() const {
			return this->toString();
		}
	};

	// POSITION
	/**
	* @brief represents the position of the search during analyzing input lexically.
	**/
	struct Position {
		size_t line = 0;
		size_t column = 0;

		operator std::pair<size_t, size_t>() const {
			return {line, column};
		}
		operator std::string() const {
			return this->toString();
		}
		std::string toString() const {
			return std::format("({}, {})", line, column);
		}
	};

}

