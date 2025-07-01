Version: 5
Closures: {
	Root: {
		C: {
			'Samples.C.SimpleBuildExtension.Executable': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Samples.C.BuildExtension.Extension': { Version: '../extension/' }
			'Soup|C': { Version: 0.4.1 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
			'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
		}
	}
}