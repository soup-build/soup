// <copyright file="build-load-engine-tests.cpp" company="Soup">
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

export module Soup.Core.Tests:BuildLoadEngineTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Soup::Core::UnitTests
{
	export class BuildLoadEngineTests
	{
	public:
		// [[Fact]]
		// Verifies we can initialize a Build Load Engine
		void Initialize()
		{
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable();
			auto hostBuildGlobalParameters = ValueTable();
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);
		}

		// [[Fact]]
		// Verifies that load throws an error when the package lock is missing
		void Load_MissingPackageLock_Throws()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1.1)
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable();
			auto hostBuildGlobalParameters = ValueTable();
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto exception = Assert::Throws<HandledException>([&]()
			{
				auto packageProvider = uut.Load(workingDirectory, std::nullopt);
			});

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: PackageLock file does not exist",
					"ERRO: Missing or invalid package lock C:/WorkingDirectory/my-package/package-lock.sml",
					"HIGH: Run `restore` and try again",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		// Verifies that a built in language extension is able to load a built in tool dependency
		void Load_LanguageExtension_Prebuilt_ToolDependency_Prebuilt()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1.1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
					Dependencies: {
						Tool: [
							'[C++]user1|test-tool@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/4.4.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-tool'
					Language: (C++@1.1)
					Version: 4.4.4
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {
							'C++': {
								'user1|test-tool': {
									Version: 4.4.4
									Digest: 'fake:user1-test-tool'
									Artifacts: {
										FakePlatform: 'fake:user1-test-tool-artifact'
									}
								}
							}
						}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/test-tool/4.4.4/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/test-tool/4.4.4/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 3),
										}
									},
									{
										"Tool",
										{
											PackageChildInfo(PackageReference("C++", "user1", "test-tool", SemanticVersion(4, 4, 4)), true, -1, 2),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-tool"),
								Digest("fake", "user1-test-tool-artifact"),
								Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/4.4.4/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/4.4.4/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/"},
							}
						},
						{
							2,
							{
								{ 3, "C:/Users/Me/.soup/artifacts/C++/user1/test-tool/4.4.4/user1-test-tool-artifact/"},
							}
						},
						{
							3,
							{
								{ 2, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/"},
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that an external language extension reference resolves correctly
		void Load_LanguageExtension_External()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1.1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto arguments = RecipeBuildArguments();
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
						2,
							{
								{ 2, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that an external language extension is able to load an external tool dependency
		void Load_LanguageExtension_External_ToolDependency_External()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@4.5)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@2.2)
					Version: 4.5.6
					Dependencies: {
						Tool: [
							'[C++]user1|test-tool@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-tool'
					Language: (C++@1.1)
					Version: 3.3.3
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@2.2)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@2.2)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': { Version: 4.5.6 }
							}
						}
					}
					Tools: {
						0: {
							'C++': {
								'user1|test-tool': { Version: 3.3.3 }
							}
						}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/cpp/4.5.6/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|cpp': { Version: '../cpp/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'user1|test-tool': { Version: '../test-tool/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/cpp/4.5.6/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/cpp/4.5.6/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/test-tool/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							5,
							PackageGraph(
								5,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(4, 5, 6)), true, -1, 5),
										}
									},
									{
										"Tool",
										{
											PackageChildInfo(PackageReference("C++", "user1", "test-tool", SemanticVersion(3, 3, 3)), true, -1, 3),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "cpp"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 4),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-tool"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 5, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							5,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/cpp/4.5.6/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that an external build dependency works
		void Load_BuildDependency_External()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
				{
					{
						"C++",
						KnownLanguage("user1", "cpp")
					},
					{
						"Wren",
						KnownLanguage("user1", "wren")
					},
				});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
				{
					{ "ArgumentValue", Value(true) },
				});
			auto hostBuildGlobalParameters = ValueTable(
				{
					{ "HostValue", Value(true) },
				});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 3)), true, -1, 3),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 4),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 3, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that an external build dependency with an external tool dependency loads correctly
		void Load_BuildDependency_External_ToolDependency_External()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
					Dependencies: {
						Tool: [
							'[C++]user1|test-tool@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-tool'
					Language: (C++@1.1)
					Version: 3.3.3
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3, Tool: '0' }
							}
						}
					}
					Tools: {
						0: {
							'C++': {
								'user1|test-tool': { Version: 3.3.3 }
							}
						}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'user1|test-tool': { Version: '../test-tool/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
				{
					{
						"C++",
						KnownLanguage("user1", "cpp")
					},
					{
						"Wren",
						KnownLanguage("user1", "wren")
					},
				});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
				{
					{ "ArgumentValue", Value(true) },
				});
			auto hostBuildGlobalParameters = ValueTable(
				{
					{ "HostValue", Value(true) },
				});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/test-tool/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							5,
							PackageGraph(
								5,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 3)), true, -1, 5),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
									{
										"Tool",
										{
											PackageChildInfo(PackageReference("C++", "user1", "test-tool", SemanticVersion(3, 3, 3)), true, -1, 3),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 4),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-tool"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
						{
							4,
							{
								{ 5, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							5,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that a local build dependency works
		void Load_BuildDependency_Local()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1.1)
					Dependencies: {
						Build: [
							'../test-build/'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/test-build/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/test-build/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/WorkingDirectory/test-build/recipe.sml",
					"DIAG: Load PackageLock: C:/WorkingDirectory/test-build/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/test-build/recipe.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/test-build/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference(Path("../test-build/")), true, -1, 3),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 4),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName(std::nullopt, "test-build"),
								std::nullopt,
								Path("C:/WorkingDirectory/test-build/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/test-build/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 3, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 2, "C:/WorkingDirectory/test-build/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies build dependency with an implicit owner fails from a local package
		void Load_BuildDependency_External_ImplicitOwner_Fails()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable();
			auto hostBuildGlobalParameters = ValueTable();
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");

			auto exception = Assert::Throws<HandledException>([&]()
			{
				auto packageProvider = uut.Load(workingDirectory, std::nullopt);
			});

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"ERRO: Implicit owner not allowed on local packages: [C++]my-package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		// Verifies an external build dependency is able to load an external runtime dependency with an implicit owner
		void Load_BuildDependency_External_RuntimeDependency_ImplicitOwner()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
					Dependencies: {
						Runtime: [
							'test-build2@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build2'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3, Tool: '0' }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
							'user1|test-build2': { Version: 3.3.3, Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build2/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 3)), true, -1, 3),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 4),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
									{
										"Runtime",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build2", SemanticVersion(3, 3, 3)), false, 3, -1),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-build2"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
								{ 3, "C:/Users/Me/.soup/packages/Wren/user1/test-build2/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 5, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies an external build dependency is able to load an external build dependency with an implicit owner
		void Load_BuildDependency_External_BuildDependency_ImplicitOwner()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
					Dependencies: {
						Build: [
							'test-build2@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build2'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3, Tool: '0' }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
								'user1|test-build2': { Version: 4.4.4 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build2/4.4.4/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build2': { Version: '../test-build2/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build2/4.4.4/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build2/4.4.4/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build2/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							5,
							PackageGraph(
								5,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 3)), true, -1, 4),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 5),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build2", SemanticVersion(4, 4, 4)), true, -1, 3),
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-build2"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/Wren/user1/test-build2/4.4.4/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							5,
							{
								{ 5, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies an external build dependency is able to load an external tool dependency with an implicit owner
		void Load_BuildDependency_External_ToolDependency_ImplicitOwner()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@2.2)
					Dependencies: {
						Tool: [
							'[C++]test-tool@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-tool'
					Language: (C++@1.1)
					Version: 3.3.3
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
								'user1|test-build': { Version: 3.3.3, Tool: '0' }
							}
						}
					}
					Tools: {
						0: {
							'C++': {
								'user1|test-tool': { Version: 3.3.3 }
							}
						}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'user1|test-tool': { Version: '../test-tool/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/C++/user1/test-tool/3.3.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/test-tool/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							5,
							PackageGraph(
								5,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 3)), true, -1, 5),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
									{
										"Tool",
										{
											PackageChildInfo(PackageReference("C++", "user1", "test-tool", SemanticVersion(3, 3, 3)), true, -1, 3),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 4),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "test-tool"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/C++/user1/test-tool/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/" },
							}
						},
						{
							4,
							{
								{ 5, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							5,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies a package lock can override a package version
		void Load_BuildDependency_External_Override()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Build: [
							'user1|test-build@3.3.3'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'test-build'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@2.2)
					Version: 1.1.2
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': { Version: 1.1.2 }
								'user1|test-build': { Version: 3.3.4 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.4/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|test-build': { Version: '../test-build/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': { Version: 2.2.3 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/wren/2.2.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|wren': { Version: '../wren/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/cpp/1.1.2/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|cpp': { Version: '../cpp/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{
					"ArgumentValue",
					Value(true),
				},
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{
					"HostValue",
					Value(true),
				},
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.4/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/wren/2.2.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/cpp/1.1.2/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/test-build/3.3.4/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/wren/2.2.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/cpp/1.1.2/package-lock.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/test-build/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							4,
							PackageGraph(
								4,
								2,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							5,
							PackageGraph(
								5,
								5,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "test-build", SemanticVersion(3, 3, 4)), true, -1, 4),
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 2)), true, -1, 5),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "test-build"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 3)), true, -1, 3),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "wren"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
						{
							5,
							PackageInfo(
								5,
								PackageName("user1", "cpp"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/"},
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
						{
							4,
							{
								{ 2, "C:/Users/Me/.soup/packages/Wren/user1/test-build/3.3.4/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
						{
							5,
							{
								{ 5, "C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.2/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies multiple packages can use the language packages with a package lock
		void Load_RuntimeDependency_Local_ExplicitLanguageOverride_MultipleReferences_ReuseBuildGraph()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C#@1)
					Dependencies: {
						Runtime: [
							'../package1/'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/package1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'package1'
					Language: (C#@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'csharp'
					Language: (Wren@2.2)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 2.2.2
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C#': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
							package1: { Version: '../package1/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|csharp': { Version: 2.2.3 }
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/csharp/2.2.3/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						Wren: {
							'user1|csharp': { Version: '../csharp/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 2.2.2
									Digest: 'fake:user1-wren'
									Artifacts: {
										FakePlatform: 'fake:user1-wren-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{
					"ArgumentValue",
					Value(true),
				},
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{
					"HostValue",
					Value(true),
				},
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/WorkingDirectory/package1/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/csharp/2.2.3/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/package1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/csharp/2.2.3/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/csharp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "csharp", SemanticVersion(2, 2, 3)), true, -1, 3),
										}
									},
									{
										"Runtime",
										{
											PackageChildInfo(PackageReference(Path("../package1/")), false, 2, -1),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName(std::nullopt, "package1"),
								std::nullopt,
								Path("C:/WorkingDirectory/package1/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/package1/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "csharp", SemanticVersion(2, 2, 3)), true, -1, 3),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "csharp"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "wren", SemanticVersion(2, 2, 2)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "wren"),
								Digest("fake", "user1-wren-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/wren/2.2.2/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/"},
								{ 2, "C:/WorkingDirectory/package1/out/zxAcy-Et010fdZUKLgFemwwWuC8/"},
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/wren/2.2.2/user1-wren-artifact/" },
							}
						},
						{
							3,
							{
								{ 3, "C:/Users/Me/.soup/packages/Wren/user1/csharp/2.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/"},
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies that external runtime dependencies that create a triangle reference resolve correctly to only build each package once
		void Load_RuntimeDependency_External_Triangle_BuildOnce()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1.1)
					Dependencies: {
						Runtime: [
							'user1|package-a@3.3.3'
							'user1|package-b@4.4.4'
						]
					}
				)")));
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'package-a'
					Language: (C++@1.1)
					Dependencies: {
						Runtime: [
							'user1|package-b@4.4.4'
						]
					}
				)")));
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'package-b'
					Language: (C++@1.1)
				)")));
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
							'user1|package-a': { Version: 3.3.3, Build: '0', Tool: '0' }
							'user1|package-b': { Version: 4.4.4, Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						0: {}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{ "ArgumentValue", Value(true) },
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{ "HostValue", Value(true) },
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/",
					"DIAG: Recipe already loaded: [C++]user1|package-b",
					"DIAG: Graph already loaded: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-a/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-b/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-b/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
									{
										"Runtime",
										{
											PackageChildInfo(PackageReference("C++", "user1", "package-a", SemanticVersion(3, 3, 3)), false, 2, -1),
											PackageChildInfo(PackageReference("C++", "user1", "package-b", SemanticVersion(4, 4, 4)), false, 3, -1),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "package-a"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
									{
										"Runtime",
										{
											PackageChildInfo(PackageReference("C++", "user1", "package-b", SemanticVersion(4, 4, 4)), false, 3, -1),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "package-b"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 2),
										}
									},
								}))
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
								{ 2, "C:/Users/Me/.soup/packages/C++/user1/package-a/3.3.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
								{ 3, "C:/Users/Me/.soup/packages/C++/user1/package-b/4.4.4/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies ability to load a runtime dependency with a name other than "Runtime" with an explicit language
		void Load_OtherDependency_ExplicitLanguage()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (C++@1)
					Dependencies: {
						Other: [
							'[C#]user1|package1@4.4.4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'package1'
					Language: (C#@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'cpp'
					Language: (Wren@1)
					Version: 1.1.1
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/3.3.3/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'csharp'
					Language: (Wren@1)
					Version: 3.3.3
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'C++': {
							'my-package': { Version: '../my-package/', Build: '0', Tool: '0' }
						}
						'C#': {
							'user1|package1': { Version: 4.4.4, Build: 'Build1', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|cpp': {
									Version: 1.1.1
									Digest: 'fake:user1-cpp'
									Artifacts: {
										FakePlatform: 'fake:user1-cpp-artifact'
									}
								}
							}
						}
						Build1: {
							Wren: {
								'user1|csharp': {
									Version: 3.3.3
									Digest: 'fake:user1-soup-csharp'
									Artifacts: {
										FakePlatform: 'fake:user1-soup-csharp-artifact'
									}
								}
							}
						}
					}
					Tools: {
						Tool1: {
						}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{
					"ArgumentValue",
					Value(true),
				},
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{
					"HostValue",
					Value(true),
				},
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto packageProvider = uut.Load(workingDirectory, std::nullopt);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/csharp/3.3.3/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"INFO: Skip loading sub graph for prebuilt package",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/csharp/3.3.3/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C#/user1/package1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C#/user1/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C#/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected package graph
			Assert::AreEqual(
				PackageProvider(
					1,
					PackageGraphLookupMap(
					{
						{
							1,
							PackageGraph(
								1,
								1,
								ValueTable(
								{
									{ "ArgumentValue", Value(true) },
								}))
						},
						{
							2,
							PackageGraph(
								2,
								3,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
						{
							3,
							PackageGraph(
								3,
								4,
								ValueTable(
								{
									{ "HostValue", Value(true) },
								}))
						},
					}),
					PackageLookupMap(
					{
						{
							1,
							PackageInfo(
								1,
								PackageName(std::nullopt, "my-package"),
								std::nullopt,
								Path("C:/WorkingDirectory/my-package/"),
								&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "cpp", SemanticVersion(1, 1, 1)), true, -1, 3),
										}
									},
									{
										"Other",
										{
											PackageChildInfo(PackageReference("C#", "user1", "package1", SemanticVersion(4, 4, 4)), false, 2, -1),
										}
									},
								}))
						},
						{
							2,
							PackageInfo(
								2,
								PackageName("user1", "package1"),
								std::nullopt,
								Path("C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/recipe.sml")),
								PackageChildrenMap({
									{
										"Build",
										{
											PackageChildInfo(PackageReference("Wren", "user1", "csharp", SemanticVersion(3, 3, 3)), true, -1, 2),
										}
									},
								}))
						},
						{
							3,
							PackageInfo(
								3,
								PackageName("user1", "csharp"),
								Digest("fake", "user1-soup-csharp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/3.3.3/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/csharp/3.3.3/recipe.sml")),
								PackageChildrenMap())
						},
						{
							4,
							PackageInfo(
								4,
								PackageName("user1", "cpp"),
								Digest("fake", "user1-cpp-artifact"),
								Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/"),
								&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/Wren/user1/cpp/1.1.1/recipe.sml")),
								PackageChildrenMap())
						},
					}),
					PackageTargetDirectories(
					{
						{
							1,
							{
								{ 1, "C:/WorkingDirectory/my-package/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
								{ 2, "C:/Users/Me/.soup/packages/C#/user1/package1/4.4.4/out/zxAcy-Et010fdZUKLgFemwwWuC8/" },
							}
						},
						{
							2,
							{
								{ 3, "C:/Users/Me/.soup/artifacts/Wren/user1/csharp/3.3.3/user1-soup-csharp-artifact/" },
							}
						},
						{
							3,
							{
								{ 4, "C:/Users/Me/.soup/artifacts/Wren/user1/cpp/1.1.1/user1-cpp-artifact/" },
							}
						},
					})),
				packageProvider,
				"Verify package graph matches expected.");
		}

		// [[Fact]]
		// Verifies ability to detect a runtime dependency that uses the parent build package creating a circular dependency
		// that requires a pre-built package to break the circle
		void Load_CircularBuildDependency_Throws()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'my-package'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/wren/1.1.1/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'wren'
					Language: (Wren@1)
					Version: 1.1.1
					Dependencies: {
						Runtime: [
							'package1@4'
						]
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/user1/package1/4.4.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'package1'
					Language: (Wren@1)
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'Wren': {
							'my-package': { Version: './', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 1.1.1
									Digest: 'fake:user1-wren'
								}
							}
						}
					}
					Tools: {
						Tool1: {
						}
					}
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/user1/wren/1.1.1/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						'Wren': {
							'user1|wren': { Version: './', Build: '0', Tool: '0' }
							'user1|package1': { Version: '../../../../../packages/Wren/user1/package1/4.4.4/', Build: '0', Tool: '0' }
						}
					}
					Builds: {
						0: {
							Wren: {
								'user1|wren': {
									Version: 1.1.1
									Digest: 'fake:user1-wren'
								}
							}
						}
					}
					Tools: {
						Tool1: {
						}
					}
				)")));

			auto knownLanguages = std::map<std::string, KnownLanguage>(
			{
				{
					"C++",
					KnownLanguage("user1", "cpp")
				},
				{
					"Wren",
					KnownLanguage("user1", "wren")
				},
				{
					"C#",
					KnownLanguage("user1", "csharp")
				},
			});
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto targetBuildGlobalParameters = ValueTable(
			{
				{
					"ArgumentValue",
					Value(true),
				},
			});
			auto hostBuildGlobalParameters = ValueTable(
			{
				{
					"HostValue",
					Value(true),
				},
			});
			auto hostPlatform = "FakePlatform";
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto recipeCache = RecipeCache();
			auto uut = BuildLoadEngine(
				knownLanguages,
				locationManager,
				targetBuildGlobalParameters,
				hostBuildGlobalParameters,
				hostPlatform,
				userDataPath,
				recipeCache);

			auto workingDirectory = Path("C:/WorkingDirectory/my-package/");

			auto exception = Assert::Throws<std::runtime_error>([&]()
			{
				auto packageProvider = uut.Load(workingDirectory, std::nullopt);
			});

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/my-package/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/my-package/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/wren/1.1.1/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/user1/wren/1.1.1/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/user1/package1/4.4.4/recipe.sml",
					"ERRO: Found circular build dependency: [Wren]user1|wren -> user1|wren@1",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/wren/1.1.1/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/user1/wren/1.1.1/package-lock.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/user1/package1/4.4.4/recipe.sml",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}
	};
}