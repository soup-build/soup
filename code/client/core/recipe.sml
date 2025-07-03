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
	'source/library.cpp'
	'source/build/build-constants.cpp'
	'source/build/build-failed-exception.cpp'
	'source/build/build-history-checker.cpp'
	'source/build/dependency-target-set.cpp'
	'source/build/file-system-state.cpp'
	'source/build/ievaluate-engine.cpp'
	'source/build/known-language.cpp'
	'source/build/recipe-build-arguments.cpp'
	'source/build/macro-manager.cpp'
	'source/build/package-provider.cpp'
	'source/build/recipe-build-cache-state.cpp'
	'source/build/recipe-build-location-manager.cpp'
	'source/build/system-access-tracker.cpp'
	'source/local-user-config/local-user-config.cpp'
	'source/local-user-config/local-user-config-extensions.cpp'
	'source/local-user-config/sdk-config.cpp'
	'source/operation-graph/command-info.cpp'
	'source/operation-graph/generate-result.cpp'
	'source/operation-graph/generate-result-manager.cpp'
	'source/operation-graph/generate-result-reader.cpp'
	'source/operation-graph/generate-result-writer.cpp'
	'source/operation-graph/operation-graph.cpp'
	'source/operation-graph/operation-graph-manager.cpp'
	'source/operation-graph/operation-graph-reader.cpp'
	'source/operation-graph/operation-graph-writer.cpp'
	'source/operation-graph/operation-info.cpp'
	'source/operation-graph/operation-result.cpp'
	'source/operation-graph/operation-results.cpp'
	'source/operation-graph/operation-results-manager.cpp'
	'source/operation-graph/operation-results-reader.cpp'
	'source/operation-graph/operation-results-writer.cpp'
	'source/package/package-manager.cpp'
	'source/package-lock/package-lock.cpp'
	'source/package-lock/package-lock-extensions.cpp'
	'source/recipe/language-reference.cpp'
	'source/recipe/language-reference-parser.cpp'
	'source/recipe/package-identifier.cpp'
	'source/recipe/package-name.cpp'
	'source/recipe/package-reference.cpp'
	'source/recipe/recipe.cpp'
	'source/recipe/recipe-build-state-converter.cpp'
	'source/recipe/recipe-cache.cpp'
	'source/recipe/recipe-extensions.cpp'
	'source/recipe/recipe-sml.cpp'
	'source/recipe/recipe-value.cpp'
	'source/recipe/root-recipe.cpp'
	'source/recipe/root-recipe-extensions.cpp'
	'source/sml/sml.cpp'
	'source/sml/sml-parser.cpp'
	'source/utilities/handled-exception.cpp'
	'source/utilities/sequence-map.cpp'
	'source/value-table/value.cpp'
	'source/value-table/value-table-manager.cpp'
	'source/value-table/value-table-reader.cpp'
	'source/value-table/value-table-writer.cpp'
	'source/wren/wren-helpers.cpp'
	'source/wren/wren-host.cpp'
	'source/wren/wren-value-table.cpp'
]
Dependencies: {
	Build: [
		'mwasplund|Soup.Test.Cpp@0'
	]
	Runtime: [
		'mwasplund|Opal@0'
		'mwasplund|reflex@5'
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
		'tests/gen/main.cpp'
	]
	IncludePaths: [
		'tests/'
	]
}