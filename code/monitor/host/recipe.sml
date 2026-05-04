Name: 'monitor-host'
Language: 'C++|0'
Version: 1.0.0
Defines: [
	# 'TRACE_DETOUR_SERVER'
	# 'TRACE_MONITOR_HOST'
]
Dependencies: {
	Runtime: [
		'mwasplund|detours@4'
		'[C]mwasplund|libseccomp@2.5'
		'mwasplund|opal@0'
		'../shared/'
	]
}
