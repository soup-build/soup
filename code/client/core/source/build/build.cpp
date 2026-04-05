// <copyright file="build.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <vector>

export module Soup.Core:Build;

import Opal;
import :BuildConstants;
import :BuildEvaluateEngine;
import :BuildLoadEngine;
import :BuildRunner;
import :FileSystemState;
import :KnownLanguage;
import :OperationGraph;
import :PackageProvider;
import :RecipeBuildArguments;
import :RecipeBuildLocationManager;
import :RecipeCache;
import :Value;

using namespace Opal;

namespace Soup::Core::Build
{
	/// <summary>
	/// The set of known languages
	/// </summary>
	export std::map<std::string, KnownLanguage> GetKnownLanguages()
	{
		auto result = std::map<std::string, KnownLanguage>(
		{
			{
				"C",
				KnownLanguage("soup", "c")
			},
			{
				"C++",
				KnownLanguage("soup", "cpp")
			},
			{
				"C#",
				KnownLanguage("soup", "csharp")
			},
			{
				"Wren",
				KnownLanguage("soup", "wren")
			},
		});

		return result;
	}

	/// <summary>
	/// Load the build graph
	/// </summary>
	export PackageProvider LoadBuildGraph(
		const Path& workingDirectory,
		const std::optional<std::string>& owner,
		const ValueTable& targetGlobalParameters,
		const Path& userDataPath,
		std::string_view hostPlatform,
		RecipeCache& recipeCache)
	{
		// auto startTime = std::chrono::high_resolution_clock::now();

		// Load the system specific state
		auto hostGlobalParameters = ValueTable();
		hostGlobalParameters.emplace("System", Value(std::string(hostPlatform)));

		// auto endTime = std::chrono::high_resolution_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

		// Log::Info("LoadSystemState: {} seconds", duration.count());

		// startTime = std::chrono::high_resolution_clock::now();

		// Generate the package build graph
		auto knownLanguages = GetKnownLanguages();
		auto locationManager = RecipeBuildLocationManager(knownLanguages);
		auto loadEngine = BuildLoadEngine(
			knownLanguages,
			locationManager,
			targetGlobalParameters,
			hostGlobalParameters,
			hostPlatform,
			userDataPath,
			recipeCache);
		auto packageProvider = loadEngine.Load(workingDirectory, std::move(owner));

		// endTime = std::chrono::high_resolution_clock::now();
		// duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

		// Log::Info("BuildLoadEngine: {} seconds", duration.count());

		return packageProvider;
	}

	/// <summary>
	/// Preload the file system
	/// </summary>
	void PreloadFileSystemState(
		PackageProvider& packageProvider,
		FileSystemState& fileSystemState)
	{
		// auto startTime = std::chrono::high_resolution_clock::now();

		for (auto package : packageProvider.GetPackageLookup())
		{
			fileSystemState.PreloadDirectory(package.second.PackageRoot, true);
			// TODO: fileSystemState.PreloadDirectory(package.second.TargetDirectory, false);
		}

		// auto endTime = std::chrono::high_resolution_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

		// Log::Info("PreloadFileSystemState: {} seconds", duration.count());
	}

	export void Execute(
		PackageProvider& packageProvider,
		const RecipeBuildArguments& arguments,
		const Path& userDataPath,
		const std::vector<Path>& systemReadAccess,
		RecipeCache& recipeCache)
	{
		// auto startTime = std::chrono::high_resolution_clock::now();

		// Load the file system state
		auto fileSystemState = FileSystemState();
		PreloadFileSystemState(packageProvider, fileSystemState);

		// Initialize a shared Evaluate Engine
		auto evaluateEngine = BuildEvaluateEngine(
			arguments.Parallelization,
			arguments.ForceRebuild,
			arguments.DisableMonitor,
			arguments.PartialMonitor,
			fileSystemState);

		// Initialize the build runner that will perform the generate and evaluate phase
		// for each individual package
		auto buildRunner = BuildRunner(
			arguments,
			userDataPath,
			systemReadAccess,
			recipeCache,
			packageProvider,
			evaluateEngine,
			fileSystemState);
		buildRunner.Execute();

		// auto endTime = std::chrono::high_resolution_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

		// Log::Info("BuildRunner: {} seconds", duration.count());
	}
}
