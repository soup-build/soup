// <copyright file="generate-build-task.wren" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

import "soup" for Soup, SoupTask
import "soup|build-utils:./path" for Path
import "soup|build-utils:./list-extensions" for ListExtensions
import "soup|build-utils:./map-extensions" for MapExtensions
import "soup|build-utils:./shared-operations" for SharedOperations

class GenerateBuildTask is SoupTask {
	/// <summary>
	/// Get the run before list
	/// </summary>
	static runBefore { [
		"BuildTask"
	] }

	/// <summary>
	/// Get the run after list
	/// </summary>
	static runAfter { [] }

	/// <summary>
	/// Core Evaluate
	/// </summary>
	static evaluate() {
		Soup.info("Running Before Build!")

		// Get the build table
		var buildTable = MapExtensions.EnsureTable(Soup.activeState, "Build")

		var contextTable = Soup.globalState["Context"]
		var targetRoot = Path.new(contextTable["TargetDirectory"])

		var generateDirectory = Path.new("./gen/")
		var generateFile = generateDirectory + Path.new("helper.cpp")

		// Ensure the generate folder exists
		var createGenerateDirectory = SharedOperations.CreateCreateDirectoryOperation(
			targetRoot,
			generateDirectory)
		Soup.createOperation(
			createGenerateDirectory.Title,
			createGenerateDirectory.Executable.toString,
			createGenerateDirectory.Arguments,
			createGenerateDirectory.WorkingDirectory.toString,
			ListExtensions.ConvertFromPathList(createGenerateDirectory.DeclaredInput),
			ListExtensions.ConvertFromPathList(createGenerateDirectory.DeclaredOutput))

		// Create the generate operation
		GenerateBuildTask.CreateGenerateFileOperation(targetRoot, generateFile)

		var generatedSourceInfo = {}
		generatedSourceInfo["File"] = generateFile.toString
		generatedSourceInfo["Root"] = targetRoot.toString
		generatedSourceInfo["IsInterface"] = true
		generatedSourceInfo["Module"] = "Sample.Generate"
		generatedSourceInfo["Imports"] = []

		var sourceFiles = [
			generatedSourceInfo
		]

		// Add the explicit source info for the generated file so we treat it like a normal
		// compiled translation unit
		ListExtensions.Append(
			MapExtensions.EnsureList(buildTable, "Source"),
			sourceFiles)
	}

	/// <summary>
	/// Create a build operation that will create a directory
	/// </summary>
	static CreateGenerateFileOperation(workingDirectory, generateFile) {
		// Discover the dependency tool
		var toolExecutable = SharedOperations.ResolveRuntimeDependencyRunExecutable("samples-cpp-generate-tool")

		var title = "Run Generate Tool"

		var program = Path.new(toolExecutable)
		var inputFiles = []
		var outputFiles = [generateFile]

		// Build the arguments
		var arguments = [generateFile.toString]

		Soup.createOperation(
			title,
			program.toString,
			arguments,
			workingDirectory.toString,
			ListExtensions.ConvertFromPathList(inputFiles),
			ListExtensions.ConvertFromPathList(outputFiles))
	}
}