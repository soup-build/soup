Version: 5
Closures: {
	Root: {
		C: {
			'mwasplund|libseccomp': { Version: 2.6.0, Build: 'Build2', Tool: 'Tool0' }
			'mwasplund|wren': { Version: 1.2.0, Build: 'Build2', Tool: 'Tool0' }
		}
		'C++': {
			'Monitor.Host': { Version: '../../monitor/host/', Build: 'Build0', Tool: 'Tool0' }
			'Monitor.Shared': { Version: '../../monitor/shared/', Build: 'Build0', Tool: 'Tool0' }
			'Soup.Core': { Version: '../../client/core/', Build: 'Build1', Tool: 'Tool0' }
			'mwasplund|CryptoPP': { Version: 1.2.7, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Detours': { Version: 4.0.14, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Opal': { Version: 0.12.4, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|Soup.Test.Assert': { Version: 0.5.0, Build: 'Build0', Tool: 'Tool0' }
			'mwasplund|reflex': { Version: 5.5.2, Build: 'Build0', Tool: 'Tool0' }
			'print.graph': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: 0.16.0 }
		}
	}
	Build1: {
		Wren: {
			'Soup|Cpp': { Version: 0.16.0 }
			'mwasplund|Soup.Test.Cpp': { Version: 0.15.1 }
		}
	}
	Build2: {
		Wren: {
			'Soup|C': { Version: 0.5.0 }
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