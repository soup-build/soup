Version: 5
Closures: {
	Root: {
		C: {
			'mwasplund|libseccomp': { Version: 2.5.8, Build: 'Build3', Tool: 'Tool0' }
			'mwasplund|wren': { Version: 1.1.0, Build: 'Build3', Tool: 'Tool0' }
		}
		'C#': {
			Opal: { Version: '../../generate-sharp/opal/', Build: 'Build0', Tool: 'Tool0' }
			'Soup.Bootstrap': { Version: './', Build: 'Build0', Tool: 'Tool0' }
			'Soup.Tools': { Version: '../../generate-sharp/soup-tools/', Build: 'Build1', Tool: 'Tool0' }
			'Soup.Utilities': { Version: '../../generate-sharp/utilities/', Build: 'Build1', Tool: 'Tool0' }
		}
		'C++': {
			'Monitor.Host': { Version: '../../monitor/host/', Build: 'Build2', Tool: 'Tool1' }
			'Monitor.Shared': { Version: '../../monitor/shared/', Build: 'Build2', Tool: 'Tool1' }
			'Soup.Core': { Version: '../../client/core/', Build: 'Build2', Tool: 'Tool1' }
			SoupTools: { Version: '../../client/tools/', Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|CryptoPP': { Version: 1.2.5, Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|Detours': { Version: 4.0.13, Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|Opal': { Version: 0.12.0, Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|Soup.Test.Assert': { Version: 0.4.2, Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|json11': { Version: 1.1.4, Build: 'Build2', Tool: 'Tool1' }
			'mwasplund|reflex': { Version: 5.5.0, Build: 'Build2', Tool: 'Tool1' }
		}
	}
	Build0: {
		Wren: {
			'Soup|CSharp': { Version: 'C:/Users/mwasp/dev/repos/soup-csharp/Source/Extension/' }
		}
	}
	Build1: {
		Wren: {
			'Soup|CSharp': { Version: 'C:/Users/mwasp/dev/repos/soup-csharp/Source/Extension/' }
			'Soup|CSharp.Nuget': { Version: 0.5.3 }
		}
	}
	Build2: {
		Wren: {
			'Soup|Cpp': { Version: 0.15.3 }
		}
	}
	Build3: {
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
	Tool1: {
		'C++': {
			'mwasplund|copy': { Version: 1.1.0 }
			'mwasplund|mkdir': { Version: 1.1.0 }
			'mwasplund|parse.modules': { Version: 1.1.0 }
		}
	}
}