Version: 5
Closures: {
	Root: {
		'C++': {
			'Samples.Cpp.ConsoleApplication': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|Cpp': { Version: '../../../../soup-cpp/code/extension/' }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
			'mwasplund|parse.module': { Version: '../../../soup/code/tools/parse-module/' }
		}
	}
}