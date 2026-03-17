module;

// Include all standard library headers in the global module
#include <string>

export module Sample.StaticLibrary.Library;

// Note: The namespace does not have to match the module name
export namespace Sample::StaticLibrary::Library
{
	class Helper
	{
	public:
		static std::string GetName()
		{
			return "Soup";
		}
	};
}