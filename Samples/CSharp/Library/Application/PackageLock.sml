Version: 5
Closures: {
	Root: {
		'C#': {
			'Samples.CSharp.Library.Library': { Version: '../Library/', Build: 'Build0', Tool: 'Tool0' }
			'Samples.CSharp.Library.ConsoleApplication': { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'mwasplund|Soup.CSharp': { Version: 0.15.0 }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
		}
	}
}