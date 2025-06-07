Version: 5
Closures: {
	Root: {
		'C++': {
			'Monitor.Client': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Monitor.Shared': { Version: '../shared/', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Detours': { Version: 4.0.13, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.12.0, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 0.15.3 }
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