#ifdef _WIN32
	#ifdef EXPORT_LIBRARY
		#define LIBRARY_API __declspec(dllexport)
	#else
		#define LIBRARY_API __declspec(dllimport)
	#endif
#else
	#define LIBRARY_API
#endif
