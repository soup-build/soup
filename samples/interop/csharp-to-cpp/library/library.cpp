#include "library.h"

extern "C"
{
	LIBRARY_API const char* GetName()
	{
		return "Soup";
	}
}