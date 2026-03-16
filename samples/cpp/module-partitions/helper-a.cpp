module;

// Include all standard library headers in the global module
#include <string>

export module Sample.ModulePartitions:HelperA;

export std::string_view Truncate(std::string_view value, size_t length)
{
	return value.substr(0, length);
}