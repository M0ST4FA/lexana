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

		//! @brief Do not skip whitespace characters. They are included in the Token object as part of the lexeme/name.
		LAF_ALLOW_WHITE_SPACE_CHARS = 0b0001,

		//! @brief Skip other characters except "new line" (does not allow other whitespace characters).
		LAF_ALLOW_NEW_LINE          = 0b0010,

		//! @brief The number of flags.
		LAF_COUNT = 3
	};

	/**
	* @brief Token used by the lexical analyzer; each token has a name and an attribute and represents a lexeme from the input file.
	* @note The class can be written to `std::ostream` (it overloads `operator<<`).
	* @tparam TerminalT The type of the name of a Token object. It is thought of as a type of terminals.
	* @tparam AttrT The type of the attribute of a Token object.
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
		 * @brief Converts the Token object into an std::string object and returns it.
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

	/**
	 * @brief Writes a Token object to a standard stream.
	 * @param os The stream to which the Token object will be written.
	 * @param token The Token object to be written.
	 * @return A reference to the standard stream given as input.
	 */
	template <typename TerminalT>
		requires requires (TerminalT a) { toString(a); }
	std::ostream& operator<<(std::ostream& os, const lexana::Token<TerminalT>& token) {
		return os << token.toString();
	}
	
	/**
	 * @brief Aliases the type of a factory function that creates tokens based on FSM states and lexeme input.
	 * @tparam TokenT The type of a token object.
	 * @tparam InputT The type of input (lexemes).
	 */
	template<typename TokenT, typename InputT = std::string_view>
	//													    state             lexeme
	using TokenFactoryType = std::function<TokenT(m0st4fa::fsm::FSMStateType, InputT)>;

	// RESULT
	/**
	* @brief represents the result of a single `getNextToken()` call.
	* @tparam TokenT The type of a token object.
	* @tparam InputT The type of input (lexemes).
	**/
	template<typename TokenT, typename InputT = std::string_view>
	struct LexicalAnalyzerResult {
		using TokenFactoryType = lexana::TokenFactoryType<TokenT, InputT>;

		//! @brief Whether the token was found or not.
		bool foundToken = false;
		//! @brief The token itself.
		TokenT token = TokenT{};
		//! @brief The indicies of the lexeme/name of the token within the actual input.
		fsm::Indicies indicies = fsm::Indicies{0, 0};
		//! @brief The number of the line at which the lexeme was found.
		size_t lineNumber = 0;
		// the remaining input after the lexeme was extracted
		//std::string_view remainingInput{};

		//! @brief The default constructor.
		LexicalAnalyzerResult() = default;

		/**
		 * @brief Initializes a LexicalAnalyzerResult object from given parameters.
		 * @param token The token that the LexicalAnalyzerResult object will store.
		 * @param indicies The indicies of that token.
		 * @param lineNumber The number of the line at which the token appears. The first line has the number 0.
		 * @param colNumber The number of the first letter of the token in the line at which it appears. The counting starts from 0.
		 */
		LexicalAnalyzerResult(const TokenT& token, fsm::Indicies indicies, const size_t lineNumber, const size_t colNumber) : token{ token }, indicies{ indicies + colNumber }, lineNumber{ lineNumber }, foundToken { true } {};
		/**
		 * @brief Initializes a LexicalAnalyzerResult object from given parameters.
		 * @param fsmres An FSMResult object from which the token and the indicies will be extracted.
		 * @param lineNumber The number of the line at which the token appears. The first line has the number 0.
		 * @param colNumber The number of the first letter of the token in the line at which it appears. The counting starts from 0.
		 */
		LexicalAnalyzerResult(const fsm::FSMResult& fsmres, const size_t lineNumber, const size_t colNumber, const TokenFactoryType factory) : foundToken{ true }, token{ factory((fsm::FSMStateType)fsmres.finalState, fsmres.getMatch()) }, indicies{ fsmres.indicies + colNumber }, lineNumber{lineNumber} {};

		/**
		 * @brief Resets the LexicalAnalyzerResult object to storing no Token object.
		 */
		void reset() {
			this->foundToken = false;
			this->token = TokenT{};
		}

		/**
		 * @brief Converts the LexicalAnalyzerResult object to a Token object.
		 */
		explicit operator TokenT() const {
			return this->token;
		}

		/**
		 * @brief Converts the LexicalAnalyzerResult object into an std::string object and returns it.
		 * @return The std::string object corresponding to `this`.
		 */
		std::string toString() const {
			return std::format("{{\nfoundToken: {},\ntoken: {},\nindicies: {},\nlineNumber: {}\n}}", foundToken, token.toString(), indicies.toString(), lineNumber);
		}

		/**
		 * @brief Calls toString().
		 */
		operator std::string() const {
			return this->toString();
		}
	};

	// POSITION
	/**
	* @brief The position of some lexeme.
	* @remark This struct is used by the lexical analyzer to represent the position of the last matched lexeme when calling the function LexicalAnalyzer::getPosition().
	**/
	struct Position {

		//! @brief The line number at which the lexeme is found.
		size_t line = 0;

		//! @brief The number of the first character of the lexeme within the line at which it is found.
		size_t column = 0;

		//! @brief Converts the Position object into std::pair object.
		operator std::pair<size_t, size_t>() const {
			return {line, column};
		}

		//! @brief Calls toString().
		operator std::string() const {
			return this->toString();
		}

		/**
		 * @brief Converts the Position object into an std::string object and returns it.
		 * @return The std::string object corresponding to `this`.
		 */
		std::string toString() const {
			return std::format("({}, {})", line, column);
		}
	};

}

