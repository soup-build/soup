Version: 5
Closures: {
	Root: {
		Wren: {
			'Samples.Cpp.BuildExtension.Extension': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Soup|Build.Utils': { Version: 0.9.0, Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Wren': { Version: 0.5.2 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.2.0 }
			'mwasplund|mkdir': { Version: 1.2.0 }
		}
	}
}