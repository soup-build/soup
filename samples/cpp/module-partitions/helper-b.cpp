module;

// Include all standard library headers in the global module
#include <string>

export module Samples.Cpp.ModulePartitions:HelperB;
import :HelperA;

export std::string_view GetSourcePrefix()
{
	return Truncate("Source", 3);
}