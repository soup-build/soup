Version: 5
Closures: {
	Root: {
		'C++': {
			'Samples.SimpleBuildExtension.Executable': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Samples.Cpp.BuildExtension.Extension': { Version: '../extension/' }
			'Soup|Cpp': { Version: 'C:/Users/mwasp/dev/repos/soup-cpp/code/extension/' }
		}
	}
	Tool0: {
		'C++': {
			'Samples.SimpleBuildExtension.Tool': { Version: '../tool/' }
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
			'mwasplund|parse.modules': { Version: 1.1.0 }
		}
	}
}