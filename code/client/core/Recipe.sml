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
	{ Source: 'source/build/BuildHistoryChecker.cpp', Imports: [ 'source/build/FileSystemState.cpp' ] }
	{ Source: 'source/build/DependencyTargetSet.cpp' }
	{ Source: 'source/build/FileSystemState.cpp' }
	{ Source: 'source/build/IEvaluateEngine.cpp', Imports: [ 'source/operation-graph/OperationGraph.cpp', 'source/operation-graph/OperationResults.cpp' ] }
	{ Source: 'source/build/KnownLanguage.cpp' }
	{ Source: 'source/build/RecipeBuildArguments.cpp', Imports: [ 'source/value-table/Value.cpp' ] }
	{ Source: 'source/build/MacroManager.cpp' }
	{ Source: 'source/build/PackageProvider.cpp', Imports: [ 'source/recipe/PackageName.cpp', 'source/recipe/PackageReference.cpp','source/recipe/Recipe.cpp', 'source/value-table/Value.cpp' ] }
	{ Source: 'source/build/RecipeBuildCacheState.cpp' }
	{ Source: 'source/build/RecipeBuildLocationManager.cpp', Imports: [ 'source/build/KnownLanguage.cpp', 'source/recipe/PackageName.cpp', 'source/recipe/Recipe.cpp', 'source/recipe/RecipeCache.cpp', 'source/recipe/RootRecipeExtensions.cpp', 'source/value-table/Value.cpp', 'source/value-table/ValueTableWriter.cpp', 'source/utilities/HandledException.cpp' ] }
	{ Source: 'source/build/SystemAccessTracker.cpp' }
	{ Source: 'source/local-user-config/LocalUserConfig.cpp', Imports: [ 'source/local-user-config/SDKConfig.cpp' ] }
	{ Source: 'source/local-user-config/LocalUserConfigExtensions.cpp', Imports: [ 'source/local-user-config/LocalUserConfig.cpp', 'source/recipe/RecipeSML.cpp' ] }
	{ Source: 'source/local-user-config/SDKConfig.cpp', Imports: [ 'source/recipe/RecipeValue.cpp' ] }
	{ Source: 'source/operation-graph/CommandInfo.cpp' }
	{ Source: 'source/operation-graph/GenerateResult.cpp', Imports: [ 'source/operation-graph/CommandInfo.cpp', 'source/operation-graph/OperationGraph.cpp', 'source/operation-graph/OperationProxyInfo.cpp' ] }
	{ Source: 'source/operation-graph/GenerateResultManager.cpp', Imports: [ 'source/operation-graph/GenerateResultReader.cpp', 'source/operation-graph/GenerateResultWriter.cpp' ] }
	{ Source: 'source/operation-graph/GenerateResultReader.cpp', Imports: [ 'source/operation-graph/GenerateResult.cpp', 'source/operation-graph/OperationGraphReader.cpp', 'source/build/FileSystemState.cpp' ] }
	{ Source: 'source/operation-graph/GenerateResultWriter.cpp', Imports: [ 'source/operation-graph/GenerateResult.cpp', 'source/operation-graph/OperationGraphWriter.cpp', 'source/build/FileSystemState.cpp' ] }
	{ Source: 'source/operation-graph/OperationGraph.cpp', Imports: [ 'source/operation-graph/CommandInfo.cpp', 'source/operation-graph/OperationInfo.cpp' ] }
	{ Source: 'source/operation-graph/OperationGraphManager.cpp', Imports: [ 'source/operation-graph/OperationGraphReader.cpp', 'source/operation-graph/OperationGraphWriter.cpp' ] }
	{ Source: 'source/operation-graph/OperationGraphReader.cpp', Imports: [ 'source/operation-graph/OperationGraph.cpp', 'source/build/FileSystemState.cpp' ] }
	{ Source: 'source/operation-graph/OperationGraphWriter.cpp', Imports: [ 'source/operation-graph/OperationGraph.cpp', 'source/build/FileSystemState.cpp' ] }
	{ Source: 'source/operation-graph/OperationInfo.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/CommandInfo.cpp' ] }
	{ Source: 'source/operation-graph/OperationProxyInfo.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/CommandInfo.cpp' ] }
	{ Source: 'source/operation-graph/OperationResult.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/OperationInfo.cpp' ] }
	{ Source: 'source/operation-graph/OperationResults.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/OperationInfo.cpp', 'source/operation-graph/OperationResult.cpp' ] }
	{ Source: 'source/operation-graph/OperationResultsManager.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/OperationResultsReader.cpp', 'source/operation-graph/OperationResultsWriter.cpp' ] }
	{ Source: 'source/operation-graph/OperationResultsReader.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/OperationResults.cpp' ] }
	{ Source: 'source/operation-graph/OperationResultsWriter.cpp', Imports: [ 'source/build/FileSystemState.cpp', 'source/operation-graph/OperationResults.cpp' ] }
	{ Source: 'source/package/PackageManager.cpp', Imports: [ 'source/utilities/HandledException.cpp' ] }
	{ Source: 'source/package-lock/PackageLock.cpp', Imports: [ 'source/recipe/PackageName.cpp', 'source/recipe/PackageReference.cpp', 'source/recipe/RecipeValue.cpp' ] }
	{ Source: 'source/package-lock/PackageLockExtensions.cpp', Imports: [ 'source/package-lock/PackageLock.cpp', 'source/recipe/RecipeSML.cpp' ] }
	{ Source: 'source/recipe/LanguageReference.cpp' }
	{ Source: 'source/recipe/PackageIdentifier.cpp', Imports: [ 'source/recipe/PackageName.cpp' ] }
	{ Source: 'source/recipe/PackageName.cpp' }
	{ Source: 'source/recipe/PackageReference.cpp', Imports: [ 'source/recipe/PackageIdentifier.cpp' ] }
	{ Source: 'source/recipe/Recipe.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp', 'source/recipe/RecipeValue.cpp' ] }
	{ Source: 'source/recipe/RecipeBuildStateConverter.cpp', Imports: [ 'source/recipe/PackageReference.cpp', 'source/recipe/RecipeValue.cpp','source/value-table/Value.cpp'  ] }
	{ Source: 'source/recipe/RecipeCache.cpp', Imports: [ 'source/recipe/Recipe.cpp', 'source/recipe/RecipeExtensions.cpp', 'source/recipe/RootRecipe.cpp', 'source/recipe/RootRecipeExtensions.cpp' ] }
	{ Source: 'source/recipe/RecipeExtensions.cpp', Imports: [ 'source/recipe/PackageReference.cpp', 'source/recipe/Recipe.cpp', 'source/recipe/RecipeSML.cpp' ] }
	{ Source: 'source/recipe/RecipeSML.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp', 'source/recipe/RecipeValue.cpp', 'source/sml/SML.cpp', 'source/utilities/SequenceMap.cpp' ] }
	{ Source: 'source/recipe/RecipeValue.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp', 'source/utilities/SequenceMap.cpp' ] }
	{ Source: 'source/recipe/RootRecipe.cpp', Imports: [ 'source/recipe/RecipeValue.cpp' ] }
	{ Source: 'source/recipe/RootRecipeExtensions.cpp', Imports: [ 'source/recipe/RootRecipe.cpp', 'source/recipe/RecipeSML.cpp' ] }
	{ Source: 'source/sml/SML.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp', 'source/utilities/SequenceMap.cpp' ] }
	{ Source: 'source/utilities/HandledException.cpp' }
	{ Source: 'source/utilities/SequenceMap.cpp' }
	{ Source: 'source/value-table/Value.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp' ] }
	{ Source: 'source/value-table/ValueTableManager.cpp', Imports: [ 'source/value-table/Value.cpp',  'source/value-table/ValueTableReader.cpp',  'source/value-table/ValueTableWriter.cpp' ] }
	{ Source: 'source/value-table/ValueTableReader.cpp', Imports: [ 'source/recipe/LanguageReference.cpp', 'source/recipe/PackageReference.cpp', 'source/value-table/Value.cpp' ] }
	{ Source: 'source/value-table/ValueTableWriter.cpp', Imports: [ 'source/value-table/Value.cpp' ] }
	{ Source: 'source/wren/WrenHelpers.cpp' }
	{ Source: 'source/wren/WrenHost.cpp', Imports: [ 'source/sml/SML.cpp', 'source/wren/WrenHelpers.cpp' ] }
	{ Source: 'source/wren/WrenValueTable.cpp', Imports: [ 'source/value-table/Value.cpp', 'source/wren/WrenHelpers.cpp' ] }
]
Dependencies: {
	Build: [
		# 'mwasplund|Soup.Test.Cpp@0'
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