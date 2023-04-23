#pragma once

#include <stack>
#include <ranges>
#include <functional>
#include <assert.h>

#include "FiniteStateMachine.h"

namespace m0st4fa {

	// DECLARATIONS
	/**
	* @brief An NFA that can be used to match strings.
	* The transition function must map states and input to sets of states.
	*/	
	template <typename TransFuncT, typename InputT = std::string_view>
	class NonDeterFiniteAutomatan : public FiniteStateMachine<TransFuncT, InputT> {
		using Base = FiniteStateMachine<TransFuncT, InputT>;
		using SubstringType = Substring<FSMStateSetType>;

		// static variables
		constexpr static FSMStateType DEAD_STATE = 0;

		// PRIVATE METHODS

		// MAIN
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		FSMResult _simulate_longest_substring(const InputT&) const;

		// HELPERS
		bool _check_accepted_longest_prefix(const std::vector<FSMStateSetType>&, size_t&) const;

		bool _check_accepted_substring(const InputT&, std::vector<FSMStateSetType>&, size_t, size_t&) const;
		inline void _record_matched_substrings(const InputT, size_t&, size_t&, std::vector<FSMStateSetType>&, std::vector<SubstringType>&) const;
		FSMResult _get_longest_substring_from_matched_sets(const InputT, const std::vector<SubstringType>&) const;

		FSMStateSetType _epsilon_closure(const FSMStateSetType&) const;

	public:
		NonDeterFiniteAutomatan() = default;
		NonDeterFiniteAutomatan(const FSMStateSetType& fStates, const TransFuncT& tranFn, FSMType machineType = FSMType::MT_EPSILON_NFA, FlagsType flags = FSM_FLAG::FF_FLAG_NONE) :
			FiniteStateMachine<TransFuncT, InputT>{ fStates, tranFn, machineType, flags }
		{


			// if the correct machine type is not passed
			if (!(machineType == FSMType::MT_EPSILON_NFA || machineType == FSMType::MT_NON_EPSILON_NFA)) {
				const std::string message = R"(NonDeterFiniteAutomatan: machineType must be either "MT_EPSILON_NFA" or "MT_NON_EPSILON_NFA")";
				this->m_Logger.log(LoggerInfo::FATAL_ERROR, message);
				throw InvalidStateMachineArgumentsException(message);
			};

		};


		FSMResult simulate(const InputT&, const FSM_MODE) const;

	};

	template <typename TransFuncT, typename InputT = std::string>
	using NFA = NonDeterFiniteAutomatan<TransFuncT, InputT>;

	// IMPLEMENTATIONS
	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_whole_string(const InputT& input) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		FSMStateSetType currState = { startState };

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Break if you hit a dead state since it is dead.
		*/
		if (this->getMachineType() == FSMType::MT_NON_EPSILON_NFA) // If the machine is a non-epsilon NFA
			for (auto c : input)
				currState = this->m_TransitionFunc(currState, c);
		else // If the machine is an epsilon NFA
			for (auto c : input)
				currState = _epsilon_closure(this->m_TransitionFunc(currState, c));
		
		// assert whether we've reached a final state
		FSMStateSetType finalStates = this->_get_final_states_from_state_set(currState);
		bool accepted = !finalStates.empty();
				

