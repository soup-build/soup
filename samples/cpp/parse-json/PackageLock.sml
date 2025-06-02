Version: 5
Closures: {
	Root: {
		'C++': {
			'Samples.Cpp.ParseJson': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|json11': { Version: '../../../../json11/', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: '../../../../soup-cpp/code/extension/' }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
			'mwasplund|parse.modules': { Version: 1.1.0 }
		}
	}
}