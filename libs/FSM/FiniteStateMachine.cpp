
#include "FiniteStateMachine.h"

std::ostream& m0st4fa::operator<<(const std::ostream& os, const FSMResult& result)
{
	Logger logger;
	
	std::string temp = "{ ";
	temp += std::to_string(*result.finalState.begin());
	for (auto s : result.finalState) {
		if (s == *result.finalState.begin())
			continue;
		
		temp += (", " + std::to_string(s));
	}
	temp += " }";

	IndexType startIndex = result.indecies.start;
	IndexType endIndex = result.indecies.end;

	logger.log(LoggerInfo::INFO, std::format("Accepted string: {}\nIndecies of the match: {{{}, {}}}\nFinal states reached: {}",
		result.accepted, startIndex, endIndex, temp.data()));
	std::string_view temp2{result.input.data() + startIndex, endIndex};
	logger.logDebug(std::format("Matched string: {}\n", temp2));

	return std::cout;
}

