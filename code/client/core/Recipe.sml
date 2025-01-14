Name: 'Soup.Core'
Language: 'C++|0'
Version: 0.1.1
Defines: [
	# 'TRACE_SYSTEM_ACCESS'
	# 'TRACE_FILE_SYSTEM_STATE'
]
IncludePaths: [
	'source/'
]
Interface: 'source/Module.cpp'
Source: [
	'source/recipe/LanguageReferenceParser.cpp'
	'source/sml/SMLParser.cpp'
]
Partitions: [
	{ Source: 'source/build/BuildConstants.cpp' }
	{ Source: 'source/build/BuildFailedException.cpp' }
	{ Source: 'source/build/DependencyTargetSet.cpp' }
	{ Source: 'source/build/FileSystemState.cpp' }
	{ Source: 'source/build/KnownLanguage.cpp' }
	{ Source: 'source/build/MacroManager.cpp' }
	{ Source: 'source/build/SystemAccessTracker.cpp' }
	{ Source: 'source/operation-graph/CommandInfo.cpp' }
	{ Source: 'source/recipe/LanguageReference.cpp' }
	{ Source: 'source/recipe/PackageIdentifier.cpp', Imports: [ 'source/recipe/PackageName.cpp' ] }
	{ Source: 'source/recipe/PackageName.cpp' }
	{ Source: 'source/recipe/PackageReference.cpp', Imports: [ 'source/recipe/PackageIdentifier.cpp' ] }
	{ Source: 'source/utilities/HandledException.cpp' }
	{ Source: 'source/utilities/SequenceMap.cpp' }
	{ Source: 'source/value-table/Value.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp' ] }
]
Dependencies: {
	Build: [
		'mwasplund|Soup.Test.Cpp@0'
	]
	Runtime: [
		'mwasplund|Opal@0'
		'mwasplund|reflex@1'
		'../../monitor/host/'
		'mwasplund|wren@1'
		'mwasplund|CryptoPP@1'
	]
	Test: [
		'mwasplund|Soup.Test.Assert@0'
	]
}
Tests: {
	Source: [
		'tests/gen/Main.cpp'
	]
	IncludePaths: [
		'tests/'
	]
}