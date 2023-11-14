#pragma once

#include <string>
#include <string_view>

#include "fsm/FiniteStateMachine.h"
#include "fsm/DFA.h"
#include "utility/Logger.h"
#include "lexana/LADataStructs.h"

namespace m0st4fa::lexana {

	/**
	 * @brief A machine (lexical analyzer) that can be used to convert a stream of characters into a stream of tokens.
	 * @tparam TokenT The type of a token object.
	 * @tparam TableT The type of FSM table.
	 * @tparam InputT The type of input (stream of characters).
	 */
	template <typename TokenT, typename TableT = fsm::FSMTable, typename InputT = std::string_view>
	class LexicalAnalyzer {

		using Result = LexicalAnalyzerResult<TokenT, InputT>;

		//! @brief The automaton used by the lexical analyzer to match the lexemes of tokens.
		fsm::DFA<fsm::TransFn<TableT>, InputT> m_Automaton;

		//! @brief The function that generates tokens given a lexeme.
		TokenFactoryType<TokenT, InputT> m_TokenFactory = nullptr;

		//! @brief The stream of characters input to the lexical analyzer and from which tokens will be extracted.
		InputT m_SourceCode;

		//! @brief The current line that the search is at.
		size_t m_Line = 0;

		//! @brief The last character touched since the last retrieved token or escaped whitespace.
		size_t m_Col = 0;

	protected:
		//! @brief The Logger object used to log to standard streams.
		Logger m_Logger;

		inline void _remove_whitespace_prefix(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);
		
		/**
		 * @brief Checks whether source code/input to the lexical analyzer is empty.
		 * @return `true` if source code is empty; `false` otherwise.
		 */
		inline bool _check_source_code_empty() {
			if (!this->m_SourceCode.empty())
				return false;

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"End of file reached"});
			this->m_Logger.logDebug(msg);

			// assuming that EOF is the default value for a token
			return true;
		};
		/**
		* @brief Removes all white-spaces and counts new lines, then checks whether source code is empty.
		* @remark This is a convenience function that does all of the previously mentioned things in sequence. 
		* @return `true` if the checks have been successful, i.e. all white-spaces have been dealt with accordingly and the source code is not empty; `false` otherwise.
		**/
		inline bool _check_presearch_conditions(const unsigned flags = (unsigned)LA_FLAG::LAF_DEFAULT) {
			// remove all white spaces and count new lines
			if (!(flags & (unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS))
				this->_remove_whitespace_prefix(flags);

			// if we are at the end of the source code or it is empty, return EOF token
			return not this->_check_source_code_empty();
		};

	protected:

		/**
		 * @brief Gets the fsm::DFA object used by this LexicalAnalyzer object.
		 */
		const fsm::DFA<fsm::TransFn<TableT>, InputT>& getAutomaton() { return this->m_Automaton; };

		/**
		 * @brief Gets the TokenFactoryType object used by this LexicalAnalyzer object.
		 */
		const TokenFactoryType<TokenT, InputT> getTokenFactory() { return this->m_TokenFactory; };

	public:

		//! @brief Default constructor.
		LexicalAnalyzer() = default;

		/**
		 * @brief Initializes a LexicalAnalyzer object from given parameters.
		 * @param automaton The fsm::DFA object that this LexicalAnalyzer object will use for matching lexemes.
		 * @param tokenFactory The function that will be used to generate Token objects out of matched lexemes.
		 * @param sourceCode The stream of characters out of which tokens will be extracted as Token objects.
		 */
		LexicalAnalyzer(
			const fsm::DFA<fsm::TransFn<TableT>, InputT>& automaton,
			const TokenFactoryType<TokenT, InputT> tokenFactory,
			const std::string_view sourceCode) :
			m_Automaton{ automaton }, m_TokenFactory{ tokenFactory }, m_SourceCode{ sourceCode }
		{

			// check the token factory is set
			assert((bool)m_TokenFactory && "Token factory is not set!");
		}

		//! @brief Copy constructor.
		LexicalAnalyzer(const LexicalAnalyzer&) = default;

		//! @brief Move constructor.
		LexicalAnalyzer(LexicalAnalyzer&&) = default;

		//! @brief Copy operator.
		LexicalAnalyzer& operator= (const LexicalAnalyzer& rhs) {
			this->m_Automaton = rhs.m_Automaton;
			this->m_Col = rhs.m_Col;
			this->m_Line = rhs.m_Line;
			this->m_SourceCode = rhs.m_SourceCode;
			this->m_TokenFactory = rhs.m_TokenFactory;

			return *this;
		}

		//! @brief Gets the source code/stream of characters used by the lexical analyzer.
		const std::string_view& getSourceCode() { return this->m_SourceCode; };

		//! @brief Gets the last matched line within the source code.
		size_t getLine() { return this->m_Line; };

		//! @brief Gets the position of the last matched character within the last matched line.
		size_t getCol() { return this->m_Col; };

		//! @brief Gets the position of the last matched lexeme.
		Position getPosition() {
			return Position{m_Line, m_Col};
		};

		Result getNextToken(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);
		Result peak(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);

	};

}

