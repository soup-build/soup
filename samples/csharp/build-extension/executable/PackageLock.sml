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
			'Soup|CSharp': { Version: 0.15.2 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
		}
	}
}