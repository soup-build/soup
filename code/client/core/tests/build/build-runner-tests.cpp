// <copyright file="build-runner-tests.cpp" company="Soup">
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
#include <unordered_map>
#include <vector>

export module Soup.Core.Tests:BuildRunnerTests;

import :MockEvaluateEngine;
import :TestHelpers;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.SML;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::SML;
using namespace Soup::Test;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Soup::Core::Build::UnitTests
{
	export class BuildRunnerTests
	{
	public:
		// [[Fact]]
		void Initialize_Success()
		{
			auto arguments = RecipeBuildArguments();
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>();
			auto recipeCache = RecipeCache();
			auto packageProvider = PackageProvider(
				1,
				PackageGraphLookupMap(),
				PackageLookupMap(),
				PackageTargetDirectories());
			auto evaluateEngine = MockEvaluateEngine();
			auto fileSystemState = FileSystemState();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
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
				std::unordered_map<FileId, Path>({}),
				TestHelpers::BuildDirectoryLookup({
					Path("C:/WorkingDirectory/my-package/recipe.sml"),
				}),
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({}));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/my-package/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto generateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto generateResultFiles = std::set<FileId>();
			auto generateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(generateResult, generateResultFiles, fileSystemState, generateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(generateResultContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.Parallelization = 1;
			arguments.HostPlatform = "FakePlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/my-package/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "my-package" },
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
							PackageName(std::nullopt, "my-package"),
							std::nullopt,
							Path("C:/WorkingDirectory/my-package/"),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
							PackageChildrenMap())
					},
				}),
				PackageTargetDirectories(
				{
					{
						1,
						{
							{ 1, Path("C:/WorkingDirectory/my-package/out/HASH1/") }
						}
					}
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 1>Running Build: [C++]my-package",
					"INFO: 1>Build 'my-package'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase1 no previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Done!",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/my-package/out/HASH1/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
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
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt"));
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
							{ "/(PACKAGE_my-package)/", std::string("C:/WorkingDirectory/my-package/") },
							{ "/(TARGET_my-package)/", std::string("C:/WorkingDirectory/my-package/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_my-package)/"),
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_my-package)/"),
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_my-package)/") },
									{ "TargetDirectory", std::string("/(TARGET_my-package)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("recipe.sml"),
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
						std::string("C:/WorkingDirectory/my-package/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor"));
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
		void Execute_NoDependencies_HasPreprocessors()
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
					Path("C:/WorkingDirectory/my-package/recipe.sml"),
				}),
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
				}));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/my-package/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto generateResultPhase1 = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				true);
			auto generateResultPhase1Files = std::set<FileId>();
			auto generateResultPhase1Content = std::stringstream();
			GenerateResultWriter::Serialize(generateResultPhase1, generateResultPhase1Files, fileSystemState, generateResultPhase1Content);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(generateResultPhase1Content)));

			auto generateResultPhase2 = OperationGraph(
				std::vector<OperationId>(),
				std::vector<OperationInfo>());
			auto generateResultPhase2Files = std::set<FileId>();
			auto generateResultPhase2Content = std::stringstream();
			OperationGraphWriter::Serialize(generateResultPhase2, generateResultPhase2Files, fileSystemState, generateResultPhase2Content);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bog"),
				std::make_shared<MockFile>(std::move(generateResultPhase2Content)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.Parallelization = 1;
			arguments.HostPlatform = "FakePlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/my-package/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "my-package" },
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
							PackageName(std::nullopt, "my-package"),
							std::nullopt,
							Path("C:/WorkingDirectory/my-package/"),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
							PackageChildrenMap())
					},
				}),
				PackageTargetDirectories(
				{
					{
						1,
						{
							{ 1, Path("C:/WorkingDirectory/my-package/out/HASH1/") }
						}
					}
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 1>Running Build: [C++]my-package",
					"INFO: 1>Build 'my-package'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase1 no previous results found",
					"INFO: 1>Checking for existing Generate Phase 2 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bog",
					"INFO: 1>Phase2 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase2.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase2 no previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Load update Generate Phase 2 Result",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Done!",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/my-package/out/HASH1/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bog",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase2.bor",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase2.bog",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase2.bor",
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
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt"));
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
							{ "/(PACKAGE_my-package)/", std::string("C:/WorkingDirectory/my-package/") },
							{ "/(TARGET_my-package)/", std::string("C:/WorkingDirectory/my-package/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_my-package)/"),
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_my-package)/"),
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_my-package)/") },
									{ "TargetDirectory", std::string("/(TARGET_my-package)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("recipe.sml"),
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
						std::string("C:/WorkingDirectory/my-package/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor"));
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
					Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/recipe.sml"),
					Path("C:/WorkingDirectory/my-package/recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/my-package/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto myProjectGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto myProjectGenerateResultFiles = std::set<FileId>();
			auto myProjectGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(myProjectGenerateResult, myProjectGenerateResultFiles, fileSystemState, myProjectGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(myProjectGenerateResultContent)));

			auto testBuildGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto testBuildGenerateResultFiles = std::set<FileId>();
			auto testBuildGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(testBuildGenerateResult, testBuildGenerateResultFiles, fileSystemState,  testBuildGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(testBuildGenerateResultContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.Parallelization = 1;
			arguments.HostPlatform = "FakePlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/my-package/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "my-package" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Build", RecipeList({ "user1|TestBuild@1.2.3" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/recipe.sml",
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
							PackageName(std::nullopt, "my-package"),
							std::nullopt,
							Path("C:/WorkingDirectory/my-package/"),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
							PackageChildrenMap({
								{
									"Build",
									{
										PackageChildInfo(
											PackageReference(std::nullopt, "user1", "TestBuild", SemanticVersion(1, 2, 3)),
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
							PackageName("user1", "TestBuild"),
							std::nullopt,
							Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/"),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/recipe.sml")),
							PackageChildrenMap())
					},
				}),
				PackageTargetDirectories(
				{
					{
						1,
						{
							{ 1, Path("C:/WorkingDirectory/my-package/out/HASH1/") }
						}
					},
					{
						2,
						{
							{ 2, Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/") }
						}
					}
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 2>Running Build: [C#]user1|TestBuild",
					"INFO: 2>Build 'user1|TestBuild'",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bgr",
					"INFO: 2>Phase1 previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/evaluate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>Phase1 no previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-input.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Save operation results",
					"INFO: 2>Loading updated Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bgr",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/temp/",
					"INFO: 2>Save operation results",
					"INFO: 2>Done!",
					"DIAG: 1>Running Build: [C++]my-package",
					"INFO: 1>Build 'my-package'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase1 no previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Done!",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/evaluate-phase1.bor",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/my-package/out/HASH1/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
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
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto testBuildGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-input.bvt"));
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
							{ "/(PACKAGE_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/") },
							{ "/(TARGET_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_user1|TestBuild)/"),
							std::string("/(TARGET_user1|TestBuild)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_user1|TestBuild)/"),
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_user1|TestBuild)/") },
									{ "TargetDirectory", std::string("/(TARGET_user1|TestBuild)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("recipe.sml"),
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
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/generate-phase1.bor"));
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
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt"));
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
										"user1|TestBuild",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/.soup/") },
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
							{ "/(PACKAGE_my-package)/", std::string("C:/WorkingDirectory/my-package/") },
							{ "/(TARGET_my-package)/", std::string("C:/WorkingDirectory/my-package/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_my-package)/"),
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.2.3/out/HASH2/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_user1|TestBuild)/", std::string("/(BUILD_TARGET_user1|TestBuild)/") },
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_my-package)/") },
									{ "TargetDirectory", std::string("/(TARGET_my-package)/") },
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
												"user1|TestBuild",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("user1|TestBuild@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_user1|TestBuild)/") },
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
									std::string("recipe.sml"),
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
						std::string("C:/WorkingDirectory/my-package/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor"));
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
					Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/recipe.sml"),
					Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/recipe.sml"),
					Path("C:/WorkingDirectory/my-package/recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/my-package/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			auto myProjectGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto myProjectGenerateResultFiles = std::set<FileId>();
			auto myProjectGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(myProjectGenerateResult, myProjectGenerateResultFiles, fileSystemState, myProjectGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(myProjectGenerateResultContent)));
			auto packageAGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto packageAGenerateResultFiles = std::set<FileId>();
			auto packageAGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(packageAGenerateResult, packageAGenerateResultFiles, fileSystemState, packageAGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(packageAGenerateResultContent)));
			auto packageBGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto packageBGenerateResultFiles = std::set<FileId>();
			auto packageBGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(packageBGenerateResult, packageBGenerateResultFiles, fileSystemState, packageBGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(packageBGenerateResultContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.Parallelization = 1;
			arguments.HostPlatform = "FakePlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/my-package/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "my-package" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Runtime", RecipeList({ "user1|package-a@1.2.3", "user1|package-b@1.1.1" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "package-a" },
						{ "Language", "C++|1" },
						{ "Version", "1.2.3" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Runtime", RecipeList({ "user1|package-b@1.1.1" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "package-b" },
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
							PackageName(std::nullopt, "my-package"),
							std::nullopt,
							Path("C:/WorkingDirectory/my-package/"),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
							PackageChildrenMap({
								{
									"Runtime",
									{
										PackageChildInfo(PackageReference(std::nullopt, "user1", "package-a", SemanticVersion(1, 2, 3)), false, 2, -1),
										PackageChildInfo(PackageReference(std::nullopt, "user1", "package-b", SemanticVersion(1, 1, 1)), false, 3, -1),
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
							Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/"),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/recipe.sml")),
							PackageChildrenMap({
								{
									"Runtime",
									{
										PackageChildInfo(PackageReference(std::nullopt, "user1", "package-b", SemanticVersion(1, 1, 1)), false, 3, -1),
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
							Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/"),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/recipe.sml")),
							PackageChildrenMap())
					},
				}),
				PackageTargetDirectories(
				{
					{
						1,
						{
							{ 1, Path("C:/WorkingDirectory/my-package/out/HASH1/") },
							{ 2, Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/") },
							{ 3, Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
						}
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 3>Running Build: [C++]user1|package-b",
					"INFO: 3>Build 'user1|package-b'",
					"INFO: 3>Checking for existing Generate Phase 1 Result",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 3>Phase1 previous graph found",
					"INFO: 3>Checking for existing Evaluate Operation Results",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 3>Operation results file does not exist",
					"INFO: 3>Phase1 no previous results found",
					"INFO: 3>Create Directory: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/",
					"INFO: 3>Check outdated generate input file: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-input.bvt",
					"INFO: 3>Value Table file does not exist",
					"INFO: 3>Save Generate Input file",
					"INFO: 3>Checking for existing Generate Operation Results",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 3>Operation results file does not exist",
					"INFO: 3>No previous results found",
					"INFO: 3>Save operation results",
					"INFO: 3>Loading updated Generate Phase 1 Result",
					"DIAG: 3>C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 3>Map previous operation graph observed results",
					"INFO: 3>Create Directory: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/temp/",
					"INFO: 3>Save operation results",
					"INFO: 3>Done!",
					"DIAG: 2>Running Build: [C++]user1|package-a",
					"INFO: 2>Build 'user1|package-a'",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 2>Phase1 previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>Phase1 no previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-input.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Save operation results",
					"INFO: 2>Loading updated Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/temp/",
					"INFO: 2>Save operation results",
					"INFO: 2>Done!",
					"DIAG: 3>Running Build: [C++]user1|package-b",
					"DIAG: 3>Recipe already built: [C++]user1|package-b",
					"DIAG: 1>Running Build: [C++]my-package",
					"INFO: 1>Build 'my-package'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase1 no previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Done!",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/evaluate-phase1.bor",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/evaluate-phase1.bor",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/my-package/out/HASH1/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
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
					"Evaluate: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto packageAGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-input.bvt"));
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
										"user1|package-b",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/") },
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
							{ "/(PACKAGE_user1|package-a)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/") },
							{ "/(TARGET_user1|package-a)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/") },
							{ "/(TARGET_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_user1|package-a)/"),
							std::string("/(TARGET_user1|package-a)/"),
							std::string("/(TARGET_user1|package-b)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_user1|package-a)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(TARGET_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_user1|package-a)/") },
									{ "TargetDirectory", std::string("/(TARGET_user1|package-a)/") },
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
												"user1|package-b",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("user1|package-b@1.1.1") },
															{ "TargetDirectory", std::string("/(TARGET_user1|package-b)/") },
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
									std::string("recipe.sml"),
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
						std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(packageAGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto packageAGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/generate-phase1.bor"));
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
				Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-input.bvt"));
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
							{ "/(PACKAGE_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/") },
							{ "/(TARGET_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_user1|package-b)/"),
							std::string("/(TARGET_user1|package-b)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_user1|package-b)/"),
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_user1|package-b)/") },
									{ "TargetDirectory", std::string("/(TARGET_user1|package-b)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("recipe.sml"),
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
						std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(packageBGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto packageBGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/generate-phase1.bor"));
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
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt"));
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
										"user1|package-a",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/.soup/") },
										})
									},
									{
										"user1|package-b",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/.soup/") },
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
							{ "/(PACKAGE_my-package)/", std::string("C:/WorkingDirectory/my-package/") },
							{ "/(TARGET_my-package)/", std::string("C:/WorkingDirectory/my-package/out/HASH1/") },
							{ "/(TARGET_user1|package-a)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/") },
							{ "/(TARGET_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_my-package)/"),
							std::string("/(TARGET_my-package)/"),
							std::string("/(TARGET_user1|package-a)/"),
							std::string("/(TARGET_user1|package-b)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(TARGET_user1|package-a)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-a/1.2.3/out/HASH1/") },
							{ "/(TARGET_user1|package-b)/", std::string("C:/Users/Me/.soup/packages/C++/user1/package-b/1.1.1/out/HASH1/") },
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_my-package)/") },
									{ "TargetDirectory", std::string("/(TARGET_my-package)/") },
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
												"user1|package-a",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("user1|package-a@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_user1|package-a)/") },
														})
													},
												})
											},
											{
												"user1|package-b",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("user1|package-b@1.1.1") },
															{ "TargetDirectory", std::string("/(TARGET_user1|package-b)/") },
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
									std::string("recipe.sml"),
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
						std::string("C:/WorkingDirectory/my-package/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor"));
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
					Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/recipe.sml"),
					Path("C:/WorkingDirectory/my-package/recipe.sml"),
				}),
				{});

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/my-package/out/HASH1/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 6
					Closure: {
						"C#": {
							"user1|test-build": { Version: "1.3.0", Build: "Build1" }
						}
						"C++": {
							'my-package': { Version: "../my-package/", Build: "Build0" }
						}
					}
					Builds: {
						Build0: {
							"C#": {
								"user1|cpp": { Version: "1.0.2" }
								"user1|test-build": { Version: "1.3.0" }
							}
						}
						Build1: {
							"C#": {
								"user1|csharp": { Version: "1.0.1" }
							}
						}
					}
				)")));

			auto myProjectGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto myProjectGenerateResultFiles = std::set<FileId>();
			auto myProjectGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(myProjectGenerateResult, myProjectGenerateResultFiles, fileSystemState, myProjectGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(myProjectGenerateResultContent)));

			auto testBuildGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto testBuildGenerateResultFiles = std::set<FileId>();
			auto testBuildGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(testBuildGenerateResult, testBuildGenerateResultFiles, fileSystemState, testBuildGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(testBuildGenerateResultContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto arguments = RecipeBuildArguments();
			arguments.Parallelization = 1;
			arguments.HostPlatform = "FakePlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/my-package/");
			auto userDataPath = Path("C:/Users/Me/.soup/");
			auto systemReadAccess = std::vector<Path>({
				Path("C:/FakeSystem/"),
			});
			auto recipeCache = RecipeCache({
				{
					"C:/WorkingDirectory/my-package/recipe.sml",
					Recipe(RecipeTable(
					{
						{ "Name", "my-package" },
						{ "Language", "C++|1" },
						{ "Version", "1.0.0" },
						{
							"Dependencies",
							RecipeTable(
							{
								{ "Build", RecipeList({ "user1|TestBuild@1.2.3" }) },
							})
						},
					}))
				},
				{
					"C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/recipe.sml",
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
							PackageName(std::nullopt, "my-package"),
							std::nullopt,
							Path("C:/WorkingDirectory/my-package/"),
							&recipeCache.GetRecipe(Path("C:/WorkingDirectory/my-package/recipe.sml")),
							PackageChildrenMap({
								{
									"Build",
									{
										PackageChildInfo(PackageReference(std::nullopt, "user1", "TestBuild", SemanticVersion(1, 2, 3)), true, -1, 2),
									}
								},
							}))
					},
					{
						2,
						PackageInfo(
							2,
							PackageName("user1", "TestBuild"),
							std::nullopt,
							Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/"),
							&recipeCache.GetRecipe(Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/recipe.sml")),
							PackageChildrenMap())
					},
				}),
				PackageTargetDirectories(
				{
					{
						1,
						{
							{ 1, Path("C:/WorkingDirectory/my-package/out/HASH1/") },
						}
					},
					{
						2,
						{
							{ 2, Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/") },
						}
					},
				}));
			auto evaluateEngine = MockEvaluateEngine();
			auto uut = BuildRunner(
				arguments,
				userDataPath,
				systemReadAccess,
				recipeCache,
				packageProvider,
				evaluateEngine,
				fileSystemState);
			uut.Execute();

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: 2>Running Build: [C#]user1|TestBuild",
					"INFO: 2>Build 'user1|TestBuild'",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bgr",
					"INFO: 2>Phase1 previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/evaluate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>Phase1 no previous results found",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-input.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"INFO: 2>Save operation results",
					"INFO: 2>Loading updated Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bgr",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/temp/",
					"INFO: 2>Save operation results",
					"INFO: 2>Done!",
					"DIAG: 1>Running Build: [C++]my-package",
					"INFO: 1>Build 'my-package'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>Phase1 no previous results found",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"INFO: 1>Save operation results",
					"INFO: 1>Done!",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/temp/",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/evaluate-phase1.bor",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/my-package/out/HASH1/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"CreateDirectory: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"OpenWriteBinary: C:/WorkingDirectory/my-package/out/HASH1/.soup/evaluate-phase1.bor",
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
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/temp/",
					"Evaluate: C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
					"Evaluate: C:/WorkingDirectory/my-package/out/HASH1/temp/",
				}),
				evaluateEngine.GetRequests(),
				"Verify evaluate requests match expected.");

			// Verify files
			auto testBuildGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-input.bvt"));
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
							{ "/(PACKAGE_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/") },
							{ "/(TARGET_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_user1|TestBuild)/"),
							std::string("/(TARGET_user1|TestBuild)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_user1|TestBuild)/"),
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_user1|TestBuild)/") },
									{ "TargetDirectory", std::string("/(TARGET_user1|TestBuild)/") },
								})
							},
							{ "Dependencies", ValueTable() },
							{
								"FileSystem",
								ValueList({
									std::string("recipe.sml"),
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
				Path("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/generate-phase1.bor"));
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
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-input.bvt"));
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
										"user1|TestBuild",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/.soup/") },
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
							{ "/(PACKAGE_my-package)/", std::string("C:/WorkingDirectory/my-package/") },
							{ "/(TARGET_my-package)/", std::string("C:/WorkingDirectory/my-package/out/HASH1/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_my-package)/"),
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_my-package)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_user1|TestBuild)/", std::string("C:/Users/Me/.soup/packages/C#/TestBuild/1.3.0/out/HASH2/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_user1|TestBuild)/", std::string("/(BUILD_TARGET_user1|TestBuild)/") },
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
									{ "HostPlatform", std::string("FakePlatform") },
									{ "PackageDirectory", std::string("/(PACKAGE_my-package)/") },
									{ "TargetDirectory", std::string("/(TARGET_my-package)/") },
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
												"user1|TestBuild",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("user1|TestBuild@1.2.3") },
															{ "TargetDirectory", std::string("/(TARGET_user1|TestBuild)/") },
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
									std::string("recipe.sml"),
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
						std::string("C:/WorkingDirectory/my-package/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(myPackageGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto myPackageGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/my-package/out/HASH1/.soup/generate-phase1.bor"));
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
