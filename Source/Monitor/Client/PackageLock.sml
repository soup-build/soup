Version: 5
Closures: {
	Root: {
		'C++': {
			'Monitor.Client': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Monitor.Shared': { Version: '../Shared/', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Detours': { Version: 4.0.12, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.11.5, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'mwasplund|Soup.Cpp': { Version: 0.13.2 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
		}
	}
}