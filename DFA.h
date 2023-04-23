#pragma once

#include "FiniteStateMachine.h"
#include <ranges>
#include <assert.h>

namespace m0st4fa {

	// DECLARATIONS
	/**
	* @brief A DFA that that can be used to match strings.
	* Provides a single function: simulate();
	*/
	template <typename TransFuncT, typename InputT = std::string_view>
	class DeterFiniteAutomatan: public FiniteStateMachine<TransFuncT, InputT> {
		using Base = FiniteStateMachine<TransFuncT, InputT>;
		using SubstringType = Substring<FSMStateType>;

		// static variables
		constexpr static FSMStateType DEAD_STATE = 0;

		// private methods
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		FSMResult _simulate_longest_substring(const InputT&) const;

		bool _check_accepted_longest_prefix(const std::vector<FSMStateType>&, size_t&) const;
		bool _check_accepted_substring(const InputT&, std::vector<FSMStateType>&, size_t, size_t&) const;
		
	public:
		DeterFiniteAutomatan() = default;
		DeterFiniteAutomatan(const FSMStateSetType& fStates, const TransFuncT& tranFn, FlagsType flags = FSM_FLAG::FF_FLAG_NONE) :
			FiniteStateMachine<TransFuncT, InputT> {fStates, tranFn, FSMType::MT_DFA, flags}
		{};
		DeterFiniteAutomatan& operator=(const DeterFiniteAutomatan& rhs) {
			this->Base::operator=(rhs);
			return *this;
		}

		FSMResult simulate(const InputT&, const FSM_MODE) const;
		
	};

	template <typename TransFuncT, typename InputT = std::string>
	using DFAType = DeterFiniteAutomatan<TransFuncT, InputT>;
	

	// IMPLEMENTATIONS
	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_whole_string(const InputT& input) const
	{
		FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		FSMStateType currState = startState;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Break if you hit a dead state since it is dead.
		*/
		for (auto c : input) {
			currState = (FSMStateType)this->m_TransitionFunc(currState, c);

			if (currState == DEAD_STATE)
				break;
		}

		bool accepted =this->_is_state_final(currState);

		return FSMResult(accepted, accepted ? FSMStateSetType{currState} : FSMStateSetType{startState}, { 0, accepted ? input.size() : 0 }, input);
	}


	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		FSMStateType currState = startState;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/ 
		std::vector matchedStates = { currState };
		// character index of the last matching character, or 0
		size_t charIndex = 0;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * Break if you hit a dead state since it is dead.
		*/
		for (; charIndex < input.size(); charIndex++) {
			const char c = input[charIndex];

			// get next state
			currState = (FSMStateType)this->m_TransitionFunc(currState, c);

			// break out if it is dead
			if (currState == DEAD_STATE) {
				if (charIndex != 0)
					charIndex--;

				break;
			}

			// update our path through the machine
			matchedStates.push_back(currState);
		}

