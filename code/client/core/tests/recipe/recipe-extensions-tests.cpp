// <copyright file="recipe-extensions-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:RecipeExtensionsTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class RecipeExtensionsTests
	{
	public:
		// [[Fact]]
		void TryLoadRecipeFromFile_MissingFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			auto directory = Path("./TestFiles/NoFile/recipe.sml");
			Recipe actual;
			auto result = RecipeExtensions::TryLoadRecipeFromFile(directory, actual);

			Assert::IsFalse(result, "Verify result is false.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: ./TestFiles/NoFile/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load Recipe: ./TestFiles/NoFile/recipe.sml",
					"INFO: Recipe file does not exist.",
				}), 
				testListener->GetMessages(),
				"Verify messages match expected.");
		}
		
		// [[Fact]]
		void TryLoadRecipeFromFile_GarbageFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			fileSystem->CreateMockFile(
				Path("./TestFiles/GarbageRecipe/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream("garbage")));

			auto directory = Path("./TestFiles/GarbageRecipe/recipe.sml");
			Recipe actual;
			auto result = RecipeExtensions::TryLoadRecipeFromFile(directory, actual);

			Assert::IsFalse(result, "Verify result is false.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: ./TestFiles/GarbageRecipe/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load Recipe: ./TestFiles/GarbageRecipe/recipe.sml",
					"ERRO: Deserialize Threw: Parsing the Recipe SML failed: Failed to parse at 1:7  ./TestFiles/GarbageRecipe/recipe.sml",
					"INFO: Failed to parse Recipe.",
				}), 
				testListener->GetMessages(),
				"Verify messages match expected.");
		}

		// [[Fact]]
		void TryLoadRecipeFromFile_SimpleFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			fileSystem->CreateMockFile(
				Path("./TestFiles/SimpleRecipe/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'MyPackage'
					Language: (C++@1)
				)")));

			auto directory = Path("./TestFiles/SimpleRecipe/recipe.sml");
			Recipe actual;
			auto result = RecipeExtensions::TryLoadRecipeFromFile(directory, actual);

			Assert::IsTrue(result, "Verify result is false.");

			auto expected = Recipe(RecipeTable(
			{
				{ "Name", "MyPackage" },
				{ "Language", LanguageReference("C++", SemanticVersion(1)) },
			}));

			Assert::AreEqual(expected, actual, "Verify matches expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: ./TestFiles/SimpleRecipe/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load Recipe: ./TestFiles/SimpleRecipe/recipe.sml",
				}), 
				testListener->GetMessages(),
				"Verify messages match expected.");
		}

		// [[Fact]]
		void SaveToFile_SimpleFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			auto directory = Path("./TestFiles/SimpleRecipe/recipe.sml");
			auto recipe = Recipe(RecipeTable(
			{
				{ "Name", "MyPackage" },
				{ "Language", LanguageReference("C++", SemanticVersion(1)) },
			}));
			RecipeExtensions::SaveToFile(directory, recipe);

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"OpenWrite: ./TestFiles/SimpleRecipe/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
				}), 
				testListener->GetMessages(),
				"Verify messages match expected.");

			// Verify the contents of the build file
			std::string expectedBuildFile = 
R"(Name: 'MyPackage'
Language: (C++@1)
)";
			auto mockBuildFile = fileSystem->GetMockFile(Path("./TestFiles/SimpleRecipe/recipe.sml"));
			Assert::AreEqual(expectedBuildFile, mockBuildFile->Content.str(), "Verify file contents.");
		}
	};
}
