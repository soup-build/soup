Version: 5
Closures: {
	Root: {
		'C++': {
			copy: { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|copy': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 0.15.4 }
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