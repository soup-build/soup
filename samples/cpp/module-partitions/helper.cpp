module;

// Include all standard library headers in the global module
#include <string>

export module Samples.Cpp.ModulePartitions;
import :HelperB;
import :HelperC;

export class Helper
{
public:
	static std::string GetName()
	{
		std::string result;
		result += GetSourcePrefix();
		result += GetPackagesPostfix();
		return  result;
	}
};