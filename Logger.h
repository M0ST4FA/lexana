#pragma once

#include <iostream>
#include <cstdarg>
#include <format>
#include <source_location>

#include "common.h"
//#define _TRACE

namespace m0st4fa {
	
	// DECLARATIONS
	enum class ERROR_TYPE {
		ET_INVALID_ARGUMENT,
		ET_INVALID_LEXEME,
		ET_PROD_BODY_EMPTY,
		ET_UNEXCPECTED_TOKEN,
		ET_ERR_RECOVERY_LIMIT_EXCEEDED,
		ET_MISSING_VAL,
		ET_INVALID_VAL,
		ET_STACK_UNDERFLOW,
		ET_UNACCEPTED_STRING,
		ET_INVALID_TABLE_ENTRY,
		ET_UNACCEPTED_GRAMMAR,
		ET_INVALID_FLAGS,
		ET_ERROR_TYPE_COUNT
	};

	enum class LOG_LEVEL {
		LL_FATAL_ERROR,
		LL_ERROR,
		LL_WARRNING,
		LL_INFO,
		LL_DEBUG,
		LL_LOG_LEVEL_COUNT
	};

	struct LoggerInfo {
		LOG_LEVEL level;
		union {
			int noVal = 0;
			ERROR_TYPE errorType;
		} info;

		static const LoggerInfo WARNING, INFO, DEBUG, FATAL_ERROR, ERR_INVALID_ARG, ERR_INVALID_LEXEME, ERR_EMPTY_PROD_BODY, ERR_UNXPCTED_TOKEN, ERR_RECOV_LIMIT_EXCEEDED, ERR_MISSING_VAL, ERR_INVALID_VAL, ERR_STACK_UNDERFLOW, ERR_UNACCEPTED_STRING, ERR_INVALID_TABLE_ENTRY, ERR_UNACCEPTED_GRAMMAR, ERR_INVALID_FLAGS;
	};
	
	class Logger {

		// static assertions to remind me of making some changing related to the number of 
		// these enumerators
		static_assert((int)LOG_LEVEL::LL_LOG_LEVEL_COUNT == 5 && "Change LOG_LEVEL_STRING");
		static_assert((int)ERROR_TYPE::ET_ERROR_TYPE_COUNT == 12 && "Change ERROR_TYPE_NAMES");

		static constexpr const volatile char* LOG_LEVEL_STRING[(int)LOG_LEVEL::LL_LOG_LEVEL_COUNT] = {
			"FATAL ERROR",
			"ERROR",
			"WARNING",
			"INFO",
			"DEBUG",
		};

		static constexpr const volatile char* ERROR_TYPE_NAMES[(int)ERROR_TYPE::ET_ERROR_TYPE_COUNT] = {
			"Invalid Argument",
			"Invalid Lexeme",
			"Empty Production Body",
			"Unexpected Token",
			"Error Recovery Limit Exceeded",
			"Value Missing",
			"Invalid Value",
			"Stack Underflow",
			"Unaccepted String",
			"Invalid Table Entry",
			"Unaccepted Grammar",
			"Invalid Flags"
		};

	public:

		void log(const LoggerInfo, const std::string&, std::source_location = std::source_location::current()) const;
		
		void logDebug(const std::string&, std::source_location = std::source_location::current()) const;

		inline std::string getCurrSourceLocation(std::source_location location = std::source_location::current()) const {
			std::string messageStr;
			messageStr += std::string("\nFile Name: ") + location.file_name() + std::string("\n");
			messageStr += std::string("Line: ") + std::to_string(location.line()) + std::string(", Character: ") + std::to_string(location.column()) + "\n";
			messageStr += std::string("Function: ") + location.function_name() + "\n";

			return messageStr;
		};

	};


}
