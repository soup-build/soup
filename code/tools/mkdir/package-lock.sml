Version: 5
Closures: {
	Root: {
		'C++': {
			mkdir: { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|mkdir': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.12.1, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 'C:/Users/mwasp/dev/repos/soup-cpp/code/extension/' }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
			'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
			'mwasplund|parse.modules': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/parse-modules/' }
		}
	}
}