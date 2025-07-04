Name: 'Soup.PackageManager'
Language: 'C#|0'
Version: 1.0.0
Type: 'Executable'
Dependencies: {
	Build: [
		'Soup|CSharp.Nuget@0'
	]
	Runtime: [
		'../package-manager.core/'
		'../utilities/'
		'../opal/'
	]
}

Nuget: {
	Dependencies: {
		'Runtime': [
			{ Name: 'Microsoft.Identity.Client', Version: '4.48.1' }
			{ Name: 'System.Security.Cryptography.ProtectedData', Version: '6.0.0' }
		]
	}
}