		return FSMResult(accepted, finalStates, { 0, accepted ? input.size() : 0 }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		std::vector<FSMStateSetType> matchedStates = { {startState} };
		size_t charIndex = 0;

		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		*/
		if (this->getMachineType() == FSMType::MT_NON_EPSILON_NFA)
			for (; charIndex < input.size(); charIndex++) {
				const auto c = input.at(charIndex);
				// get next set of states and update our path through the machine
				matchedStates.push_back(this->m_TransitionFunc(matchedStates.back(), c));
			}
		else 
			for (; charIndex < input.size(); charIndex++) {
				const auto c = input.at(charIndex);
				// get next set of states and update our path through the machine
				matchedStates.push_back(this->_epsilon_closure(this->m_TransitionFunc(matchedStates.back(), c)));
			}

		// make sure `charIndex` < input.size()
		if (charIndex == input.size())
			charIndex = input.size() - 1;

		// set the index of the current character; figure out whether there is an accepted longest prefix; set the end index
		bool accepted = _check_accepted_longest_prefix(matchedStates, charIndex);
		size_t end = accepted ? charIndex + 1 : 0;
		
		// get the final states we've reached
		const FSMStateSetType& finalStates = this->_get_final_states_from_state_set(matchedStates.back());

		return FSMResult(accepted, finalStates, { 0, end }, input);
	}

	/**
	* @param `input`: the input that the NFA will be simulated against.
	* @return the result of the simulation.
	**/
	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		std::vector<FSMStateSetType> matchedStates = { {startState} };

		std::vector<SubstringType> substrings{};

		size_t start = 0, charIndex = 0;

		_record_matched_substrings(input, start, charIndex, matchedStates, substrings);

		if (substrings.size())
			return _get_longest_substring_from_matched_sets(input, substrings);