namespace m0st4fa::lexana {

	/**
	* @brief Eliminates any whitespace "prefix" from the source code. 
	* @details Depending on the flags given to it, it may allow new lines or other characters. It also sets the line and character numbers correctly.
	* @param flags The flags to the function. These flags influence the characters escaped. 
	* @note Currently, the only flag that is supported is LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE; any other flag will be ignored and will have no effect.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	void LexicalAnalyzer<TokenT, TableT, InputT>::_remove_whitespace_prefix(unsigned flags)
	{

		// this function will be entered in case white space characters are to be removed.
		// remove all white-spaces...
		while (true) {
			const char currChar = *this->m_SourceCode.data();
			const bool isWhiteSpace = std::isspace(currChar) && currChar != '\0'; // null marks the EOF.
			// test whether `currChar` is a new line and new lines are allowed to be counted
			const bool matchNewLine = currChar == '\n' && (flags & (unsigned)LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE);

			// if we have not catched a white space...
			if (!isWhiteSpace)
				return; // early return

			// erase whitespace from source code stream
			this->m_SourceCode.remove_prefix(1);

			// if the current character is a new line char and they are allowed, do not remove the current char
			if (matchNewLine)
				this->m_Line++, this->m_Col = 0;
			else 
				this->m_Col++; // if this whitespace character is not a new line character
		}

	}

	/**
	* @brief Gets the next matching token from the source code/input.
	* @note The behavior of the function can be modified by giving it some flags. The flags determine what characters are escaped.
	* @param flags The flags to the function.
	* @return The next token in the input stream, if any; otherwise an empty token.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	lexana::LexicalAnalyzer<TokenT, TableT, InputT>::Result lexana::LexicalAnalyzer<TokenT, TableT, InputT>::getNextToken(unsigned flags)
	{

		// if checks fail
		if (not this->_check_presearch_conditions(flags))
			return Result{}; // empty token

		// check whether there is a matched lexeme
		const fsm::FSMResult fsmRes = this->m_Automaton.simulate(this->m_SourceCode, fsm::FSM_MODE::MM_LONGEST_PREFIX);

		// if there is not, early return
		if (not fsmRes.accepted)
			return LexicalAnalyzerResult<TokenT, InputT>{};

		// there is a matching lexeme

		// 1. extract it
		const Result res{ fsmRes, this->getLine(), this->getCol(), this->m_TokenFactory};

		// 2. adjust the column (character) number
		const size_t lexemeSize = fsmRes.size();
		this->m_Col += lexemeSize;

		// 3. erase the lexeme from source code stream
		this->m_SourceCode.remove_prefix(lexemeSize);

		return res;
	}

	/**
	* @brief Peaks the next token (i.e. it doesn't modify the character count nor line number).
	* @note It deals with whitespace as with getNextToken() (i.e. may skip them, depending on the flags, and hence modify line number and character count).
	* @param flags The flags to the function.
	* @return The next token in the input stream, if any; otherwise an empty token.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	lexana::LexicalAnalyzer<TokenT, TableT, InputT>::Result lexana::LexicalAnalyzer<TokenT, TableT, InputT>::peak(unsigned flags) {

		// if checks fail
		if (!this->_check_presearch_conditions(flags))
			return Result{};

		// check whether there is a matched lexeme
		const fsm::FSMResult fsmRes = this->m_Automaton.simulate(this->m_SourceCode, fsm::FSM_MODE::MM_LONGEST_PREFIX);

		// if there is not, early return
		if (!fsmRes.accepted)
			return LexicalAnalyzerResult<TokenT, InputT>{};

		// there is a matching lexeme

		// extract it; do not erase it nor update the character count nor line count
		const Result res{ fsmRes, this->getLine(), this->getCol(), this->m_TokenFactory };

		return res;
	}

};