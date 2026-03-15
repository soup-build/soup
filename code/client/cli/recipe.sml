Name: 'soup'
Version: 0.45.0
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
		'../../generate/'
		'../../monitor/host/'
	]
	Build: [
		# TODO: 'soup-test-cpp@0.4.0'
	]
	Test: [
		# TODO: 'soup-test-assert@0.3.0'
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