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
	'source/**/*.cpp'
]
Dependencies: {
	Build: [
		'mwasplund|soup-test-cpp@0'
	]
	Runtime: [
		'mwasplund|opal@0'
		'mwasplund|reflex@5'
		'../../monitor/host/'
		'[C]mwasplund|wren@1'
		'mwasplund|cryptopp@1'
	]
	Test: [
		'mwasplund|soup-test-assert@0'
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