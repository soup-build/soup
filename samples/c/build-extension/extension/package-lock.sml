Version: 5
Closures: {
	Root: {
		Wren: {
			'Samples.C.BuildExtension.Extension': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Soup|Build.Utils': { Version: 0.9.0, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Wren': { Version: 0.5.3 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.2.0 }
			'mwasplund|mkdir': { Version: 1.2.0 }
		}
	}
}