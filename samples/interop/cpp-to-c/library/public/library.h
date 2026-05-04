#ifdef _WIN32
	#ifdef EXPORT_LIBRARY
		#define LIBRARY_API __declspec(dllexport)
	#else
		#define LIBRARY_API __declspec(dllimport)
	#endif
#else
	#define LIBRARY_API
#endif

// When the C++ code includes the shared header we need
// to maintain the C ABI
#ifdef __cplusplus
extern "C"
{
#endif

LIBRARY_API const char* GetName();

#ifdef __cplusplus
}
#endif