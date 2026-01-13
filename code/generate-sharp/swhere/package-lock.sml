Version: 5
Closures: {
	Root: {
		'C#': {
			Opal: { Version: '../opal/', Build: 'Build0', Tool: 'Tool0' }
			'Soup.Build.Utilities': { Version: '../utilities/', Build: 'Build1', Tool: 'Tool0' }
			Swhere: { Version: '../swhere', Build: 'Build0', Tool: 'Tool0' }
			'Swhere.Core': { Version: '../swhere.core/', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'Soup|CSharp': { Version: 0.17.0 }
		}
	}
	Build1: {
		Wren: {
			'Soup|CSharp': { Version: 0.17.0 }
			'Soup|CSharp.Nuget': { Version: 0.6.0 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.2.0 }
			'mwasplund|mkdir': { Version: 1.2.0 }
		}
	}
}