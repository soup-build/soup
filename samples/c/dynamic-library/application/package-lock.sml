Version: 5
Closures: {
	Root: {
		C: {
			'Samples.C.DynamicLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Samples.C.DynamicLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|C': { Version: 0.4.1 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
		}
	}
}