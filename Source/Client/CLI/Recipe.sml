Name: "Soup"
Version: "0.19.4"
Language: "C++|0.1"

Type: "Executable"

Source: [
	"Source/Main.cpp"
]
IncludePaths: [
	"Source/Commands/"
	"Source/Options/"
]

Dependencies: {
	// Ensure the core build extensions are runtime dependencies
	Runtime: [
		"../Core/"
		"../../Tools/Copy/"
		"../../Tools/Mkdir/"
		"../../Monitor/Host/"
	]
	Other: [
		// "../../GenerateSharp/Generate/"
		"C#|Soup.Cpp@0.2.2"
		"C#|Soup.CSharp@0.5.2"
	]
	Build: [
		// TODO: "Soup.Test.Cpp@0.2.1"
	]
	Test: [
		"Soup.Test.Assert@0.2.1"
	]
}

Tests: {
	Source: [
		"UnitTests/gen/Main.cpp"
	]
	IncludePaths: [
		"UnitTests/"
	]
}
