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
Source: [
	'source/Module.cpp'
	'source/build/BuildConstants.cpp'
	'source/build/BuildFailedException.cpp'
	'source/build/BuildHistoryChecker.cpp'
	'source/build/DependencyTargetSet.cpp'
	'source/build/FileSystemState.cpp'
	'source/build/IEvaluateEngine.cpp'
	'source/build/KnownLanguage.cpp'
	'source/build/RecipeBuildArguments.cpp'
	'source/build/MacroManager.cpp'
	'source/build/PackageProvider.cpp'
	'source/build/RecipeBuildCacheState.cpp'
	'source/build/RecipeBuildLocationManager.cpp'
	'source/build/SystemAccessTracker.cpp'
	'source/local-user-config/LocalUserConfig.cpp'
	'source/local-user-config/LocalUserConfigExtensions.cpp'
	'source/local-user-config/SDKConfig.cpp'
	'source/operation-graph/CommandInfo.cpp'
	'source/operation-graph/GenerateResult.cpp'
	'source/operation-graph/GenerateResultManager.cpp'
	'source/operation-graph/GenerateResultReader.cpp'
	'source/operation-graph/GenerateResultWriter.cpp'
	'source/operation-graph/OperationGraph.cpp'
	'source/operation-graph/OperationGraphManager.cpp'
	'source/operation-graph/OperationGraphReader.cpp'
	'source/operation-graph/OperationGraphWriter.cpp'
	'source/operation-graph/OperationInfo.cpp'
	'source/operation-graph/OperationResult.cpp'
	'source/operation-graph/OperationResults.cpp'
	'source/operation-graph/OperationResultsManager.cpp'
	'source/operation-graph/OperationResultsReader.cpp'
	'source/operation-graph/OperationResultsWriter.cpp'
	'source/package/PackageManager.cpp'
	'source/package-lock/PackageLock.cpp'
	'source/package-lock/PackageLockExtensions.cpp'
	'source/recipe/LanguageReference.cpp'
	'source/recipe/LanguageReferenceParser.cpp'
	'source/recipe/PackageIdentifier.cpp'
	'source/recipe/PackageName.cpp'
	'source/recipe/PackageReference.cpp'
	'source/recipe/Recipe.cpp'
	'source/recipe/RecipeBuildStateConverter.cpp'
	'source/recipe/RecipeCache.cpp'
	'source/recipe/RecipeExtensions.cpp'
	'source/recipe/RecipeSML.cpp'
	'source/recipe/RecipeValue.cpp'
	'source/recipe/RootRecipe.cpp'
	'source/recipe/RootRecipeExtensions.cpp'
	'source/sml/SML.cpp'
	'source/sml/SMLParser.cpp'
	'source/utilities/HandledException.cpp'
	'source/utilities/SequenceMap.cpp'
	'source/value-table/Value.cpp'
	'source/value-table/ValueTableManager.cpp'
	'source/value-table/ValueTableReader.cpp'
	'source/value-table/ValueTableWriter.cpp'
	'source/wren/WrenHelpers.cpp'
	'source/wren/WrenHost.cpp'
	'source/wren/WrenValueTable.cpp'
]
Dependencies: {
	Build: [
		# 'mwasplund|Soup.Test.Cpp@0'
	]
	Runtime: [
		'mwasplund|Opal@0'
		'mwasplund|reflex@1'
		'../../monitor/host/'
		'[C]mwasplund|wren@1'
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