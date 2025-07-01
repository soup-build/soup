Version: 5
Closures: {
	Root: {
		'C#': {
			'Samples.CSharp.BuildExtension.Executable': { Version: '../Executable', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Samples.CSharp.BuildExtension.Extension': { Version: '../extension/' }
			'Soup|CSharp': { Version: 0.15.3 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
			'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
		}
	}
}