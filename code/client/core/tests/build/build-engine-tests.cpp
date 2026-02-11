// <copyright file="build-engine-tests.cpp" company="Soup">
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

export module Soup.Core.Tests:BuildEngineTests;

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
	export class BuildEngineTests
	{
	public:
		// [[Fact]]
		void Execute_NoDependencies()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test system
			auto system = std::make_shared<MockSystem>();
			auto scopedSystem = ScopedSystemRegister(system);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'MyPackage'
					Language: (C++@0.8)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Cpp'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Wren'
					Language: (Wren@1)
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							'C++': {
								MyPackage: { Version: '../MyPackage/', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Cpp': { Version: 0.8.2 }
							}
						}
						Tool0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							Wren: {
								'Soup|Cpp': { Version: './', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Wren': { Version: 0.5.4 }
							}
						}
						Tool0: {}
					}
				)")));

			auto fileSystemState = FileSystemState();

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			// Register the test process manager
			auto monitorProcessManager = std::make_shared<Monitor::MockMonitorProcessManager>();
			auto scopedMonitorProcessManager = Monitor::ScopedMonitorProcessManagerRegister(monitorProcessManager);

			// Emulate generate phase
			monitorProcessManager->RegisterExecuteCallback(
				std::format("CreateMonitorProcess: 2 [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
				[&](Monitor::ISystemAccessMonitor& /*monitor*/)
				{
					auto myPackageGenerateResult = GenerateResult(
						OperationGraph(
							std::vector<OperationId>(),
							std::vector<OperationInfo>()),
						false);
					auto myPackageFiles = std::set<FileId>();
					auto myPackageGenerateResultContent = std::stringstream();
					GenerateResultWriter::Serialize(myPackageGenerateResult, myPackageFiles, fileSystemState, myPackageGenerateResultContent);
					fileSystem->CreateMockFile(
						Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr"),
						std::make_shared<MockFile>(std::move(myPackageGenerateResultContent)));
				});

			monitorProcessManager->RegisterExecuteCallback(
				std::format("CreateMonitorProcess: 1 [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
				[&](Monitor::ISystemAccessMonitor& /*monitor*/)
				{
					auto soupCppGenerateResult = GenerateResult(
						OperationGraph(
							std::vector<OperationId>(),
							std::vector<OperationInfo>()),
						false);
					auto soupCppFiles = std::set<FileId>();
					auto soupCppGenerateResultContent = std::stringstream();
					GenerateResultWriter::Serialize(soupCppGenerateResult, soupCppFiles, fileSystemState, soupCppGenerateResultContent);
					fileSystem->CreateMockFile(
						Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr"),
						std::make_shared<MockFile>(std::move(soupCppGenerateResultContent)));
				});

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");

			// Load user config state
			auto userDataPath = BuildEngine::GetSoupUserDataPath();
			auto recipeCache = RecipeCache();

			auto packageProvider = BuildEngine::LoadBuildGraph(
				arguments.WorkingDirectory,
				arguments.GlobalParameters,
				userDataPath,
				recipeCache);

			BuildEngine::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				recipeCache);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/MyPackage/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"DIAG: 0>Package was prebuilt: Soup|Wren",
					"DIAG: 2>Running Build: [Wren]Soup|Cpp",
					"INFO: 2>Build 'Soup|Cpp'",
					"INFO: 2>Preload Directory Missing: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"INFO: 2>Generate result file does not exist",
					"INFO: 2>Phase1 no previous graph",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Check for previous operation invocation",
					"INFO: 2>Operation has no successful previous invocation",
					"HIGH: 2>Generate Core: [Wren]Soup|Cpp",
					std::format("DIAG: 2>Execute: [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/", GetGenerateExePath()),
					"DIAG: 2>Allowed Read Access:",
					std::format("DIAG: 2>{0}", GetRuntimeLibraryPath()),
					"DIAG: 2>C:/Users/Me/.soup/local-user-config.sml",
					"DIAG: 2>C:/Windows/",
					"DIAG: 2>C:/Program Files/dotnet/",
					"DIAG: 2>C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 2>Allowed Write Access:",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Save operation results",
					"INFO: 2>Loading updated Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Nothing to do.",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Preload Directory Missing: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"INFO: 1>Generate result file does not exist",
					"INFO: 1>Phase1 no previous graph",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Operation has no successful previous invocation",
					"HIGH: 1>Generate Core: [C++]MyPackage",
					std::format("DIAG: 1>Execute: [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/", GetGenerateExePath()),
					"DIAG: 1>Allowed Read Access:",
					std::format("DIAG: 1>{0}", GetRuntimeLibraryPath()),
					"DIAG: 1>C:/Users/Me/.soup/local-user-config.sml",
					"DIAG: 1>C:/Windows/",
					"DIAG: 1>C:/Program Files/dotnet/",
					"DIAG: 1>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Allowed Write Access:",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Nothing to do.",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentTime",
					"GetCurrentTime",
				}),
				system->GetRequests(),
				"Verify system requests match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentDirectory",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"TryOpenReadBinary: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"TryGetDirectoryFilesLastWriteTime: C:/BuiltIn/Packages/Soup/Wren/0.5.4/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
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

			Assert::AreEqual(
				std::vector<std::string>({
					std::format("CreateMonitorProcess: 1 [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetStandardOutput: 1",
					"GetStandardError: 1",
					"GetExitCode: 1",
					std::format("CreateMonitorProcess: 2 [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
					"ProcessStart: 2",
					"WaitForExit: 2",
					"GetStandardOutput: 2",
					"GetStandardError: 2",
					"GetExitCode: 2",
				}),
				monitorProcessManager->GetRequests(),
				"Verify monitor process manager requests match expected.");

			// Verify files
			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt"));
			Assert::AreEqual(
				ValueTable({
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"Soup|Cpp",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/") },
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
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/") },
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
							{ "/(BUILD_TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_Soup|Cpp)/", std::string("/(BUILD_TARGET_Soup|Cpp)/") },
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
												"Soup|Cpp",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("[Wren]Soup|Cpp@0.8.2") },
															{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
							{ "Parameters", ValueTable() },
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
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							std::chrono::clock_cast<std::chrono::file_clock>(
								std::chrono::time_point<std::chrono::system_clock>()),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify my package generate results content match expected.");

			auto soupCppGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt"));
			Assert::AreEqual(
				ValueTable({
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"Soup|Wren",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/.soup/") },
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
							{ "/(PACKAGE_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/") },
							{ "/(TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_Soup|Cpp)/"),
							std::string("/(TARGET_Soup|Cpp)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_Soup|Cpp)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_Soup|Wren)/", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_Soup|Wren)/", std::string("/(BUILD_TARGET_Soup|Wren)/") },
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
									{ "PackageDirectory", std::string("/(PACKAGE_Soup|Cpp)/") },
									{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
												"Soup|Wren",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("[Wren]Soup|Wren@0.5.4") },
															{ "TargetDirectory", std::string("/(TARGET_Soup|Wren)/") },
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
									{ "System", std::string("Windows") },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(soupCppGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto soupCppGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor"));
			auto soupCppGenerateResults = OperationResultsReader::Deserialize(soupCppGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							std::chrono::clock_cast<std::chrono::file_clock>(
								std::chrono::time_point<std::chrono::system_clock>()),
							{},
							{})
					},
				}),
				soupCppGenerateResults,
				"Verify my package generate results content match expected.");
		}

		// [[Fact]]
		void Execute_NoDependencies_UpToDate()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test system
			auto system = std::make_shared<MockSystem>();
			auto scopedSystem = ScopedSystemRegister(system);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			fileSystem->CreateMockFile(
				Path(GetGenerateExePath()),
				std::make_shared<MockFile>(
					std::chrono::clock_cast<std::chrono::file_clock>(
						std::chrono::sys_days{January/9/2024} + 11h + 3min + 4s)));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'MyPackage'
					Language: (C++@0.8)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Cpp'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Wren'
					Language: (Wren@1)
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							'C++': {
								MyPackage: { Version: '../MyPackage/', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Cpp': { Version: 0.8.2 }
							}
						}
						Tool0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							Wren: {
								'Soup|Cpp': { Version: './', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Wren': { Version: 0.5.4 }
							}
						}
						Tool0: {}
					}
				)")));

			auto fileSystemState = FileSystemState();

			auto myPackageGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto myPackageFiles = std::set<FileId>();
			auto myPackageGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(myPackageGenerateResult, myPackageFiles, fileSystemState, myPackageGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(myPackageGenerateResultContent)));

			auto soupCppGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto soupCppFiles = std::set<FileId>();
			auto soupCppGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(soupCppGenerateResult, soupCppFiles, fileSystemState, soupCppGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(soupCppGenerateResultContent)));

			auto soupCppGenerateInput = ValueTable({
				{
					"Dependencies",
					ValueTable(
					{
						{
							"Build",
							ValueTable(
							{
								{
									"Soup|Wren",
									ValueTable(
									{
										{ "SoupTargetDirectory", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/.soup/") },
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
						{ "/(PACKAGE_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/") },
						{ "/(TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
					})
				},
				{
					"EvaluateReadAccess",
					ValueList(
					{
						std::string("/(PACKAGE_Soup|Cpp)/"),
						std::string("/(TARGET_Soup|Cpp)/"),
					})
				},
				{
					"EvaluateWriteAccess",
					ValueList(
					{
						std::string("/(TARGET_Soup|Cpp)/"),
					})
				},
				{
					"GenerateMacros",
					ValueTable(
					{
						{ "/(BUILD_TARGET_Soup|Wren)/", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/") },
					})
				},
				{
					"GenerateSubGraphMacros",
					ValueTable(
					{
						{ "/(TARGET_Soup|Wren)/", std::string("/(BUILD_TARGET_Soup|Wren)/") },
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
								{ "PackageDirectory", std::string("/(PACKAGE_Soup|Cpp)/") },
								{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
											"Soup|Wren",
											ValueTable(
											{
												{
													"Context",
													ValueTable(
													{
														{ "Reference", std::string("[Wren]Soup|Wren@0.5.4") },
														{ "TargetDirectory", std::string("/(TARGET_Soup|Wren)/") },
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
								{ "System", std::string("Windows") },
							})
						},
					})
				},
				{
					"PackageRoot",
					std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/")
				},
				{
					"UserDataPath",
					std::string("C:/Users/Me/.soup/")
				},
			});
			auto soupCppGenerateInputContent = std::stringstream();
			ValueTableWriter::Serialize(soupCppGenerateInput, soupCppGenerateInputContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt"),
				std::make_shared<MockFile>(std::move(soupCppGenerateInputContent)));

			auto myPackageGenerateInput = ValueTable({
				{
					"Dependencies",
					ValueTable(
					{
						{
							"Build",
							ValueTable(
							{
								{
									"Soup|Cpp",
									ValueTable(
									{
										{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/") },
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
						{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/") },
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
						{ "/(BUILD_TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
					})
				},
				{
					"GenerateSubGraphMacros",
					ValueTable(
					{
						{ "/(TARGET_Soup|Cpp)/", std::string("/(BUILD_TARGET_Soup|Cpp)/") },
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
											"Soup|Cpp",
											ValueTable(
											{
												{
													"Context",
													ValueTable(
													{
														{ "Reference", std::string("[Wren]Soup|Cpp@0.8.2") },
														{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
						{ "Parameters", ValueTable() },
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
			});
			auto myPackageGenerateInputContent = std::stringstream();
			ValueTableWriter::Serialize(myPackageGenerateInput, myPackageGenerateInputContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt"),
				std::make_shared<MockFile>(std::move(myPackageGenerateInputContent)));

			auto myPackageGenerateResults = OperationResults({
				{
					1,
					OperationResult(
						true,
						std::chrono::clock_cast<std::chrono::file_clock>(
							std::chrono::sys_days{January/9/2024} + 12h + 35min + 34s),
						{},
						{})
				},
			});
			auto myPackageGenerateResultsContent = std::stringstream();
			auto myPackageGenerateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(myPackageGenerateResults, myPackageGenerateResultsFiles, fileSystemState,  myPackageGenerateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor"),
				std::make_shared<MockFile>(std::move(myPackageGenerateResultsContent)));

			auto soupCppGenerateResults = OperationResults({
				{
					1,
					OperationResult(
						true,
						std::chrono::clock_cast<std::chrono::file_clock>(
							std::chrono::sys_days{January/9/2024} + 12h + 13min + 23s),
						{},
						{})
				},
			});
			auto soupCppGenerateResultsContent = std::stringstream();
			auto soupCppGenerateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(soupCppGenerateResults, soupCppGenerateResultsFiles, fileSystemState, soupCppGenerateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor"),
				std::make_shared<MockFile>(std::move(soupCppGenerateResultsContent)));

			auto soupCppEvaluateResults = OperationResults();
			auto soupCppEvaluateResultsContent = std::stringstream();
			auto soupCppEvaluateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(soupCppEvaluateResults, soupCppEvaluateResultsFiles, fileSystemState, soupCppEvaluateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor"),
				std::make_shared<MockFile>(std::move(soupCppEvaluateResultsContent)));

			auto myPackageEvaluateResults = OperationResults();
			auto myPackageEvaluateResultsContent = std::stringstream();
			auto myPackageEvaluateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(myPackageEvaluateResults, myPackageEvaluateResultsFiles, fileSystemState, myPackageEvaluateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor"),
				std::make_shared<MockFile>(std::move(myPackageEvaluateResultsContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			// Register the test process manager
			auto monitorProcessManager = std::make_shared<Monitor::MockMonitorProcessManager>();
			auto scopedMonitorProcessManager = Monitor::ScopedMonitorProcessManagerRegister(monitorProcessManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");

			// Load user config state
			auto userDataPath = BuildEngine::GetSoupUserDataPath();
			auto recipeCache = RecipeCache();

			auto packageProvider = BuildEngine::LoadBuildGraph(
				arguments.WorkingDirectory,
				arguments.GlobalParameters,
				userDataPath,
				recipeCache);

			BuildEngine::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				recipeCache);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/MyPackage/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"DIAG: 0>Package was prebuilt: Soup|Wren",
					"DIAG: 2>Running Build: [Wren]Soup|Cpp",
					"INFO: 2>Build 'Soup|Cpp'",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"INFO: 2>Phase1 previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor",
					"INFO: 2>Phase1 previous results found",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"INFO: 2>Previous results found",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Check for previous operation invocation",
					"INFO: 2>Up to date",
					"INFO: 2>Generate Core: [Wren]Soup|Cpp",
					"DIAG: 2>Build evaluation end",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Nothing to do.",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor",
					"INFO: 1>Phase1 previous results found",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",       
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"INFO: 1>Previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Up to date",
					"INFO: 1>Generate Core: [C++]MyPackage",
					"DIAG: 1>Build evaluation end",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Nothing to do.",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected system requests
			Assert::AreEqual(
				std::vector<std::string>(),
				system->GetRequests(),
				"Verify system requests match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentDirectory",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"TryOpenReadBinary: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"TryGetDirectoryFilesLastWriteTime: C:/BuiltIn/Packages/Soup/Wren/0.5.4/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					std::format("TryGetLastWriteTime: {0}", GetGenerateExePath()),
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
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

			Assert::AreEqual(
				std::vector<std::string>(),
				monitorProcessManager->GetRequests(),
				"Verify monitor process manager requests match expected.");
		}

		// [[Fact]]
		void Execute_NoDependencies_HasPreprocessors()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test system
			auto system = std::make_shared<MockSystem>();
			auto scopedSystem = ScopedSystemRegister(system);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'MyPackage'
					Language: (C++@0.8)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Cpp'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Wren'
					Language: (Wren@1)
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							'C++': {
								MyPackage: { Version: '../MyPackage/', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Cpp': { Version: 0.8.2 }
							}
						}
						Tool0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							Wren: {
								'Soup|Cpp': { Version: './', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Wren': { Version: 0.5.4 }
							}
						}
						Tool0: {}
					}
				)")));

			auto fileSystemState = FileSystemState();

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			// Register the test process manager
			auto monitorProcessManager = std::make_shared<Monitor::MockMonitorProcessManager>();
			auto scopedMonitorProcessManager = Monitor::ScopedMonitorProcessManagerRegister(monitorProcessManager);

			// Emulate generate phase
			monitorProcessManager->RegisterExecuteCallback(
				std::format("CreateMonitorProcess: 2 [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
				[&](Monitor::ISystemAccessMonitor& /*monitor*/)
				{
					auto myPackageGenerateResult = GenerateResult(
						OperationGraph(
							std::vector<OperationId>(),
							std::vector<OperationInfo>()),
						true);
					auto myPackageFiles = std::set<FileId>();
					auto myPackageGenerateResultContent = std::stringstream();
					GenerateResultWriter::Serialize(myPackageGenerateResult, myPackageFiles, fileSystemState, myPackageGenerateResultContent);
					fileSystem->CreateMockFile(
						Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr"),
						std::make_shared<MockFile>(std::move(myPackageGenerateResultContent)));
				});

			monitorProcessManager->RegisterExecuteCallback(
				std::format("CreateMonitorProcess: 3 [C:/WorkingDirectory/MyPackage/] {0} false C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
				[&](Monitor::ISystemAccessMonitor& /*monitor*/)
				{
					auto myPackageOperationGraph = OperationGraph(
						std::vector<OperationId>(),
						std::vector<OperationInfo>());
					auto myPackageFiles = std::set<FileId>();
					auto myPackageOperationGraphContent = std::stringstream();
					OperationGraphWriter::Serialize(myPackageOperationGraph, myPackageFiles, fileSystemState, myPackageOperationGraphContent);
					fileSystem->CreateMockFile(
						Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bog"),
						std::make_shared<MockFile>(std::move(myPackageOperationGraphContent)));
				});

			monitorProcessManager->RegisterExecuteCallback(
				std::format("CreateMonitorProcess: 1 [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
				[&](Monitor::ISystemAccessMonitor& /*monitor*/)
				{
					auto soupCppGenerateResult = GenerateResult(
						OperationGraph(
							std::vector<OperationId>(),
							std::vector<OperationInfo>()),
						false);
					auto soupCppFiles = std::set<FileId>();
					auto soupCppGenerateResultContent = std::stringstream();
					GenerateResultWriter::Serialize(soupCppGenerateResult, soupCppFiles, fileSystemState, soupCppGenerateResultContent);
					fileSystem->CreateMockFile(
						Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr"),
						std::make_shared<MockFile>(std::move(soupCppGenerateResultContent)));
				});

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");

			// Load user config state
			auto userDataPath = BuildEngine::GetSoupUserDataPath();
			auto recipeCache = RecipeCache();

			auto packageProvider = BuildEngine::LoadBuildGraph(
				arguments.WorkingDirectory,
				arguments.GlobalParameters,
				userDataPath,
				recipeCache);

			BuildEngine::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				recipeCache);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/MyPackage/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"DIAG: 0>Package was prebuilt: Soup|Wren",
					"DIAG: 2>Running Build: [Wren]Soup|Cpp",
					"INFO: 2>Build 'Soup|Cpp'",
					"INFO: 2>Preload Directory Missing: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"INFO: 2>Generate result file does not exist",
					"INFO: 2>Phase1 no previous graph",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"INFO: 2>Value Table file does not exist",
					"INFO: 2>Save Generate Input file",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"INFO: 2>Operation results file does not exist",
					"INFO: 2>No previous results found",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Check for previous operation invocation",
					"INFO: 2>Operation has no successful previous invocation",
					"HIGH: 2>Generate Core: [Wren]Soup|Cpp",
					std::format("DIAG: 2>Execute: [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/", GetGenerateExePath()),
					"DIAG: 2>Allowed Read Access:",
					std::format("DIAG: 2>{0}", GetRuntimeLibraryPath()),
					"DIAG: 2>C:/Users/Me/.soup/local-user-config.sml",
					"DIAG: 2>C:/Windows/",
					"DIAG: 2>C:/Program Files/dotnet/",
					"DIAG: 2>C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 2>Allowed Write Access:",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Save operation results",
					"INFO: 2>Loading updated Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"DIAG: 2>Map previous operation graph observed results",
					"INFO: 2>Create Directory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Nothing to do.",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Preload Directory Missing: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"INFO: 1>Generate result file does not exist",
					"INFO: 1>Phase1 no previous graph",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",       
					"INFO: 1>Value Table file does not exist",
					"INFO: 1>Save Generate Input file",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Operation has no successful previous invocation",
					"HIGH: 1>Generate Core: [C++]MyPackage",
					std::format("DIAG: 1>Execute: [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/", GetGenerateExePath()),
					"DIAG: 1>Allowed Read Access:",
					std::format("DIAG: 1>{0}", GetRuntimeLibraryPath()),
					"DIAG: 1>C:/Users/Me/.soup/local-user-config.sml",
					"DIAG: 1>C:/Windows/",
					"DIAG: 1>C:/Program Files/dotnet/",
					"DIAG: 1>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Allowed Write Access:",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Save operation results",
					"INFO: 1>Loading updated Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",       
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor",
					"INFO: 1>Operation results file does not exist",
					"INFO: 1>No previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Operation has no successful previous invocation",
					"HIGH: 1>Generate Core: [C++]MyPackage",
					std::format("DIAG: 1>Execute: [C:/WorkingDirectory/MyPackage/] {0} false C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/", GetGenerateExePath()),
					"DIAG: 1>Allowed Read Access:",
					std::format("DIAG: 1>{0}", GetRuntimeLibraryPath()),
					"DIAG: 1>C:/Users/Me/.soup/local-user-config.sml",
					"DIAG: 1>C:/Windows/",
					"DIAG: 1>C:/Program Files/dotnet/",
					"DIAG: 1>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Allowed Write Access:",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Save operation results",
					"INFO: 1>Load update Generate Phase 2 Result",
					"DIAG: 1>Map previous operation graph observed results",
					"INFO: 1>Create Directory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Nothing to do.",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentTime",
					"GetCurrentTime",
					"GetCurrentTime",
				}),
				system->GetRequests(),
				"Verify system requests match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentDirectory",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"TryOpenReadBinary: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"TryGetDirectoryFilesLastWriteTime: C:/BuiltIn/Packages/Soup/Wren/0.5.4/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"CreateDirectory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"CreateDirectory: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor",
					"OpenWriteBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"CreateDirectory: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
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

			Assert::AreEqual(
				std::vector<std::string>({
					std::format("CreateMonitorProcess: 1 [C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/] {0} true C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetStandardOutput: 1",
					"GetStandardError: 1",
					"GetExitCode: 1",
					std::format("CreateMonitorProcess: 2 [C:/WorkingDirectory/MyPackage/] {0} true C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
					"ProcessStart: 2",
					"WaitForExit: 2",
					"GetStandardOutput: 2",
					"GetStandardError: 2",
					"GetExitCode: 2",
					std::format("CreateMonitorProcess: 3 [C:/WorkingDirectory/MyPackage/] {0} false C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/ Environment [2] 1 0 AllowedRead [7] AllowedWrite [1]", GetGenerateExePath()),
					"ProcessStart: 3",
					"WaitForExit: 3",
					"GetStandardOutput: 3",
					"GetStandardError: 3",
					"GetExitCode: 3",
				}),
				monitorProcessManager->GetRequests(),
				"Verify monitor process manager requests match expected.");

			// Verify files
			auto myPackageGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt"));
			Assert::AreEqual(
				ValueTable({
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"Soup|Cpp",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/") },
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
							{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/") },
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
							{ "/(BUILD_TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_Soup|Cpp)/", std::string("/(BUILD_TARGET_Soup|Cpp)/") },
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
												"Soup|Cpp",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("[Wren]Soup|Cpp@0.8.2") },
															{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
							{ "Parameters", ValueTable() },
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
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor"));
			auto myPackageGenerateResults = OperationResultsReader::Deserialize(myPackageGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							std::chrono::clock_cast<std::chrono::file_clock>(
								std::chrono::time_point<std::chrono::system_clock>()),
							{},
							{})
					},
				}),
				myPackageGenerateResults,
				"Verify my package generate results content match expected.");

			auto soupCppGenerateInputMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt"));
			Assert::AreEqual(
				ValueTable({
					{
						"Dependencies",
						ValueTable(
						{
							{
								"Build",
								ValueTable(
								{
									{
										"Soup|Wren",
										ValueTable(
										{
											{ "SoupTargetDirectory", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/.soup/") },
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
							{ "/(PACKAGE_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/") },
							{ "/(TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
						})
					},
					{
						"EvaluateReadAccess",
						ValueList(
						{
							std::string("/(PACKAGE_Soup|Cpp)/"),
							std::string("/(TARGET_Soup|Cpp)/"),
						})
					},
					{
						"EvaluateWriteAccess",
						ValueList(
						{
							std::string("/(TARGET_Soup|Cpp)/"),
						})
					},
					{
						"GenerateMacros",
						ValueTable(
						{
							{ "/(BUILD_TARGET_Soup|Wren)/", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/") },
						})
					},
					{
						"GenerateSubGraphMacros",
						ValueTable(
						{
							{ "/(TARGET_Soup|Wren)/", std::string("/(BUILD_TARGET_Soup|Wren)/") },
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
									{ "PackageDirectory", std::string("/(PACKAGE_Soup|Cpp)/") },
									{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
												"Soup|Wren",
												ValueTable(
												{
													{
														"Context",
														ValueTable(
														{
															{ "Reference", std::string("[Wren]Soup|Wren@0.5.4") },
															{ "TargetDirectory", std::string("/(TARGET_Soup|Wren)/") },
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
									{ "System", std::string("Windows") },
								})
							},
						})
					},
					{
						"PackageRoot",
						std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/")
					},
					{
						"UserDataPath",
						std::string("C:/Users/Me/.soup/")
					},
				}),
				ValueTableReader::Deserialize(soupCppGenerateInputMockFile->Content),
				"Verify file content match expected.");

			auto soupCppGenerateResultsMockFile = fileSystem->GetMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor"));
			auto soupCppGenerateResults = OperationResultsReader::Deserialize(soupCppGenerateResultsMockFile->Content, fileSystemState);

			Assert::AreEqual(
				OperationResults({
					{
						1,
						OperationResult(
							true,
							std::chrono::clock_cast<std::chrono::file_clock>(
								std::chrono::time_point<std::chrono::system_clock>()),
							{},
							{})
					},
				}),
				soupCppGenerateResults,
				"Verify my package generate results content match expected.");
		}

		// [[Fact]]
		void Execute_NoDependencies_HasPreprocessors_UpToDate()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test system
			auto system = std::make_shared<MockSystem>();
			auto scopedSystem = ScopedSystemRegister(system);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			fileSystem->CreateMockFile(
				Path(GetGenerateExePath()),
				std::make_shared<MockFile>(
					std::chrono::clock_cast<std::chrono::file_clock>(
						std::chrono::sys_days{January/9/2024} + 11h + 3min + 4s)));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/"),
				std::make_shared<MockDirectory>(std::vector<Path>({
					Path("./recipe.sml"),
				})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			fileSystem->CreateMockDirectory(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/"),
				std::make_shared<MockDirectory>(std::vector<Path>({})));

			// Create the Recipe to build
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'MyPackage'
					Language: (C++@0.8)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Cpp'
					Language: (Wren@1)
				)")));

			fileSystem->CreateMockFile(
				Path("C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Name: 'Wren'
					Language: (Wren@1)
				)")));

			// Create the package lock
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							'C++': {
								MyPackage: { Version: '../MyPackage/', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Cpp': { Version: 0.8.2 }
							}
						}
						Tool0: {}
					}
				)")));

			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml"),
				std::make_shared<MockFile>(std::stringstream(R"(
					Version: 5
					Closures: {
						Root: {
							Wren: {
								'Soup|Cpp': { Version: './', Build: 'Build0', Tool: 'Tool0' }
							}
						}
						Build0: {
							Wren: {
								'Soup|Wren': { Version: 0.5.4 }
							}
						}
						Tool0: {}
					}
				)")));

			auto fileSystemState = FileSystemState();

			auto myPackageGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				true);
			auto myPackageFiles = std::set<FileId>();
			auto myPackageGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(myPackageGenerateResult, myPackageFiles, fileSystemState, myPackageGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(myPackageGenerateResultContent)));

			auto soupCppGenerateResult = GenerateResult(
				OperationGraph(
					std::vector<OperationId>(),
					std::vector<OperationInfo>()),
				false);
			auto soupCppFiles = std::set<FileId>();
			auto soupCppGenerateResultContent = std::stringstream();
			GenerateResultWriter::Serialize(soupCppGenerateResult, soupCppFiles, fileSystemState, soupCppGenerateResultContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr"),
				std::make_shared<MockFile>(std::move(soupCppGenerateResultContent)));

			auto soupCppGenerateInput = ValueTable({
				{
					"Dependencies",
					ValueTable(
					{
						{
							"Build",
							ValueTable(
							{
								{
									"Soup|Wren",
									ValueTable(
									{
										{ "SoupTargetDirectory", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/.soup/") },
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
						{ "/(PACKAGE_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/") },
						{ "/(TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
					})
				},
				{
					"EvaluateReadAccess",
					ValueList(
					{
						std::string("/(PACKAGE_Soup|Cpp)/"),
						std::string("/(TARGET_Soup|Cpp)/"),
					})
				},
				{
					"EvaluateWriteAccess",
					ValueList(
					{
						std::string("/(TARGET_Soup|Cpp)/"),
					})
				},
				{
					"GenerateMacros",
					ValueTable(
					{
						{ "/(BUILD_TARGET_Soup|Wren)/", std::string("C:/BuiltIn/Packages/Soup/Wren/0.5.4/out/") },
					})
				},
				{
					"GenerateSubGraphMacros",
					ValueTable(
					{
						{ "/(TARGET_Soup|Wren)/", std::string("/(BUILD_TARGET_Soup|Wren)/") },
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
								{ "PackageDirectory", std::string("/(PACKAGE_Soup|Cpp)/") },
								{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
											"Soup|Wren",
											ValueTable(
											{
												{
													"Context",
													ValueTable(
													{
														{ "Reference", std::string("[Wren]Soup|Wren@0.5.4") },
														{ "TargetDirectory", std::string("/(TARGET_Soup|Wren)/") },
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
								{ "System", std::string("Windows") },
							})
						},
					})
				},
				{
					"PackageRoot",
					std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/")
				},
				{
					"UserDataPath",
					std::string("C:/Users/Me/.soup/")
				},
			});
			auto soupCppGenerateInputContent = std::stringstream();
			ValueTableWriter::Serialize(soupCppGenerateInput, soupCppGenerateInputContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt"),
				std::make_shared<MockFile>(std::move(soupCppGenerateInputContent)));

			auto myPackageGenerateInput = ValueTable({
				{
					"Dependencies",
					ValueTable(
					{
						{
							"Build",
							ValueTable(
							{
								{
									"Soup|Cpp",
									ValueTable(
									{
										{ "SoupTargetDirectory", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/") },
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
						{ "/(TARGET_MyPackage)/", std::string("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/") },
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
						{ "/(BUILD_TARGET_Soup|Cpp)/", std::string("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/") },
					})
				},
				{
					"GenerateSubGraphMacros",
					ValueTable(
					{
						{ "/(TARGET_Soup|Cpp)/", std::string("/(BUILD_TARGET_Soup|Cpp)/") },
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
											"Soup|Cpp",
											ValueTable(
											{
												{
													"Context",
													ValueTable(
													{
														{ "Reference", std::string("[Wren]Soup|Cpp@0.8.2") },
														{ "TargetDirectory", std::string("/(TARGET_Soup|Cpp)/") },
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
						{ "Parameters", ValueTable() },
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
			});
			auto myPackageGenerateInputContent = std::stringstream();
			ValueTableWriter::Serialize(myPackageGenerateInput, myPackageGenerateInputContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt"),
				std::make_shared<MockFile>(std::move(myPackageGenerateInputContent)));

			auto myPackageGenerateResults = OperationResults({
				{
					1,
					OperationResult(
						true,
						std::chrono::clock_cast<std::chrono::file_clock>(
							std::chrono::sys_days{January/9/2024} + 12h + 35min + 34s),
						{},
						{})
				},
			});
			auto myPackageGeneratePhase1ResultsContent = std::stringstream();
			auto myPackageGeneratePhase1ResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(myPackageGenerateResults, myPackageGeneratePhase1ResultsFiles, fileSystemState,  myPackageGeneratePhase1ResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor"),
				std::make_shared<MockFile>(std::move(myPackageGeneratePhase1ResultsContent)));

			auto myPackageGeneratePhase2ResultsContent = std::stringstream();
			auto myPackageGeneratePhase2ResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(myPackageGenerateResults, myPackageGeneratePhase2ResultsFiles, fileSystemState,  myPackageGeneratePhase2ResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor"),
				std::make_shared<MockFile>(std::move(myPackageGeneratePhase2ResultsContent)));

			auto soupCppGenerateResults = OperationResults({
				{
					1,
					OperationResult(
						true,
						std::chrono::clock_cast<std::chrono::file_clock>(
							std::chrono::sys_days{January/9/2024} + 12h + 13min + 23s),
						{},
						{})
				},
			});
			auto soupCppGenerateResultsContent = std::stringstream();
			auto soupCppGenerateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(soupCppGenerateResults, soupCppGenerateResultsFiles, fileSystemState, soupCppGenerateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor"),
				std::make_shared<MockFile>(std::move(soupCppGenerateResultsContent)));

			auto soupCppEvaluateResults = OperationResults();
			auto soupCppEvaluateResultsContent = std::stringstream();
			auto soupCppEvaluateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(soupCppEvaluateResults, soupCppEvaluateResultsFiles, fileSystemState, soupCppEvaluateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor"),
				std::make_shared<MockFile>(std::move(soupCppEvaluateResultsContent)));

			auto myPackageEvaluateResults = OperationResults();
			auto myPackageEvaluateResultsContent = std::stringstream();
			auto myPackageEvaluateResultsFiles = std::set<FileId>();
			OperationResultsWriter::Serialize(myPackageEvaluateResults, myPackageEvaluateResultsFiles, fileSystemState, myPackageEvaluateResultsContent);
			fileSystem->CreateMockFile(
				Path("C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor"),
				std::make_shared<MockFile>(std::move(myPackageEvaluateResultsContent)));

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			// Register the test process manager
			auto monitorProcessManager = std::make_shared<Monitor::MockMonitorProcessManager>();
			auto scopedMonitorProcessManager = Monitor::ScopedMonitorProcessManagerRegister(monitorProcessManager);

			auto arguments = RecipeBuildArguments();
			arguments.HostPlatform = "TestPlatform";
			arguments.WorkingDirectory = Path("C:/WorkingDirectory/MyPackage/");

			// Load user config state
			auto userDataPath = BuildEngine::GetSoupUserDataPath();
			auto recipeCache = RecipeCache();

			auto packageProvider = BuildEngine::LoadBuildGraph(
				arguments.WorkingDirectory,
				arguments.GlobalParameters,
				userDataPath,
				recipeCache);

			BuildEngine::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				recipeCache);

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"DIAG: Load PackageLock: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/WorkingDirectory/MyPackage/recipe.sml",
					"DIAG: Load Recipe: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"DIAG: Load PackageLock: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"INFO: Package lock loaded",
					"DIAG: Load Recipe: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"DIAG: 0>Package was prebuilt: Soup|Wren",
					"DIAG: 2>Running Build: [Wren]Soup|Cpp",
					"INFO: 2>Build 'Soup|Cpp'",
					"INFO: 2>Checking for existing Generate Phase 1 Result",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"INFO: 2>Phase1 previous graph found",
					"INFO: 2>Checking for existing Evaluate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor",
					"INFO: 2>Phase1 previous results found",
					"INFO: 2>Check outdated generate input file: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"INFO: 2>Checking for existing Generate Operation Results",
					"DIAG: 2>C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					"INFO: 2>Previous results found",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Check for previous operation invocation",
					"INFO: 2>Up to date",
					"INFO: 2>Generate Core: [Wren]Soup|Cpp",
					"DIAG: 2>Build evaluation end",
					"DIAG: 2>Build evaluation start",
					"DIAG: 2>Build evaluation end",
					"INFO: 2>Nothing to do.",
					"DIAG: 1>Running Build: [C++]MyPackage",
					"INFO: 1>Build 'MyPackage'",
					"INFO: 1>Checking for existing Generate Phase 1 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"INFO: 1>Phase1 previous graph found",
					"INFO: 1>Checking for existing Evaluate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor",
					"INFO: 1>Phase1 previous results found",
					"INFO: 1>Checking for existing Generate Phase 2 Result",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bog",
					"INFO: 1>Operation graph file does not exist",
					"INFO: 1>Phase2 no previous graph",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"INFO: 1>Previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Up to date",
					"INFO: 1>Generate Core: [C++]MyPackage",
					"DIAG: 1>Build evaluation end",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Check outdated generate input file: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"INFO: 1>Checking for existing Generate Operation Results",
					"DIAG: 1>C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor",
					"INFO: 1>Previous results found",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Check for previous operation invocation",
					"INFO: 1>Up to date",
					"INFO: 1>Generate Core: [C++]MyPackage",
					"DIAG: 1>Build evaluation end",
					"DIAG: 1>Build evaluation start",
					"DIAG: 1>Build evaluation end",
					"INFO: 1>Nothing to do.",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected system requests
			Assert::AreEqual(
				std::vector<std::string>(),
				system->GetRequests(),
				"Verify system requests match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentDirectory",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/package-lock.sml",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/recipe.sml",
					"TryOpenReadBinary: C:/Users/Me/.soup/locks/Wren/Soup/Cpp/0.8.2/package-lock.sml",
					"TryOpenReadBinary: C:/BuiltIn/Packages/Soup/Wren/0.5.4/recipe.sml",
					"Exists: C:/WorkingDirectory/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/Wren/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/packages/root-recipe.sml",
					"Exists: C:/Users/Me/.soup/root-recipe.sml",
					"Exists: C:/Users/Me/root-recipe.sml",
					"Exists: C:/Users/root-recipe.sml",
					"Exists: C:/root-recipe.sml",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/",
					"TryGetDirectoryFilesLastWriteTime: C:/BuiltIn/Packages/Soup/Wren/0.5.4/",
					"TryGetDirectoryFilesLastWriteTime: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/evaluate-phase1.bor",
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/.soup/generate-phase1.bor",
					std::format("TryGetLastWriteTime: {0}", GetGenerateExePath()),
					"Exists: C:/Users/Me/.soup/packages/Wren/Soup/Cpp/0.8.2/out/tsWW3RZ_9Jb7Xbk2kTzx3n6uQUM/temp/",
					"TryGetDirectoryFilesLastWriteTime: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bgr",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/evaluate-phase1.bor",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bog",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase1.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-input.bvt",
					"TryOpenReadBinary: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/.soup/generate-phase2.bor",
					"Exists: C:/WorkingDirectory/MyPackage/out/J_HqSstV55vlb-x6RWC_hLRFRDU/temp/",
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

			Assert::AreEqual(
				std::vector<std::string>(),
				monitorProcessManager->GetRequests(),
				"Verify monitor process manager requests match expected.");
		}

	private:
		std::string_view GetRuntimeLibraryPath()
		{
			#ifdef _WIN32
				return "C:/testlocation/";
			#else
				return "C:/lib/soup/";
			#endif
		}

		std::string_view GetGenerateExePath()
		{
			#ifdef _WIN32
				return "C:/testlocation/Soup.Generate.exe";
			#else
				return "C:/lib/soup/generate";
			#endif
		}
	};
}