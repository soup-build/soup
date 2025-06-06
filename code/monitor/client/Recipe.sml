Name: 'Monitor.Client'
Description: 'The Detours dynamic library that is injected into a child process to intercept system calls.'
Language: 'C++|0'
Version: 1.0.0
Type: 'DynamicLibrary'
Defines: [
	# 'ENABLE_MONITOR_DEBUG'
	# 'TRACE_DETOUR_CLIENT'
]
Dependencies: {
	Runtime: [
		'mwasplund|Detours@4'
		'../shared/'
		'mwasplund|Opal@0'
	]
}