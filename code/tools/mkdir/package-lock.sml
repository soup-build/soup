Version: 5
Closures: {
	Root: {
		'C++': {
			mkdir: { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|mkdir': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.12.4, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 0.16.0 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.2.0 }
			'mwasplund|mkdir': { Version: 1.2.0 }
			'mwasplund|parse.modules': { Version: 1.2.0 }
		}
	}
}