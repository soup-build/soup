Version: 4
Closures: {
	Root: {
		"C++": [
			{ Name: "CryptoPP", Version: "1.2.1", Build: "Build0" }
			{ Name: "Detours", Version: "4.0.9", Build: "Build0" }
			{ Name: "Monitor.Host", Version: "../Monitor/Host/", Build: "Build2" }
			{ Name: "Monitor.Shared", Version: "../Monitor/Shared/", Build: "Build2" }
			{ Name: "Opal", Version: "0.9.2", Build: "Build0" }
			{ Name: "reflex", Version: "1.0.2", Build: "Build0" }
			{ Name: "Soup.Core", Version: "../Client/Core/", Build: "Build1" }
			{ Name: "Soup.Generate.Test", Version: "./", Build: "Build0" }
			{ Name: "Soup.Test.Assert", Version: "0.3.1", Build: "Build0" }
			{ Name: "wren", Version: "1.0.0", Build: "Build0" }
		]
	}
	Build0: {
		"Wren": [
			{ Name: "Soup.Cpp", Version: "0.6.2" }
		]
	}
	Build1: {
		"Wren": [
			{ Name: "Soup.Cpp", Version: "0.6.2" }
			{ Name: "Soup.Test.Cpp", Version: "0.5.2" }
		]
	}
	Build2: {
		"Wren": [
			{ Name: "Soup.Cpp", Version: "0.6.2" }
		]
	}
}