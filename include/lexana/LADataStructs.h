#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "fsm/FiniteStateMachine.h"

namespace m0st4fa::lexana {

	/**
	* @brief The LexicalAnalyzer flags that control its behavior.
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
	* @brief Token used by the lexical analyzer; each token has a name and an attribute and represents a lexeme from the input file.
	**/
	template <typename TerminalT, typename AttrT = std::string_view>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	struct Token {
		/**
		 * @brief The name associated with the token.
		 */
		TerminalT name = TerminalT::T_EOF;
		/**
		 * @brief The attribute associated with the token.
		 */
		AttrT attribute;

		/**
		 * @brief The Token object representing epsilon (an empty-string lexeme).
		 */
		static const Token<TerminalT, AttrT> EPSILON;
		/**
		 * @brief The Token object representing an end-of-file lexeme.
		 */
		static const Token<TerminalT, AttrT> TEOF;

		/**
		 * @brief Checks whether to Token objects are equal (i.e. have the same name *and* attribute).
		 * @param rhs The right-hand-side parameter/operand of the operator. The left-hand-side is `this` object.
		 * @return `true` if both Token objects are equal; `false` otherwise.
		 */
		bool operator==(const Token& rhs) const {
			return name == rhs.name && attribute == rhs.attribute;
		};
		
		/**
		 * @brief Calls toString().
		 */
		operator std::string() const {
			return this->toString();
		}

		/**
		 * @brief Converts this Token object into an std::string object and returns it.
		 * @return The std::string object corresponding to `this`.
		 */
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
	std::ostream& operator<<(std::ostream& os, const lexana::Token<TerminalT>& token) {
		return os << token.toString();
	}
	
	/**
	 * @brief Aliases the type of a factory function that creates tokens based on FSM states and lexeme input.
	 */
	template<typename TokenT, typename InputT = std::string_view>
	//													    state             lexeme
	using TokenFactoryType = std::function<TokenT(m0st4fa::fsm::FSMStateType, InputT)>;

	// RESULT
	/**
	* @brief represents the result of a single `getNextToken()` call.
	**/
	template<typename TokenT, typename InputT = std::string_view>
	struct LexicalAnalyzerResult {
		using TokenFactoryType = lexana::TokenFactoryType<TokenT, InputT>;

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

