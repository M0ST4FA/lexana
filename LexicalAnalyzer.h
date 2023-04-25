#pragma once

#include <string>

#include "../FiniteStateMachine.h"
#include "../DFA.h"
#include "../Logger.h"
#include "../LADataStructs.h"

namespace m0st4fa {

	template <typename TokenT, typename TableT = FSMTable, typename InputT = std::string_view>
	class LexicalAnalyzer {

		using Result = LexicalAnalyzerResult<TokenT, InputT>;

		DFAType<TransFn<TableT>, InputT> m_Automatan;
		TokenFactoryType<TokenT, InputT> m_TokenFactory = nullptr;
		InputT m_SourceCode;

		/** 
		* @brief The current line w're searching at.
		**/ 
		size_t m_Line = 0;
		// @brief The last character we've touched since the last retrieved token or escaped whitespace.
		size_t m_Col = 0;
		Logger m_Logger;

		inline void _remove_whitespace_prefix(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);
		inline bool _check_source_code_empty() {
			if (!this->m_SourceCode.empty())
				return false;

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"End of file reached"});
			this->m_Logger.logDebug(msg);

			// assuming that EOF is the default value for a token
			return true;
		};
		/**
		* @brief remove all whitespaces and count new lines, then check whether source code is empty.
		* @return boolean indicating whether the checks have been successful, i.e. all whitespaces have been dealt with accordingly and the source code is not empty.
		**/
		inline bool _check_presearch_conditions(const unsigned flags = (unsigned)LA_FLAG::LAF_DEFAULT) {
			// remove all white spaces and count new lines
			if (!(flags & (unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS))
				this->_remove_whitespace_prefix(flags);

			// if we are at the end of the source code or it is empty, return EOF token
			return not this->_check_source_code_empty();
		};

	protected:

		const DFAType<TransFn<TableT>, InputT>& getAutomatan() { return this->m_Automatan; };
		const TokenFactoryType<TokenT, InputT> getTokenFactory() { return this->m_TokenFactory; };

	public:

		LexicalAnalyzer() = default;

		LexicalAnalyzer(
			const DFAType<TransFn<TableT>, InputT>& automaton,
			const TokenFactoryType<TokenT, InputT> tokenFactory,
			const std::string_view sourceCode) :
			m_Automatan{ automaton }, m_TokenFactory{ tokenFactory }, m_SourceCode{ sourceCode }
		{

			// check the token factory is set
			assert((bool)m_TokenFactory && "Token factory is not set!");
		}

		LexicalAnalyzer(const LexicalAnalyzer&) = default;
		LexicalAnalyzer(LexicalAnalyzer&&) = default;
		LexicalAnalyzer& operator= (const LexicalAnalyzer& rhs) {
			this->m_Automatan = rhs.m_Automatan;
			this->m_Col = rhs.m_Col;
			this->m_Line = rhs.m_Line;
			this->m_SourceCode = rhs.m_SourceCode;
			this->m_TokenFactory = rhs.m_TokenFactory;

			return *this;
		}

		Result getNextToken(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);
		const std::string_view& getSourceCode() { return this->m_SourceCode; };
		Result peak(unsigned = (unsigned)LA_FLAG::LAF_DEFAULT);
		size_t getLine() { return this->m_Line; };
		size_t getCol() { return this->m_Col; };
		Position getPosition() {
			return Position{m_Line, m_Col};
		};

	};

}

namespace m0st4fa {

	/**
	* @brief Eliminate any whitespace "prefix" from the current source code. depending on the flag given to it, it may also count new lines and set `m_Line` and `m_Col` appropriately.
	* @param flags to influence its behavior. currently, the only flag that does is `LAF_ALLOW_NEW_LINE`; any other flag will be ignored and will have no effect on its behavior.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	void LexicalAnalyzer<TokenT, TableT, InputT>::_remove_whitespace_prefix(unsigned flags)
	{

		// this function will be entered in case white space characters are to be removed.
		// remove all whitespaces...
		while (true) {
			const char currChar = *this->m_SourceCode.data();
			const bool isWhiteSpace = std::isspace(currChar) && currChar != '\0'; // null marks the EOF.
			// test whether `currChar` is a new line and new lines are allowed to be counted
			const bool matchNewLine = currChar == '\n' && (flags & (unsigned)LA_FLAG::LAF_ALLOW_NEW_LINE);

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
	* @brief get the next matching token from the input, skipping all nonmatching characters in between this and the previous match.
	* @param flags to modify the behavior: 
		`LAF_DEFAULT`                 -> default behavior;
		`LAF_ALLOW_WHITE_SPACE_CHARS` -> do not skip whitespace, however include them in the token (by default, they are not included in the token);
		`LAF_ALLOW_NEW_LINE`          -> allow only new lines, however, do not allow other whitespace characters.
	* @return the next token.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::Result m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::getNextToken(unsigned flags)
	{

		// if checks fail
		if (not this->_check_presearch_conditions(flags))
			return Result{};

		// check whether there is a matched lexeme
		const FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);

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
		m_SourceCode.remove_prefix(lexemeSize);

		return res;
	}

	/**
	* @brief peak the next token without modifying character count nor line number, except that it deals with whitespace as normal (i.e. may skip them, depending on the flags, and hence modify line number and character count).
	* @return the next token.
	**/
	template<typename TokenT, typename TableT, typename InputT>
	m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::Result m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::peak(unsigned flags) {

		// if checks fail
		if (!this->_check_presearch_conditions(flags))
			return Result{};

		// check whether there is a matched lexeme
		const FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);

		// if there is not, early return
		if (!fsmRes.accepted)
			return LexicalAnalyzerResult<TokenT, InputT>{};

		// there is a matching lexeme

		// extract it; do not erase it nor update the character count nor line count
		const Result res{ fsmRes, this->getLine(), this->getCol(), this->m_TokenFactory };

		return res;
	}

};