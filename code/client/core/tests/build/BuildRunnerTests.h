// <copyright file="BuildRunnerTests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "MockEvaluateEngine.h"

namespace Soup::Core::UnitTests
{
	class BuildRunnerTests
	{
	public:
		// [[Fact]]
		void Initialize_Success()
		{
			auto arguments = RecipeBuildArguments();
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>();
			auto recipeCache = RecipeCache();
			auto packageProvider = PackageProvider(1, PackageGraphLookupMap(), PackageLookupMap());
			auto evaluateEngine = MockEvaluateEngine();
			auto fileSystemState = FileSystemState();
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState,
				locationManager);
		}

		// [[Fact]]
		void Execute_NoDependencies()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			auto fileSystemState = FileSystemState(
				0,
				std::unordered_map<FileId, Path>({
				}),
				TestHelpers::BuildDirectoryLookup({
					Path("C:/WorkingDirectory/MyPackage/Recipe.sml"),
				}),
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
				}));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto operationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto operationGraphFiles = std::set<FileId>();
			auto operationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(operationGraph, operationGraphFiles, fileSystemState, operationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(operationGraphContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/MyPackage/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "MyPackage" },
						{ "Language", "C++|1" },
					}))
				},
			});
			auto packageProvider = PackageProvider(
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
				}),
				PackageLookupMap(
				{
					{
						1,
						PackageInfo(
							1,
							PackageName(std::nullopt, "MyPackage"),
							false,
							Path("C:/WorkingDirectory/MyPackage/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/MyPackage/Recipe.sml")),
							PackageChildrenMap())
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState,
				locationManager);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Evaluate Operation Graph",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 1>Previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Loading new Evaluate Operation Graph",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 1>Saving updated build state",
					"INFO: 1>Done",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/WorkingDirectory/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected process requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
				}),
				processManager->GetRequests(),
				"Verify process manager requests match expected.");

			// Verify expected evaluate requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable()
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/") },
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_MyPackage)/"),
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable()
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_MyPackage)/") },
									{ "TargetDirectory", std::string("/(TARGET_MyPackage)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/WorkingDirectory/MyPackage/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify generate results content match expected.");
		}

		// [[Fact]]
		void Execute_BuildDependency()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			auto fileSystemState = FileSystemState(
				0,
				{},
				TestHelpers::BuildDirectoryLookup({
					Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/Recipe.sml"),
					Path("C:/WorkingDirectory/MyPackage/Recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto myProjectOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto myProjectOperationGraphFiles = std::set<FileId>();
			auto myProjectOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(myProjectOperationGraph, myProjectOperationGraphFiles, fileSystemState, myProjectOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(myProjectOperationGraphContent)));

			auto testBuildOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto testBuildOperationGraphFiles = std::set<FileId>();
			auto testBuildOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(testBuildOperationGraph, testBuildOperationGraphFiles, fileSystemState,  testBuildOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(testBuildOperationGraphContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/MyPackage/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "MyPackage" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Build", RecipeList({ "User1|TestBuild@1.2.3" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "TestBuild" },
						{ "Language", "C#|1" },
						{ "Version", "1.2.3" },
					}))
				},
			});
			auto packageProvider = PackageProvider(
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
							PackageName(std::nullopt, "MyPackage"),
							false,
							Path("C:/WorkingDirectory/MyPackage/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/MyPackage/Recipe.sml")),
							PackageChildrenMap({
								{
									"Build",
									{
										PackageChildInfo(
											PackageReference(std::nullopt, "User1", "TestBuild", SemanticVersion(1, 2, 3)),
											true,
											-1,
											2),
									}
								},
							}))
					},
					{
						2,
						PackageInfo(
							2,
							PackageName("User1", "TestBuild"),
							false,
							Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/Recipe.sml")),
							PackageChildrenMap())
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState,
				locationManager);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 2>Running Build: [C#]User1|TestBuild",
					"INFO: 2>Build 'User1|TestBuild'",
					"INFO: 2>Checking for existing Evaluate Operation Graph",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"INFO: 2>Previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Loading new Evaluate Operation Graph",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"INFO: 2>Saving updated build state",
					"INFO: 2>Done",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Evaluate Operation Graph",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 1>Previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Loading new Evaluate Operation Graph",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 1>Saving updated build state",
					"INFO: 1>Done",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C#/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/RootRecipe.sml",
					"Exists: C:/Users/Me/RootRecipe.sml",
					"Exists: C:/Users/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected process requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					"GetCurrentProcessFileName",
				}),
				processManager->GetRequests(),
				"Verify process manager requests match expected.");

			// Verify expected evaluate requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto testBuildGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable()
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/") },
							{ "/(TARGET_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_User1|TestBuild)/"),
							std::string("/(TARGET_User1|TestBuild)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_User1|TestBuild)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable()
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_User1|TestBuild)/") },
									{ "TargetDirectory", std::string("/(TARGET_User1|TestBuild)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "HostValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(testBuildGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto testBuildGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor"));
			auto testBuildGenerateResults = OperationResultsReader::Deserialize(testBuildGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				testBuildGenerateResults,
				"Verify test build generate results content match expected.");

			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"User1|TestBuild",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/") },
										})
									},
								})
							},
						})
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/") },
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_MyPackage)/"),
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/zDqRc65c9x3jySpevCCCyZ15fGs/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_User1|TestBuild)/", std::string("/(BUILD_TARGET_User1|TestBuild)/") },
						})
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_MyPackage)/") },
									{ "TargetDirectory", std::string("/(TARGET_MyPackage)/") },
								})
							},
							{
								"Dependencies",
								ValueTable(
								{
									{
										"Build",
										ValueTable(
										{
											{
												"User1|TestBuild",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("User1|TestBuild@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_User1|TestBuild)/") },
														})
													},
												})
											},
										})
									},
								})
							},
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/WorkingDirectory/MyPackage/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify my package generate results content match expected.");
		}

		// [[Fact]]
		void Execute_TriangleDependency_NoRebuild()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			auto fileSystemState = FileSystemState(
				0,
				{},
				TestHelpers::BuildDirectoryLookup({
					Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/Recipe.sml"),
					Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/Recipe.sml"),
					Path("C:/WorkingDirectory/MyPackage/Recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto myProjectOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto myProjectOperationGraphFiles = std::set<FileId>();
			auto myProjectOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(myProjectOperationGraph, myProjectOperationGraphFiles, fileSystemState, myProjectOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(myProjectOperationGraphContent)));
			auto packageAOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto packageAOperationGraphFiles = std::set<FileId>();
			auto packageAOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(packageAOperationGraph, packageAOperationGraphFiles, fileSystemState, packageAOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(packageAOperationGraphContent)));
			auto packageBOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto packageBOperationGraphFiles = std::set<FileId>();
			auto packageBOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(packageBOperationGraph, packageBOperationGraphFiles, fileSystemState, packageBOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(packageBOperationGraphContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/MyPackage/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "MyPackage" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Runtime", RecipeList({ "User1|PackageA@1.2.3", "User1|PackageB@1.1.1" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "PackageA" },
						{ "Language", "C++|1" },
						{ "Version", "1.2.3" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Runtime", RecipeList({ "User1|PackageB@1.1.1" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "PackageB" },
						{ "Language", "C++|1" },
						{ "Version", "1.1.1" },
					}))
				},
			});
			auto packageProvider = PackageProvider(
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
				}),
				PackageLookupMap(
				{
					{
						1,
						PackageInfo(
							1,
							PackageName(std::nullopt, "MyPackage"),
							false,
							Path("C:/WorkingDirectory/MyPackage/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/MyPackage/Recipe.sml")),
							PackageChildrenMap({
								{
									"Runtime",
									{
										PackageChildInfo(PackageReference(std::nullopt, "User1", "PackageA", SemanticVersion(1, 2, 3)), false, 2, -1),
										PackageChildInfo(PackageReference(std::nullopt, "User1", "PackageB", SemanticVersion(1, 1, 1)), false, 3, -1),
									}
								},
							}))
					},
					{
						2,
						PackageInfo(
							2,
							PackageName("User1", "PackageA"),
							false,
							Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/Recipe.sml")),
							PackageChildrenMap({
								{
									"Runtime",
									{
										PackageChildInfo(PackageReference(std::nullopt, "User1", "PackageB", SemanticVersion(1, 1, 1)), false, 3, -1),
									}
								},
							}))
					},
					{
						3,
						PackageInfo(
							3,
							PackageName("User1", "PackageB"),
							false,
							Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/Recipe.sml")),
							PackageChildrenMap())
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState,
				locationManager);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 3>Running Build: [C++]User1|PackageB",
					"INFO: 3>Build 'User1|PackageB'",
					"INFO: 3>Checking for existing Evaluate Operation Graph",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 3>Previous graph found",
					"INFO: 3>Checking for existing Evaluate Operation Results",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 3>Operation results file does not exist",
					"INFO: 3>No previous results found",
					"INFO: 3>Create Directory: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 3>Check outdated generate input file: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 3>Value Table file does not exist",
					"INFO: 3>Save Generate Input file",
					"INFO: 3>Checking for existing Generate Operation Results",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 3>Operation results file does not exist",
					"INFO: 3>No previous results found",
					"INFO: 3>Loading new Evaluate Operation Graph",
					"DIAG: 3>Map previous operation graph observed results",
					"INFO: 3>Create Directory: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 3>Saving updated build state",
					"INFO: 3>Done",
					"DIAG: 2>Running Build: [C++]User1|PackageA",
					"INFO: 2>Build 'User1|PackageA'",
					"INFO: 2>Checking for existing Evaluate Operation Graph",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 2>Previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Loading new Evaluate Operation Graph",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 2>Saving updated build state",
					"INFO: 2>Done",
					"DIAG: 3>Running Build: [C++]User1|PackageB",
					"DIAG: 3>Recipe already built: [C++]User1|PackageB",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Evaluate Operation Graph",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 1>Previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Loading new Evaluate Operation Graph",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 1>Saving updated build state",
					"INFO: 1>Done",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageB/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/RootRecipe.sml",
					"Exists: C:/Users/Me/RootRecipe.sml",
					"Exists: C:/Users/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageA/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C++/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/RootRecipe.sml",
					"Exists: C:/Users/Me/RootRecipe.sml",
					"Exists: C:/Users/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected process requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					"GetCurrentProcessFileName",
					"GetCurrentProcessFileName",
				}),
				processManager->GetRequests(),
				"Verify process manager requests match expected.");

			// Verify expected evaluate requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Evaluate: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto packageAGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Runtime",
								ValueTable(
								{
									{
										"User1|PackageB",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/") },
										})
									},
								})
							},
						})
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_User1|PackageA)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/") },
							{ "/(TARGET_User1|PackageA)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
							{ "/(TARGET_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_User1|PackageA)/"),
							std::string("/(TARGET_User1|PackageA)/"),
							std::string("/(TARGET_User1|PackageB)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_User1|PackageA)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(TARGET_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_User1|PackageA)/") },
									{ "TargetDirectory", std::string("/(TARGET_User1|PackageA)/") },
								})
							},
							{
								"Dependencies",
								ValueTable(
								{
									{
										"Runtime",
										ValueTable(
										{
											{
												"User1|PackageB",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("User1|PackageB@1.1.1") },
															{ "TargetDirectory", std::string("/(TARGET_User1|PackageB)/") },
														})
													},
												})
											},
										})
									},
								})
							},
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(packageAGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto packageAGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto packageAGenerateResults = OperationResultsReader::Deserialize(packageAGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				packageAGenerateResults,
				"Verify package A generate results content match expected.");

			auto packageBGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable()
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/") },
							{ "/(TARGET_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_User1|PackageB)/"),
							std::string("/(TARGET_User1|PackageB)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_User1|PackageB)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable()
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_User1|PackageB)/") },
									{ "TargetDirectory", std::string("/(TARGET_User1|PackageB)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(packageBGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto packageBGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto packageBGenerateResults = OperationResultsReader::Deserialize(packageBGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				packageBGenerateResults,
				"Verify package B generate results content match expected.");

			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Runtime",
								ValueTable(
								{
									{
										"User1|PackageA",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/") },
										})
									},
									{
										"User1|PackageB",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/") },
										})
									},
								})
							},
						})
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/") },
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
							{ "/(TARGET_User1|PackageA)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
							{ "/(TARGET_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_MyPackage)/"),
							std::string("/(TARGET_MyPackage)/"),
							std::string("/(TARGET_User1|PackageA)/"),
							std::string("/(TARGET_User1|PackageB)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(TARGET_User1|PackageA)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageA/1.2.3/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
							{ "/(TARGET_User1|PackageB)/", std::string("C:/Users/Me/.soup/packages/C++/User1/PackageB/1.1.1/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_MyPackage)/") },
									{ "TargetDirectory", std::string("/(TARGET_MyPackage)/") },
								})
							},
							{
								"Dependencies",
								ValueTable(
								{
									{
										"Runtime",
										ValueTable(
										{
											{
												"User1|PackageA",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("User1|PackageA@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_User1|PackageA)/") },
														})
													},
												})
											},
											{
												"User1|PackageB",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("User1|PackageB@1.1.1") },
															{ "TargetDirectory", std::string("/(TARGET_User1|PackageB)/") },
														})
													},
												})
											},
										})
									},
								})
							},
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/WorkingDirectory/MyPackage/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify my package generate results content match expected.");
		}

		// [[Fact]]
		void Execute_PackageLock_OverrideBuildDependency()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);
			auto fileSystemState = FileSystemState(
				0,
				{},
				TestHelpers::BuildDirectoryLookup({
					Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/Recipe.sml"),
					Path("C:/WorkingDirectory/MyPackage/Recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/PackageLock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							"C#": {
								"User1|TestBuild": { Version: "1.3.0", Build: "Build1" }
							}
							"C++": {
								MyPackage: { Version: "../MyPackage/", Build: "Build0" }
							}
						}
						Build0: {
							"C#": {
								"User1|Cpp": { Version: "1.0.2" }
								"User1|TestBuild": { Version: "1.3.0" }
							}
						}
						Build1: {
							"C#": {
								"User1|Soup.CSharp": { Version: "1.0.1" }
							}
						}
					}
				)")));

			auto myProjectOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto myProjectOperationGraphFiles = std::set<FileId>();
			auto myProjectOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(myProjectOperationGraph, myProjectOperationGraphFiles, fileSystemState, myProjectOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(myProjectOperationGraphContent)));

			auto testBuildOperationGraph = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto testBuildOperationGraphFiles = std::set<FileId>();
			auto testBuildOperationGraphContent = std::stringstream();
			OperationGraphWriter::Serialize(testBuildOperationGraph, testBuildOperationGraphFiles, fileSystemState, testBuildOperationGraphContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog"),
				std::make_shared<MockFile>(std::move(testBuildOperationGraphContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/MyPackage/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "MyPackage" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Build", RecipeList({ "User1|TestBuild@1.2.3" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/Recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "TestBuild" },
						{ "Language", "C#|1" },
						{ "Version", "1.3.0" },
					}))
				},
			});
			auto packageProvider = PackageProvider(
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
							PackageName(std::nullopt, "MyPackage"),
							false,
							Path("C:/WorkingDirectory/MyPackage/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/MyPackage/Recipe.sml")),
							PackageChildrenMap({
								{
									"Build",
									{
										PackageChildInfo(PackageReference(std::nullopt, "User1", "TestBuild", SemanticVersion(1, 2, 3)), true, -1, 2),
									}
								},
							}))
					},
					{
						2,
						PackageInfo(
							2,
							PackageName("User1", "TestBuild"),
							false,
							Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/"),
							Path(),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/Recipe.sml")),
							PackageChildrenMap())
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto knownLanguages = std::map<std::string, KnownLanguage>();
			auto locationManager = RecipeBuildLocationManager(knownLanguages);
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState,
				locationManager);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 2>Running Build: [C#]User1|TestBuild",
					"INFO: 2>Build 'User1|TestBuild'",
					"INFO: 2>Checking for existing Evaluate Operation Graph",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"INFO: 2>Previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Loading new Evaluate Operation Graph",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"INFO: 2>Saving updated build state",
					"INFO: 2>Done",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Evaluate Operation Graph",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"INFO: 1>Previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Loading new Evaluate Operation Graph",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"INFO: 1>Saving updated build state",
					"INFO: 1>Done",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/C#/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/packages/RootRecipe.sml",
					"Exists: C:/Users/Me/.soup/RootRecipe.sml",
					"Exists: C:/Users/Me/RootRecipe.sml",
					"Exists: C:/Users/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bog",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/RootRecipe.sml",
					"Exists: C:/RootRecipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Evaluate.bor",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");

			// Verify expected process requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					"GetCurrentProcessFileName",
				}),
				processManager->GetRequests(),
				"Verify process manager requests match expected.");

			// Verify expected evaluate requests
			Assert::AreEqual(
				std::vector<std::string>({
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
					"Evaluate: C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto testBuildGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable()
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/") },
							{ "/(TARGET_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_User1|TestBuild)/"),
							std::string("/(TARGET_User1|TestBuild)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_User1|TestBuild)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable()
					},
					{
						"GenerateSubGraphMacros",
						ValueTable()
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_User1|TestBuild)/") },
									{ "TargetDirectory", std::string("/(TARGET_User1|TestBuild)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "HostValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(testBuildGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto testBuildGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/Generate.bor"));
			auto testBuildGenerateResults = OperationResultsReader::Deserialize(testBuildGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				testBuildGenerateResults,
				"Verify test build generate results content match expected.");

			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/GenerateInput.bvt"));
			Assert::AreEqual(
				ValueTable(
				{
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"User1|TestBuild",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/.soup/") },
										})
									},
								})
							},
						})
					},
					{
						"EvaluateMacros",
						ValueTable(
						{
							{ "/(PACKAGE_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/") },
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_MyPackage)/"),
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_MyPackage)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_User1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/zDqRc65c9x3jySpevCCCyZ15fGs/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_User1|TestBuild)/", std::string("/(BUILD_TARGET_User1|TestBuild)/") },
						})
					},
					{
						"GlobalState",
						ValueTable(
						{
							{
								"Context",
								ValueTable(
								{
									{ "HostPlatform", std::string("TestPlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_MyPackage)/") },
									{ "TargetDirectory", std::string("/(TARGET_MyPackage)/") },
								})
							},
							{
								"Dependencies",
								ValueTable(
								{
									{
										"Build",
										ValueTable(
										{
											{
												"User1|TestBuild",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("User1|TestBuild@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_User1|TestBuild)/") },
														})
													},
												})
											},
										})
									},
								})
							},
							{
								"FileSystem",
								ValueList({
									std::string("Recipe.sml"),
								})
							},
							{
								"Parameters",
								ValueTable(
								{
									{ "ArgumentValue", true },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/WorkingDirectory/MyPackage/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/zxAcy-Et010fdZUKLgFemwwWuC8/.soup/Generate.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							GetEpochTime(),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify my package generate results content match expected.");
		}

	private:
		static std::chrono::time_point<std::chrono::file_clock> GetEpochTime()
		{
			return std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::time_point<std::chrono::system_clock>());
		}
	};
}
