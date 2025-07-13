// <copyright file="recipe-build-location-manager-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core:RecipeBuildLocationManagerTests;

import Monitor.Host;
import Opal;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Soup::Core::UnitTests
{
	export class RecipeBuildLocationManagerTests
	{
	public:
		// [[Fact]]
		void GetOutputDirectory_Simple()
		{
			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			auto packageName = Core::PackageName(std::nullopt, "MyPackage");
			auto workingDirectory = Path("C:/WorkingDirectory/");
			auto recipe = Recipe(RecipeTable(
			{
				{ "Name", "MyPackage" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
			}));
			auto globalParameters = ValueTable();
			auto recipeCache = RecipeCache();
			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("User1", "Cpp")
				}
			});
			auto uut = RecipeBuildLocationManager(knownLanguages);
			auto targetDirectory = uut.GetOutputDirectory(
				packageName,
				workingDirectory,
				recipe,
				globalParameters,
				recipeCache);

			Assert::AreEqual(Path("C:/WorkingDirectory/out/J_HqSstV55vlb-x6RWC_hLRFRDU/"), targetDirectory, "Verify target directory matches expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}
		
		// [[Fact]]
		void GetOutputDirectory_RootRecipe()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create a root recipe
			fileSystem->CreateMockFile(
				Path("C:/root-recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					OutputRoot: './BuildOut/'
				)")));

			auto packageName = Core::PackageName(std::nullopt, "MyPackage");
			auto workingDirectory = Path("C:/WorkingDirectory/");
			auto recipe = Recipe(RecipeTable(
			{
				{ "Name", "MyPackage" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
			}));
			auto globalParameters = ValueTable();
			auto recipeCache = RecipeCache();
			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("User1", "Cpp")
				}
			});
			auto uut = RecipeBuildLocationManager(knownLanguages);
			auto targetDirectory = uut.GetOutputDirectory(
				packageName,
				workingDirectory,
				recipe,
				globalParameters,
				recipeCache);

			Assert::AreEqual(
				Path("C:/BuildOut/C++/Local/MyPackage/1.2.3/J_HqSstV55vlb-x6RWC_hLRFRDU/"),
				targetDirectory,
				"Verify target directory matches expected.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: Found Root Recipe: 'C:/root-recipe.sml'",
					"DIAG: Load Root Recipe: C:/root-recipe.sml",
					"INFO: Override root output: C:/BuildOut/C++/Local/MyPackage/1.2.3/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"OpenReadBinary: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}
	};
}
