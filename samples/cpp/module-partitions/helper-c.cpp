module;

// Include all standard library headers in the global module
#include <string>

export module ModulePartitions:HelperC;
import :HelperA;

export std::string_view GetPackagesPostfix()
{
	return Truncate("packages", 1);
}