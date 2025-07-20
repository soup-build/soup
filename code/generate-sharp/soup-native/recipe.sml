Name: 'Soup.Native'
Language: 'C#|0'
Version: 1.0.0
TargetFramework: 'net9.0'
AllowUnsafeBlocks: true
Dependencies: {
	Build: [
		'Soup|CSharp.Nuget@0'
	]
	Runtime: [
		'../opal/'
		'../utilities/'
	]
	RuntimeReference: [
		'../../client/native/'
	]
}