#include <string>
#include <set>
#include <type_traits>
#include <concepts>
#include <format>

#include "common.h"
#include "Logger.h"

namespace m0st4fa {

	inline namespace utility {
		// STRING
		std::string toString(const std::source_location& srcloc, bool full) {
			std::string msg;

			msg += full ? (srcloc.file_name()) : "";
			msg += std::format("(line: {}, char: {}): ", srcloc.line(), srcloc.column());
			msg += full ? (srcloc.function_name() + std::string("()")) : std::string("");

			return msg;
		}

		// INTEGER
		size_t pow(size_t base, size_t p) {
			if (p == 0)
				return 1;

			size_t res = base;

			for (size_t i = 1; p > i; i++)
				res *= res;

			return res;
		}

		size_t toInteger(const std::string& str, bool atBeginning) {
			// find the first integer
			auto itr = std::find_if(str.begin(), str.end(), isalnum);

			if (itr == str.end()) {
				Logger logger;

				std::string msg = std::format("Could not convert the string `{}` into integer (Could not even extract an integer from it.)", str);

				logger.log(LoggerInfo::ERR_INVALID_VAL, msg);
				throw std::logic_error(msg);
			}

			size_t res = 0;

			for (; itr < str.end(); itr++) {
				const char currChar = *itr;

				// if the current character is not numeric
				if (not isalnum(currChar))
					break;

				size_t numVal = ((size_t)currChar - '0');
				res += numVal;
				res *= 10;
			}

			return res / 10;
		}
	}
}