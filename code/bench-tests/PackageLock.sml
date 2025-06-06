Version: 5
Closures: {
	Root: {
		'C++': {
			'Monitor.Host': { Version: '../monitor/host/', Build: 'Build0', Tool: 'Tool0' }
			'Monitor.Shared': { Version: '../monitor/shared/', Build: 'Build0', Tool: 'Tool0' }
			'Soup.BenchTests': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Soup.Core': { Version: '../client/core/', Build: 'Build1', Tool: 'Tool0' }
			'mwasplund|CryptoPP': { Version: 1.2.5, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Detours': { Version: 4.0.13, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.12.0, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Soup.Test.Assert': { Version: 0.4.2, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|reflex': { Version: 5.5.0, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|wren': { Version: 1.1,0, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 0.15.3 }
		}
	}
	Build1: {
		Wren: {
			'Soup|Cpp': { Version: 0.15.3 }
			'mwasplund|Soup.Test.Cpp': { Version: 0.11.0 }
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