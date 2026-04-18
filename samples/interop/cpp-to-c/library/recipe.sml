Name: 'samples-interop-cpp-to-c-library'
Language: 'C|0'
Version: 1.0.0
Type: 'DynamicLibrary'
Defines: [
	'EXPORT_LIBRARY'
]
IncludePaths: [
	'public/'
]
PublicHeaders: [
	{
		Root: 'public/'
		Files: [
			'library.h'
		]
	}
]