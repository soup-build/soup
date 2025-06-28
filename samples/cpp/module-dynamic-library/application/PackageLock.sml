Version: 5
Closures: {
	Root: {
		'C++': {
			'Samples.Cpp.ModuleDynamicLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Samples.Cpp.ModuleDynamicLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
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