		// if there was no accepted substring
		return FSMResult(false, {}, {0, 0}, input);

	}

	/**
	* @param `stateSet`: the result of matching an NFA against a string. `charIndex` the index of the last matching character within the input.
	* @return true if a prefix matches, false otherwise. It also updates `charIndex` to the index of the last character of that prefix.
	**/
	template<typename TransFuncT, typename InputT>
	bool NonDeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<FSMStateSetType>& stateSet, size_t& charIndex) const
	{
		constexpr FSMStateType startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* Loop through the path from the end seeking the closes final state.
		* Update the character index as you do so.
		*/
		std::ranges::reverse_view rv{stateSet};
		for(const FSMStateSetType& state : rv)
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
	* @brief check whether the substring from `input` starting from index `startIndex` and ending at index `endIndex` is accepted. also, populate the `matchedStatesSet` with the path going through that substring.
	* @param `charIndex` is set the index of the last matching character and `matchedStates` is set to the path followed through the machine.
	* @return return whether the substring is accepted.
	**/
	template<typename TransFuncT, typename InputT>
	bool NonDeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_substring(const InputT& input, std::vector<FSMStateSetType>& matchedStates, size_t startIndex, size_t& charIndex) const	
	{

		assert(charIndex == startIndex);

		/**
			* Follow a path through the machine using the characters of the string.
			* Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		*/
		auto matchInput = [input, &charIndex, &matchedStates, this](bool calcClosure = false) {
			for (; charIndex < input.size(); charIndex++) {

				auto c = input[charIndex];

				FSMStateSetType currStateSet{};

				if (calcClosure) {
					currStateSet = _epsilon_closure(
						this->m_TransitionFunc(matchedStates.back(), c)
					);
				}
				else {
					currStateSet = this->m_TransitionFunc(matchedStates.back(), c);
				}

				// if the current state is empty
				if (!currStateSet.size()) {
					charIndex--;
					return; // break out of the loop and the function
				}

				// get next set of states
				// update our path through the machine
				matchedStates.push_back(currStateSet);
			}

			// make sure charIndex is less that input.size() even if the entire string accepts
			if (charIndex == input.size())
				charIndex--;
		};
		
		if (this->getMachineType() == FSMType::MT_NON_EPSILON_NFA)
			matchInput();
		else
			matchInput(true);

		// figure out whether there is an accepted longest prefix
		return _check_accepted_longest_prefix(matchedStates, charIndex);
	}

	/**
	* @brief records all matched substrings in `input` if any and updates all necessary variables given to as input.
	**/
	template<typename TransFuncT, typename InputT>
	inline void NonDeterFiniteAutomatan<TransFuncT, InputT>::_record_matched_substrings(const InputT input, size_t& start, size_t& charIndex, std::vector<FSMStateSetType>& matchedStates, std::vector<SubstringType>& substrings) const
	{
		for (; charIndex < input.size(); charIndex = ++start) {

			// check whether the substring is accepted (and populate everything given by reference)
			bool accepted = _check_accepted_substring(input, matchedStates, start, charIndex);

			// if the substring was accepted
			if (accepted) {
				substrings.push_back({ matchedStates, start, charIndex + 1 });

				// if the rest of the input is less than the current substring in length, leave
				if ((input.size() - start) < matchedStates.size())
					break;
			}

			matchedStates.resize(1);
		}

	}

	/**
	* @brief takes a set of matched substrings, finds the longest one and returns an FSMResult object representing that longest substring.
	* @return FSMResult object representing the longest matching substring from the set of substrings given to it.
	**/
	template<typename TransFuncT, typename InputT>
	inline FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_get_longest_substring_from_matched_sets(const InputT input, const std::vector<SubstringType>& substrings) const {

		const SubstringType* longest = nullptr;
		size_t size = 0;

		// choose the longest substring or the first of many having the same length
		for (const auto& substring : substrings) {
			// if the length of this substring is longer than that of the previously catched
			if (substring.size() > size) {
				longest = &substring;
				size = longest->size();
			}
		}

		size_t start = 0;
		size_t end = 0;

		if (longest) {
			start = longest->indecies.start;
			end = longest->indecies.end;
		}

		// get the final states we've reached
		const FSMStateSetType currState = longest->matchedStates.back();
		const FSMStateSetType finalStateSet = this->_get_final_states_from_state_set(currState);
		//assert("This set must contain at least a single final state" && finalStateSet.size());

		return FSMResult(true, finalStateSet, { start, end }, input);
	};

	template<typename TransFuncT, typename InputT>
	FSMStateSetType NonDeterFiniteAutomatan<TransFuncT, InputT>::_epsilon_closure(const FSMStateSetType& set) const
	{
		FSMStateSetType res{set};

		std::stack<FSMStateType> stack{};
		// initialize the stack
		for (auto s : set)
			stack.push(s);
		
		while (stack.size()) {
			// get the last state and pop it; we will get its closure now, so we will not need it in the future
			FSMStateType s = stack.top();
			stack.pop();

			// check if the state has at least one transition on epsilon
			FSMStateSetType epsilonTransitions = this->m_TransitionFunc(s, '\0');

			// if the set has at least a single epsilon-transition
			if (!epsilonTransitions.empty()) {
				/**
				* Push all the states in the epsilon transitions onto the stack.
				* We do this to consider whether the state itself has any epsilon transitions.
				* This applies the recursivness of the algorithm.
				* Before we push a state, we check to see if it is already in the set so that we don't consider the state again.
				* If we don't do that, we might end up with an infinite loop.
				*/ 
				for (const FSMStateType state : epsilonTransitions)
					if (!res.contains(state))
						stack.push(state);

				res.insert(epsilonTransitions.begin(), epsilonTransitions.end());
			};
			
		};
		
		return res;
	}
	
	/**
	* @brief simulate the NFA against `input` according to `mode`.
	* this function can throw an exception of type `UnrecognizedSimModeException` in case `mode` is not recognized.
	**/
	template<typename TransFuncT, typename InputT>
	inline FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::simulate(const InputT& input, FSM_MODE mode) const
	{
		switch (mode) {
		case FSM_MODE::MM_WHOLE_STRING:
			return this->_simulate_whole_string(input);
		case FSM_MODE::MM_LONGEST_PREFIX:
			return this->_simulate_longest_prefix(input);
		case FSM_MODE::MM_LONGEST_SUBSTRING:
			return this->_simulate_longest_substring(input);
		default:
			this->m_Logger.log(LoggerInfo::ERR_INVALID_ARG, "Unreachable: simulate() cannot reach this point. The provided mode is probably erraneous.");
			throw UnrecognizedSimModeException();
		}

	}
	
}