		if (charIndex == input.size())
			charIndex--;

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStates, charIndex);

		const IndexType	start = 0;
		const IndexType end = accepted ? charIndex + 1 : 0;

		// Note: it is guaranteed that there will be one final state; it is just that this data structure is used for both NFA and DFA
		const FSMStateSetType finalStates = this->_get_final_states_from_state_set(matchedStates.back());

		return FSMResult(accepted, finalStates, { start, end }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		std::vector matchedStates = { {startState } };
		size_t startIndex = 0;
		size_t charIndex = 0;

		std::vector<SubstringType> matchedStatesSets{};

		/**
		 * Follow a path through the machine using the characters of the string until you check all the characters.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * If a substring was not accepted, start matching the next substring if any
		*/
		for (; charIndex < input.size(); charIndex = ++startIndex) {

			// check whether the substring is accepted (and populate everything given by reference)
			bool accepted = _check_accepted_substring(input, matchedStates, startIndex, charIndex);

			// if the substring was accepted
			if (accepted) {
				matchedStatesSets.push_back(SubstringType{ matchedStates, startIndex, charIndex + 1 });

				// if the rest of the input is less than the current substring in length, leave
				if ((input.size() - startIndex) < matchedStates.size())
					break;
			}

			matchedStates.resize(1);
		}

		if (matchedStatesSets.size()) {

			const SubstringType* longest = nullptr;
			size_t size = 0;

			// choose the longest substring or the first of many having the same length
			std::ranges::reverse_view rv{ matchedStatesSets };
			for (const auto& data : rv) {
				// if the length of this substring is longer than that of the previously catched
				if (data.size() > size) {
					longest = &data;
					size = longest->size();
				}
			}

			const size_t startIndex = longest->indecies.start;
			const size_t endIndex = longest->indecies.end;

			// get the final states we've reached
			const FSMStateType finalState = matchedStates.back();

			return FSMResult(true, finalState, { startIndex, endIndex }, input);
		}

		// if there was no accepted substring
		return FSMResult(false, { startState }, { 0, 0 }, input);
	};
	
	/**
	* @param `matchedStates`: the result of matching an NFA against a string. `charIndex` the index of the last matching character within the input.
	* @return true if a prefix matches, false otherwise. It also updates `charIndex` to the index of the last character of that prefix.
	**/
	template<typename TransFuncT, typename InputT>
	inline bool DeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<FSMStateType>& matchedStates, size_t& charIndex) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;

		/**
		* Loop through the path from the end seeking the closes final state.
		* Update the character index as you do so.
		*/
		std::ranges::reverse_view rv{ matchedStates };
		for (const FSMStateSetType& state : rv)
		{

			if (this->_is_state_final(state))
				return true;

			charIndex--;
		}

		// make sure `charIndex` does not get below 0
		if (charIndex < 0)
			charIndex = 0;

		return false;
	}

	/**
	* @brief checks whether the substring starting from `startIndex` accepts.
	* @param 
		* `matchedStates`: set to the path taken through the machine.
		* `charIndex`: the index of the last checked character (the last that didn't result in a dead state).
	* @return whether a substring starting from startIndex has accepted
	**/
	template<typename TransFuncT, typename InputT>
	bool DeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_substring(const InputT& input, std::vector<FSMStateType>& matchedStates, size_t startIndex, size_t& charIndex) const
	{
		assert(startIndex == charIndex);

		FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		FSMStateType currState = startState;
		// start from startIndex until the end of the string or until you reach a dead state
			/**
			* The value of charIndex will be correct because the last time we increment it, the condition is guaranteed to fail.
			* This guarantees that charIndex will always equal: the number of matched characters + startIndex.
			*/

		for (; charIndex < input.size(); charIndex++) {
			// get next state
			auto c = input[charIndex];
			currState = FSMStateType(this->m_TransitionFunc(currState, c));

			// break out if it is dead
			if (currState == DEAD_STATE) {
				charIndex--; // do not keep count of the last character
				break;
			}

			// update the path through the machine
			matchedStates.push_back(currState);
		};

		// make sure charIndex is less that input.size() even if the entire string accepts
		if (charIndex == input.size())
			charIndex--;

		// endIndex = number of characters checked + the offset of the substring into the input string
		size_t endIndex = matchedStates.size() + startIndex;
		//this->m_Logger.logDebug(std::format("at _check_accepted_substring: charIndex: {}, endIndex: {}\n", charIndex, endIndex));

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStates, endIndex);
		
		return accepted;
	};

	/**
	* @brief Simulate the given input string using the given simulation method.
	*/
	template<typename TransFuncT, typename InputT>
	inline FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::simulate(const InputT& input, const FSM_MODE mode) const
	{
		switch (mode) {
		case FSM_MODE::MM_WHOLE_STRING:
			return this->_simulate_whole_string(input);
		case FSM_MODE::MM_LONGEST_PREFIX:
			return this->_simulate_longest_prefix(input);
		case FSM_MODE::MM_LONGEST_SUBSTRING:
			return this->_simulate_longest_substring(input);
		default:
			std::cerr << "Unreachable: simulate() cannot reach this point." << std::endl;
			throw UnrecognizedSimModeException();
		}

	}

}