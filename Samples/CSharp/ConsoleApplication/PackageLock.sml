Version: 5
Closures: {
	Root: {
		"C#": {
			"Samples.CSharp.ConsoleApplication": { Version: "../ConsoleApplication", Build: "Build0", Tool: "Tool0" }
		}
	}
	Build0: {
		Wren: {
			"mwasplund|Soup.CSharp": { Version: "0.12.0" }
		}
	}
	Tool0: {
		"C++": {
			"mwasplund|copy": { Version: "1.0.0" }
			"mwasplund|mkdir": { Version: "1.0.0" }
		}
	}
}