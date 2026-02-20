Name: 'Soup'
Version: 0.43.1
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
		# TODO: 'Soup.Test.Cpp@0.4.0'
	]
	Test: [
		# TODO: 'Soup.Test.Assert@0.3.0'
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