#include "Logger.h"

namespace m0st4fa {
	
	// LoggerInfo CONSTANTS
	const LoggerInfo LoggerInfo::WARNING = LoggerInfo{ LOG_LEVEL::LL_WARRNING };
	const LoggerInfo LoggerInfo::INFO = LoggerInfo{ LOG_LEVEL::LL_INFO };
	const LoggerInfo LoggerInfo::DEBUG = LoggerInfo{ LOG_LEVEL::LL_DEBUG };
	const LoggerInfo LoggerInfo::FATAL_ERROR = LoggerInfo{ LOG_LEVEL::LL_FATAL_ERROR };
	const LoggerInfo LoggerInfo::ERR_INVALID_ARG = LoggerInfo{ LOG_LEVEL::LL_ERROR, { .errorType = ERROR_TYPE::ET_INVALID_ARGUMENT} };
	const LoggerInfo LoggerInfo::ERR_INVALID_LEXEME = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_INVALID_LEXEME} };
	const LoggerInfo LoggerInfo::ERR_EMPTY_PROD_BODY = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_PROD_BODY_EMPTY} };
	const LoggerInfo LoggerInfo::ERR_UNXPCTED_TOKEN = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_UNEXCPECTED_TOKEN} };
	const LoggerInfo LoggerInfo::ERR_RECOV_LIMIT_EXCEEDED = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_ERR_RECOVERY_LIMIT_EXCEEDED} };
	const LoggerInfo LoggerInfo::ERR_MISSING_VAL = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_MISSING_VAL} };
	const LoggerInfo LoggerInfo::ERR_INVALID_VAL = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_INVALID_VAL} };
	const LoggerInfo LoggerInfo::ERR_STACK_UNDERFLOW = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_STACK_UNDERFLOW} };
	const LoggerInfo LoggerInfo::ERR_UNACCEPTED_STRING = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_UNACCEPTED_STRING} };
	const LoggerInfo LoggerInfo::ERR_INVALID_TABLE_ENTRY = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_INVALID_TABLE_ENTRY} };
	const LoggerInfo LoggerInfo::ERR_UNACCEPTED_GRAMMAR = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_UNACCEPTED_GRAMMAR} };
	const LoggerInfo LoggerInfo::ERR_INVALID_FLAGS = LoggerInfo{ LOG_LEVEL::LL_ERROR, {.errorType = ERROR_TYPE::ET_INVALID_FLAGS} };

	// IMPLEMENTATIONS OF Logger FUNCTIONS
	void Logger::log(LoggerInfo loggerInfo, const std::string& message, std::source_location location) const
	{

		// if logging a debug message
		if (loggerInfo.level == LOG_LEVEL::LL_DEBUG) {
			logDebug(message, location);
			return;
		};

		// if logging any other message
		const std::string logLevelStr = (const char *)LOG_LEVEL_STRING[static_cast<int>(loggerInfo.level)];
		const std::string errorTypeStr = (const char* )ERROR_TYPE_NAMES[static_cast<int>(loggerInfo.info.errorType)];

		std::string messageStr;
		
		if (loggerInfo.level == LOG_LEVEL::LL_ERROR)
			messageStr += std::format(ANSI_ERR_COLOR"[{:s} => {:s}]: {:s}\n" ANSI_RESET_ALL, logLevelStr, errorTypeStr, message);
		else if (loggerInfo.level == LOG_LEVEL::LL_FATAL_ERROR)
			messageStr += std::format(ANSI_ERR_COLOR"[{:s}]: {:s}\n" ANSI_RESET_ALL, logLevelStr, message);
		else
			messageStr += std::format(ANSI_INFO_COLOR"[{:s}]: {:s}\n" ANSI_RESET_ALL, logLevelStr, message);

#ifdef _DEBUG 
#ifdef _TRACE
		messageStr += this->getCurrSourceLocation(location);
#endif
#endif

		switch (loggerInfo.level) {
		case LOG_LEVEL::LL_FATAL_ERROR:
		case LOG_LEVEL::LL_ERROR:
			std::cerr << messageStr << "\n";
			return;
		case LOG_LEVEL::LL_WARRNING:
			std::cout << messageStr << "\n";
			return;
		case LOG_LEVEL::LL_INFO:
			std::clog << messageStr << "\n";
			return;
		default:
			std::cerr << "Unknown log level: " << static_cast<int>(loggerInfo.level) << "\n";
			throw std::invalid_argument("Unknown log level.");
		};

	}

	void Logger::logDebug(const std::string& message, std::source_location location) const
	{
#ifdef _DEBUG
		const std::string logLevelStr = (const char* ) LOG_LEVEL_STRING[(int)LOG_LEVEL::LL_DEBUG];
		std::string messageStr = std::format(ANSI_DEBUG_COLOR"[{:s}]: {:s}" ANSI_RESET_ALL, logLevelStr, message);
		
#ifdef _TRACE
		messageStr += this->getCurrSourceLocation(location);
#endif
		
		std::cout << messageStr << "\n";
#endif
		return;
	}

}
