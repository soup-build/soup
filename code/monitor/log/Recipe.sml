# A sample Monitor that logs all access to disk
Name: 'LogMonitor'
Language: 'C++|0'
Version: 1.0.0
Type: 'Executable'
Dependencies: {
	Runtime: [
		'mwasplund|Detours@4'
		'mwasplund|Opal@0'
		'../shared/'
	]
}