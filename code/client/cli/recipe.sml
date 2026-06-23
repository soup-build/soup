Name: 'soup'
Version: 0.48.0
Language: 'C++|0'
Type: 'Executable'
Source: [
	'source/**/*.cpp'
]
IncludePaths: [
	'source/commands/'
	'source/options/'
]
Dependencies: {
	# Ensure the core build extensions are runtime dependencies
	Runtime: [
		'../core/'
		'../view/'
		'../../generate/'
		'../../monitor/host/'
	]
	Build: [
		# 'soup-test-cpp@0'
	]
	Test: [
		# 'mwasplund|soup-test-assert@0'
	]
}
Tests: {
	Source: [
		'tests/**/*.cpp'
	]
	IncludePaths: [
		'tests/'
	]
}
