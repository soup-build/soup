Name: 'Soup.ParseModules'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
Defines: [
	'SHOW_TOKENS'
]
Interface: 'Module.cpp'
Partitions: [
	{ Source: 'parser/ModuleParser.cpp' }
]
Source: [
	'Main.cpp'
]
Dependencies: {
	Runtime: [
		'mwasplund|Opal@0'
		'mwasplund|reflex@1'
	]
}