// <copyright file="build-runner.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <map>
#include <sstream>
#include <string>
#include <vector>

export module Soup.Core:BuildRunner;

import :BuildConstants;
import :BuildFailedException;
import :CommandInfo;
import :DependencyTargetSet;
import :FileSystemState;
import :GenerateResult;
import :GenerateResultManager;
import :IEvaluateEngine;
import :OperationGraph;
import :OperationGraphManager;
import :OperationInfo;
import :OperationResult;
import :OperationResults;
import :OperationResultsManager;
import :PackageProvider;
import :RecipeBuildArguments;
import :RecipeBuildCacheState;
import :RecipeCache;
import :Value;
import :ValueTableManager;
import Opal;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The build runner that knows how to perform the correct build for a recipe
	/// and all of its development and runtime dependencies
	/// </summary>
	export class BuildRunner
	{
	private:
		// Root arguments
		const RecipeBuildArguments& _arguments;

		// System Parameters
		Path _userDataPath;
		const std::vector<Path>& _systemReadAccess;

		// Shared Runtime
		RecipeCache& _recipeCache;
		PackageProvider& _packageProvider;
		IEvaluateEngine& _evaluateEngine;
		FileSystemState& _fileSystemState;

		// Mapping from package id to the required information to be used with dependencies parameters
		std::map<PackageId, RecipeBuildCacheState> _buildCache;

		const std::string _dependencyTypeBuild = "Build";
		const std::string _dependencyTypeTool = "Tool";

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="BuildRunner"/> class.
		/// </summary>
		BuildRunner(
			const RecipeBuildArguments& arguments,
			Path userDataPath,
			const std::vector<Path>& systemReadAccess,
			RecipeCache& recipeCache,
			PackageProvider& packageProvider,
			IEvaluateEngine& evaluateEngine,
			FileSystemState& fileSystemState) :
			_arguments(arguments),
			_userDataPath(std::move(userDataPath)),
			_systemReadAccess(systemReadAccess),
			_recipeCache(recipeCache),
			_packageProvider(packageProvider),
			_evaluateEngine(evaluateEngine),
			_fileSystemState(fileSystemState),
			_buildCache()
		{
		}

		/// <summary>
		/// The Core Execute task
		/// </summary>
		void Execute()
		{
			// TODO: A scoped listener cleanup would be nice
			try
			{
				Log::EnsureListener().SetShowEventId(true);

				// Enable log event ids to track individual builds
				auto& packageGraph = _packageProvider.GetRootPackageGraph();
				auto& packageInfo = _packageProvider.GetPackageInfo(packageGraph.RootPackageId);
				BuildPackageAndDependencies(packageGraph, packageInfo);

				Log::EnsureListener().SetShowEventId(false);
			}
			catch(...)
			{
				Log::EnsureListener().SetShowEventId(false);
				throw;
			}
		}

	private:
		/// <summary>
		/// Build the dependencies for the provided recipe recursively
		/// </summary>
		void BuildPackageAndDependencies(
			const PackageGraph& packageGraph,
			const PackageInfo& packageInfo)
		{
			if (packageInfo.IsPrebuilt)
			{
				if (_buildCache.contains(packageInfo.Id))
				{
					Log::Diag("Prebuilt Package was already processed");
				}
				else
				{
					// Get the target directory
					const auto& targetDirectory = _packageProvider.GetTargetDirectory(
						packageGraph.Id,
						packageInfo.Id);

					// Cache the build state for upstream dependencies
					Log::Diag("Package was prebuilt: {}", packageInfo.Name.ToString());
					_buildCache.emplace(
						packageInfo.Id,
						RecipeBuildCacheState(
							packageInfo.Name.ToString(),
							Path(std::format("/(TARGET_{})/", packageInfo.Name.ToString())),
							targetDirectory,
							targetDirectory + Path("./.soup/"),
							{},
							{}));
				}
			}
			else
			{
				for (auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
				{
					for (auto& dependency : dependencyTypeSet)
					{
						if (dependency.IsSubGraph)
						{
							// Load this package recipe
							auto& dependencyPackageGraph = _packageProvider.GetPackageGraph(dependency.PackageGraphId);
							auto& dependencyPackageInfo = _packageProvider.GetPackageInfo(dependencyPackageGraph.RootPackageId);

							// Build all recursive dependencies
							BuildPackageAndDependencies(dependencyPackageGraph, dependencyPackageInfo);
						}
						else
						{
							// Load this package recipe
							auto& dependencyPackageInfo = _packageProvider.GetPackageInfo(dependency.PackageId);

							// Build all recursive dependencies
							BuildPackageAndDependencies(packageGraph, dependencyPackageInfo);
						}
					}
				}

				// Get the target directory
				const auto& targetDirectory = _packageProvider.GetTargetDirectory(packageGraph.Id, packageInfo.Id);

				// Build the target recipe
				CheckBuildPackage(packageGraph.GlobalParameters, packageInfo, targetDirectory);
			}
		}

		/// <summary>
		/// The core build that will either invoke the recipe builder directly
		/// or load a previous state
		/// </summary>
		void CheckBuildPackage(
			const ValueTable& globalParameters,
			const PackageInfo& packageInfo,
			const Path& targetDirectory)
		{
			// TODO: RAII for active id
			try
			{
				Log::SetActiveId(packageInfo.Id);
				Log::Diag("Running Build: [{}]{}", packageInfo.Recipe->GetLanguage().GetName(), packageInfo.Name.ToString());

				// Check if we already built this package down a different dependency path
				if (_buildCache.contains(packageInfo.Id))
				{
					Log::Diag("Recipe already built: [{}]{}", packageInfo.Recipe->GetLanguage().GetName(), packageInfo.Name.ToString());
				}
				else
				{
					// Run the required builds in process
					RunBuild(globalParameters, packageInfo, targetDirectory);
				}

				Log::SetActiveId(0);
			}
			catch(...)
			{
				Log::SetActiveId(0);
				throw;
			}
		}

		/// <summary>
		/// Setup and run the individual components of the Generate and Evaluate phases for a given package
		/// </summary>
		void RunBuild(
			const ValueTable& globalParameters,
			const PackageInfo& packageInfo,
			const Path& targetDirectory)
		{
			Log::Info("Build '{}'", packageInfo.Name.ToString());

			// Build up the expected output directory for the build to be used to cache state
			auto macroPackageDirectory = Path(
				std::format("/(PACKAGE_{})/", packageInfo.Name.ToString()));
			auto macroTargetDirectory = Path(
				std::format("/(TARGET_{})/", packageInfo.Name.ToString()));
			auto soupTargetDirectory = targetDirectory + BuildConstants::SoupTargetDirectory();

			// Build up the set of directories and macros that grant access to the generate/evaluate phases
			auto packageAccessSet = GenerateAccessSet(
				packageInfo,
				macroPackageDirectory,
				macroTargetDirectory,
				targetDirectory);

			// Preload target
			// TODO: Ideally this should be done in the preload step, but easier here with the graph id
			_fileSystemState.PreloadDirectory(targetDirectory, false);

			//////////////////////////////////////////////
			// GENERATE
			/////////////////////////////////////////////
			auto evaluateGraph = OperationGraph();
			auto evaluateResults = OperationResults();
			auto evaluateResultsFile = Path();
			if (!_arguments.SkipGenerate)
			{
				std::tie(evaluateGraph, evaluateResults, evaluateResultsFile) = RunGenerate(
					packageInfo,
					macroPackageDirectory,
					macroTargetDirectory,
					targetDirectory,
					soupTargetDirectory,
					globalParameters,
					packageAccessSet);
			}

			//////////////////////////////////////////////
			// EVALUATE
			/////////////////////////////////////////////
			if (!_arguments.SkipEvaluate)
			{
				RunEvaluate(
					evaluateGraph,
					evaluateResults,
					evaluateResultsFile,
					targetDirectory);
			}

			// Cache the build state for upstream dependencies
			_buildCache.emplace(
				packageInfo.Id,
				RecipeBuildCacheState(
					packageInfo.Name.ToString(),
					std::move(macroTargetDirectory),
					targetDirectory,
					std::move(soupTargetDirectory),
					std::move(packageAccessSet.EvaluateRecursiveReadDirectories),
					std::move(packageAccessSet.EvaluateRecursiveMacros)));
		}

		/// <summary>
		/// Setup and run the individual components of the Generate phase
		/// </summary>
		std::tuple<OperationGraph, OperationResults, Path> RunGenerate(
			const PackageInfo& packageInfo,
			const Path& macroPackageDirectory,
			const Path& macroTargetDirectory,
			const Path& realTargetDirectory,
			const Path& soupTargetDirectory,
			const ValueTable& globalParameters,
			const DependencyTargetSet& packageAccessSet)
		{
			auto generatePhase1Result = GenerateResult();
			auto generatePhase2Result = OperationGraph();
			auto evaluatePhase1Results = OperationResults();
			auto evaluatePhase2Results = OperationResults();

			auto generatePhase1ResultFile = soupTargetDirectory + BuildConstants::GeneratePhase1ResultFileName();
			auto generatePhase2ResultFile = soupTargetDirectory + BuildConstants::GeneratePhase2ResultFileName();
			auto evaluatePhase1ResultsFile = soupTargetDirectory + BuildConstants::EvaluatePhase1ResultsFileName();
			auto evaluatePhase2ResultsFile = soupTargetDirectory + BuildConstants::EvaluatePhase2ResultsFileName();

			//////////////////////////////////////////////
			// Setup Generate
			/////////////////////////////////////////////
			Log::Info("Checking for existing Generate Phase 1 Result");
			Log::Diag(generatePhase1ResultFile.ToString());
			auto hasExistingResult = GenerateResultManager::TryLoadState(
				generatePhase1ResultFile,
				generatePhase1Result,
				_fileSystemState);

			if (hasExistingResult)
			{
				Log::Info("Phase1 previous graph found");

				Log::Info("Checking for existing Evaluate Operation Results");
				Log::Diag(evaluatePhase1ResultsFile.ToString());
				if (OperationResultsManager::TryLoadState(
					evaluatePhase1ResultsFile,
					evaluatePhase1Results,
					_fileSystemState))
				{
					Log::Info("Phase1 previous results found");
				}
				else
				{
					Log::Info("Phase1 no previous results found");
				}

				if (generatePhase1Result.IsPreprocessor())
				{
					// Load the previous operation graph and result if they exist
					Log::Info("Checking for existing Generate Phase 2 Result");

					Log::Diag(generatePhase2ResultFile.ToString());
					auto hasExistingGraph = OperationGraphManager::TryLoadState(
						generatePhase2ResultFile,
						generatePhase2Result,
						_fileSystemState);
					if (hasExistingGraph)
					{
						Log::Info("Phase2 previous graph found");

						Log::Info("Checking for existing Evaluate Operation Results");
						Log::Diag(evaluatePhase2ResultsFile.ToString());
						if (OperationResultsManager::TryLoadState(
							evaluatePhase2ResultsFile,
							evaluatePhase2Results,
							_fileSystemState))
						{
							Log::Info("Phase2 previous results found");
						}
						else
						{
							Log::Info("Phase2 no previous results found");
						}
					}
					else
					{
						Log::Info("Phase2 no previous graph");
					}
				}
			}
			else
			{
				Log::Info("Phase1 no previous graph");
			}

			//////////////////////////////////////////////
			// Run Generate Phase 1
			/////////////////////////////////////////////

			// Ensure the target directories exists
			if (!System::IFileSystem::Current().Exists(soupTargetDirectory))
			{
				Log::Info("Create Directory: {}", soupTargetDirectory.ToString());
				System::IFileSystem::Current().CreateDirectory(soupTargetDirectory);
			}

			auto ranGeneratePhase1 = RunGenerateCore(
				true,
				packageInfo,
				macroPackageDirectory,
				macroTargetDirectory,
				realTargetDirectory,
				soupTargetDirectory,
				globalParameters,
				packageAccessSet);

			//////////////////////////////////////////////
			// Load Updated Result
			/////////////////////////////////////////////
			if (ranGeneratePhase1)
			{
				Log::Info("Loading updated Generate Phase 1 Result");
				Log::Diag(generatePhase1ResultFile.ToString());
				auto updatedGeneratePhase1Result = GenerateResult();
				if (!GenerateResultManager::TryLoadState(
					generatePhase1ResultFile,
					updatedGeneratePhase1Result,
					_fileSystemState))
				{
					throw std::runtime_error("Missing required generate phase 1 result.");
				}

				Log::Diag("Map previous operation graph observed results");
				auto updatedEvaluatePhase1Results = MergeOperationResults(
					generatePhase1Result.GetGraph(),
					evaluatePhase1Results,
					updatedGeneratePhase1Result.GetGraph());

				// Replace the previous operation graph and results
				generatePhase1Result = std::move(updatedGeneratePhase1Result);
				evaluatePhase1Results = std::move(updatedEvaluatePhase1Results);
			}

			//////////////////////////////////////////////
			// Evaluate Optional Preprocessor
			/////////////////////////////////////////////
			if (generatePhase1Result.IsPreprocessor())
			{
				auto ranPreprocessors = RunPreprocessorOperations(
					generatePhase1Result.GetGraph(),
					evaluatePhase1Results,
					realTargetDirectory,
					soupTargetDirectory);

				// TODO: Only run gen if needed?
				#ifdef _WIN32
				(ranPreprocessors);
				#endif

				auto ranGeneratePhase2 = RunGenerateCore(
					false,
					packageInfo,
					macroPackageDirectory,
					macroTargetDirectory,
					realTargetDirectory,
					soupTargetDirectory,
					globalParameters,
					packageAccessSet);

				// TODO : Do I need this check
				if (ranGeneratePhase2)
				{
					// Load the update operation graph and result if they exist
					Log::Info("Load update Generate Phase 2 Result");
					auto updatedGeneratePhase2Result = OperationGraph();
					if (!OperationGraphManager::TryLoadState(
						generatePhase2ResultFile,
						updatedGeneratePhase2Result,
						_fileSystemState))
					{
						throw std::runtime_error("Missing required generate phase 2 result.");
					}
					

					Log::Diag("Map previous operation graph observed results");
					auto updatedEvaluatePhase2Results = MergeOperationResults(
						generatePhase2Result,
						evaluatePhase2Results,
						updatedGeneratePhase2Result);

					// Replace the previous operation graph and results
					generatePhase2Result = std::move(updatedGeneratePhase2Result);
					evaluatePhase2Results = std::move(updatedEvaluatePhase2Results);
				}

				return std::make_tuple(
					std::move(generatePhase2Result),
					std::move(evaluatePhase2Results),
					std::move(evaluatePhase2ResultsFile));
			}
			else
			{
				return std::make_tuple(
					std::move(generatePhase1Result.GetGraph()),
					std::move(evaluatePhase1Results),
					std::move(evaluatePhase1ResultsFile));
			}
		}

		/// <summary>
		/// Run the core generate phase
		/// </summary>
		bool RunGenerateCore(
			bool isFirstRun,
			const PackageInfo& packageInfo,
			const Path& macroPackageDirectory,
			const Path& macroTargetDirectory,
			const Path& realTargetDirectory,
			const Path& soupTargetDirectory,
			const ValueTable& globalParameters,
			const DependencyTargetSet& packageAccessSet)
		{
			// Clone the global parameters
			auto inputTable = ValueTable();

			// Pass along internal dependency information
			inputTable.emplace("Dependencies", GenerateInputDependenciesValueTable(packageInfo));

			// Setup input that will be included in the global state
			auto globalState = ValueTable();

			// Setup environment context generated by build runner
			auto context = ValueTable();
			context.emplace("PackageDirectory", macroPackageDirectory.ToString());
			context.emplace("TargetDirectory", macroTargetDirectory.ToString());
			context.emplace("HostPlatform", _arguments.HostPlatform);
			globalState.emplace("Context", std::move(context));

			// Pass along the parameters
			globalState.emplace("Parameters", globalParameters);

			// Generate the dependencies input state
			globalState.emplace("Dependencies", GenerateParametersDependenciesValueTable(packageInfo));

			// Pass along the file system state
			auto fileSystemRoot = BuildDirectoryStructure(packageInfo.PackageRoot);
			globalState.emplace("FileSystem", std::move(fileSystemRoot));

			inputTable.emplace("GlobalState", std::move(globalState));

			// Build up the input state for the generate call
			auto generateMacros = ValueTable();
			auto generateSubGraphMacros = ValueTable();
			auto evaluateAllowedReadAccess = ValueList();
			auto evaluateAllowedWriteAccess = ValueList();
			auto evaluateMacros = ValueTable();

			// Allow generate to resolve generate macros
			for (auto& [key, value] : packageAccessSet.GenerateCurrentMacros)
				generateMacros.emplace(key, value);

			// Make subgraph macros unique
			for (auto& [key, value] : packageAccessSet.GenerateSubGraphMacros)
				generateSubGraphMacros.emplace(key, value);

			// Pass along the read access set
			for (auto& value : packageAccessSet.EvaluateCurrentReadDirectories)
				evaluateAllowedReadAccess.push_back(value.ToString());
			for (auto& value : packageAccessSet.EvaluateRecursiveReadDirectories)
				evaluateAllowedReadAccess.push_back(value.ToString());

			// Pass along the write access set
			for (auto& value : packageAccessSet.EvaluateCurrentWriteDirectories)
				evaluateAllowedWriteAccess.push_back(value.ToString());

			// Allow generate to resolve evaluate macros
			for (auto& [key, value] : packageAccessSet.EvaluateCurrentMacros)
				evaluateMacros.emplace(key, value);
			for (auto& [key, value] : packageAccessSet.EvaluateRecursiveMacros)
				evaluateMacros.emplace(key, value);

			inputTable.emplace("PackageRoot", packageInfo.PackageRoot.ToString());
			inputTable.emplace("UserDataPath", _userDataPath.ToString());
			inputTable.emplace("GenerateMacros", std::move(generateMacros));
			inputTable.emplace("GenerateSubGraphMacros", std::move(generateSubGraphMacros));
			inputTable.emplace("EvaluateReadAccess", std::move(evaluateAllowedReadAccess));
			inputTable.emplace("EvaluateWriteAccess", std::move(evaluateAllowedWriteAccess));
			inputTable.emplace("EvaluateMacros", std::move(evaluateMacros));

			auto inputFile = soupTargetDirectory + BuildConstants::GenerateInputFileName();
			Log::Info("Check outdated generate input file: {}", inputFile.ToString());
			if (IsOutdated(inputTable, inputFile))
			{
				Log::Info("Save Generate Input file");
				ValueTableManager::SaveState(inputFile, inputTable);
			}

			// Run the incremental generate
			auto generateGraph = OperationGraph();

			// Add the single root operation to perform the generate
			auto moduleName = System::IProcessManager::Current().GetCurrentProcessFileName();
			auto generateFolder = moduleName.GetParent();

			#if defined(_WIN32)
			auto generateExecutable = generateFolder + Path("./Soup.Generate.exe");
			#elif defined(__linux__)
			auto generateExecutable = generateFolder + Path("./generate");
			#else
			#error "Unknown platform"
			#endif

			OperationId generateOperationId = 1;
			auto generateArguments = std::vector<std::string>();
			generateArguments.push_back(isFirstRun ? "true" : "false");
			generateArguments.push_back(soupTargetDirectory.ToString());
			auto generateOperation = OperationInfo(
				generateOperationId,
				std::format("Generate Core: [{}]{}", packageInfo.Recipe->GetLanguage().GetName(), packageInfo.Name.ToString()),
				CommandInfo(
					packageInfo.PackageRoot,
					generateExecutable,
					std::move(generateArguments)),
				{},
				{},
				{},
				{});
			generateOperation.DependencyCount = 1;
			generateGraph.AddOperation(std::move(generateOperation));

			// Set the Generate operation as the root
			generateGraph.SetRootOperationIds({
				generateOperationId,
			});

			// Set Read and Write access fore the generate phase
			auto generateAllowedReadAccess = std::vector<Path>();
			auto generateAllowedWriteAccess = std::vector<Path>();

			// Allow read access to the generate executable folder
			generateAllowedReadAccess.push_back(generateFolder);

			// Allow read access to the local user config
			auto localUserConfigPath = _userDataPath + BuildConstants::LocalUserConfigFileName();
			generateAllowedReadAccess.push_back(std::move(localUserConfigPath));

			// TODO: Windows specific
			generateAllowedReadAccess.push_back(Path("C:/Windows/"));
			generateAllowedReadAccess.push_back(Path("C:/Program Files/dotnet/"));

			// Pass along the read access set
			for (auto& value : packageAccessSet.GenerateCurrentReadDirectories)
				generateAllowedReadAccess.push_back(value);

			// Pass along the write access set
			for (auto& value : packageAccessSet.GenerateCurrentWriteDirectories)
				generateAllowedWriteAccess.push_back(value);

			// Load the previous build results if it exists
			auto generateResultFile = isFirstRun ?
				soupTargetDirectory + BuildConstants::GeneratePhase1OperationResultFileName()
				: soupTargetDirectory + BuildConstants::GeneratePhase2OperationResultFileName();
			Log::Info("Checking for existing Generate Operation Results");
			Log::Diag(generateResultFile.ToString());
			auto generateResults = OperationResults();
			if (OperationResultsManager::TryLoadState(
				generateResultFile,
				generateResults,
				_fileSystemState))
			{
				Log::Info("Previous results found");
			}
			else
			{
				Log::Info("No previous results found");
			}

			// Set the temporary folder under the target folder
			auto temporaryDirectory = realTargetDirectory + BuildConstants::TemporaryFolderName();

			// Evaluate the Generate phase
			return RunIncrementalEvaluate(
				generateGraph,
				generateResults,
				generateResultFile,
				temporaryDirectory,
				generateAllowedReadAccess,
				generateAllowedWriteAccess);
		}

		bool RunPreprocessorOperations(
			const OperationGraph& operationGraph,
			OperationResults& operationResults,
			const Path& realTargetDirectory,
			const Path& soupTargetDirectory)
		{
			// Set the temporary folder under the target folder
			auto temporaryDirectory = realTargetDirectory + BuildConstants::TemporaryFolderName();

			// Initialize the read access with the shared global set
			auto allowedReadAccess = std::vector<Path>();
			auto allowedWriteAccess = std::vector<Path>();

			// Allow read access from system runtime directories
			std::copy(
				_systemReadAccess.begin(),
				_systemReadAccess.end(),
				std::back_inserter(allowedReadAccess));

			// Allow read and write access to the temporary directory that is not explicitly declared
			allowedReadAccess.push_back(temporaryDirectory);
			allowedWriteAccess.push_back(temporaryDirectory);

			// Ensure the temporary directories exists
			if (!System::IFileSystem::Current().Exists(temporaryDirectory))
			{
				Log::Info("Create Directory: {}", temporaryDirectory.ToString());
				System::IFileSystem::Current().CreateDirectory(temporaryDirectory);
			}

			// Evaluate the build
			auto evaluatePhase1ResultsFile = soupTargetDirectory + BuildConstants::EvaluatePhase1ResultsFileName();
			return RunIncrementalEvaluate(
				operationGraph,
				operationResults,
				evaluatePhase1ResultsFile,
				temporaryDirectory,
				allowedReadAccess,
				allowedWriteAccess);
		}

		void RunEvaluate(
			const OperationGraph& operationGraph,
			OperationResults& operationResults,
			const Path& operationResultsFile,
			const Path& realTargetDirectory)
		{
			// Set the temporary folder under the target folder
			auto temporaryDirectory = realTargetDirectory + BuildConstants::TemporaryFolderName();

			// Initialize the read access with the shared global set
			auto allowedReadAccess = std::vector<Path>();
			auto allowedWriteAccess = std::vector<Path>();

			// Allow read access from system runtime directories
			std::copy(
				_systemReadAccess.begin(),
				_systemReadAccess.end(),
				std::back_inserter(allowedReadAccess));

			// Allow read and write access to the temporary directory that is not explicitly declared
			allowedReadAccess.push_back(temporaryDirectory);
			allowedWriteAccess.push_back(temporaryDirectory);

			// Ensure the temporary directories exists
			if (!System::IFileSystem::Current().Exists(temporaryDirectory))
			{
				Log::Info("Create Directory: {}", temporaryDirectory.ToString());
				System::IFileSystem::Current().CreateDirectory(temporaryDirectory);
			}

			// Evaluate the build
			auto ranEvaluate = RunIncrementalEvaluate(
				operationGraph,
				operationResults,
				operationResultsFile,
				temporaryDirectory,
				allowedReadAccess,
				allowedWriteAccess);

			if (ranEvaluate)
			{
				Log::Info("Done!");
			}
			else
			{
				Log::Info("Nothing to do.");
			}
		}

		/// <summary>
		/// Run incremental evaluation
		/// </summary>
		bool RunIncrementalEvaluate(
			const OperationGraph& operationGraph,
			OperationResults& operationResults,
			const Path& resultsFile,
			const Path& temporaryDirectory,
			const std::vector<Path>& allowedReadAccess,
			const std::vector<Path>& allowedWriteAccess)
		{
			try
			{
				// Evaluate the outdated operations
				bool ranEvaluate = _evaluateEngine.Evaluate(
					operationGraph,
					operationResults,
					temporaryDirectory,
					allowedReadAccess,
					allowedWriteAccess);

				if (ranEvaluate)
				{
					// Save the generate operation results for future incremental builds
					Log::Info("Save operation results");
					OperationResultsManager::SaveState(resultsFile, operationResults, _fileSystemState);
				}

				return ranEvaluate;
			}
			catch(const BuildFailedException&)
			{
				Log::Info("Saving partial build state");
				OperationResultsManager::SaveState(resultsFile, operationResults, _fileSystemState);
				throw;
			}
		}

		OperationResults MergeOperationResults(
			const OperationGraph& previousGraph,
			OperationResults& previousResults,
			const OperationGraph& updatedGraph)
		{
			auto updatedResults = OperationResults();
			for (auto& [operationId, updatedOperation] : updatedGraph.GetOperations())
			{
				// Check if the new operation command existing in the previous set too
				OperationId previousOperationId;
				if (previousGraph.TryFindOperation(updatedOperation.Command, previousOperationId))
				{
					// Check if there is an existing result for the previous operation
					OperationResult* previousOperationResult;
					if (previousResults.TryFindResult(previousOperationId, previousOperationResult))
					{
						// Move this result into the updated results store
						updatedResults.AddOrUpdateOperationResult(updatedOperation.Id, std::move(*previousOperationResult));
					}
				}
			}

			return updatedResults;
		}

		bool IsOutdated(const ValueTable& parametersTable, const Path& parametersFile)
		{
			// Load up the existing parameters file and check if our state matches the previous
			// to ensure incremental builds function correctly
			auto previousParametersState = ValueTable();
			if (ValueTableManager::TryLoadState(parametersFile, previousParametersState))
			{
				return previousParametersState != parametersTable;
			}
			else
			{
				return true;
			}
		}

		ValueTable GenerateInputDependenciesValueTable(
			const PackageInfo& packageInfo)
		{
			auto result = ValueTable();

			for (const auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
			{
				auto dependencyTypeTable = ValueTable();
				for (auto& dependency : dependencyTypeSet)
				{
					// Load this package recipe
					auto dependencyPackageId = dependency.PackageId;
					if (dependency.IsSubGraph)
					{
						auto& dependencyPackageGraph = _packageProvider.GetPackageGraph(dependency.PackageGraphId);
						dependencyPackageId = dependencyPackageGraph.RootPackageId;
					}

					auto& dependencyPackageInfo = _packageProvider.GetPackageInfo(dependencyPackageId);

					// Grab the build state for upstream dependencies
					auto findBuildCache = _buildCache.find(dependencyPackageInfo.Id);
					if (findBuildCache != _buildCache.end())
					{
						auto& dependencyState = findBuildCache->second;
						dependencyTypeTable.emplace(
							dependencyState.Name,
							Value(ValueTable({
								{
									"SoupTargetDirectory",
									Value(dependencyState.SoupTargetDirectory.ToString())
								},
							})));
					}
					else
					{
						Log::Error("Dependency does not exist in build cache: {}", dependencyPackageInfo.PackageRoot.ToString());
						throw std::runtime_error(
							std::format(
								"Dependency does not exist in build cache: {}",
								dependencyPackageInfo.PackageRoot.ToString()));
					}
				}

				result.emplace(dependencyType, Value(std::move(dependencyTypeTable)));
			}

			return result;
		}

		ValueTable GenerateParametersDependenciesValueTable(
			const PackageInfo& packageInfo)
		{
			auto result = ValueTable();

			for (const auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
			{
				auto dependencyTypeTable = ValueTable();
				for (auto& dependency : dependencyTypeSet)
				{
					// Load this package recipe
					auto dependencyPackageId = dependency.PackageId;
					if (dependency.IsSubGraph)
					{
						auto& dependencyPackageGraph = _packageProvider.GetPackageGraph(dependency.PackageGraphId);
						dependencyPackageId = dependencyPackageGraph.RootPackageId;
					}

					auto& dependencyPackageInfo = _packageProvider.GetPackageInfo(dependencyPackageId);

					// Grab the build state for upstream dependencies
					auto findBuildCache = _buildCache.find(dependencyPackageInfo.Id);
					if (findBuildCache != _buildCache.end())
					{
						auto& dependencyState = findBuildCache->second;
						auto contextTable = ValueTable({
							{
								"Reference",
								Value(dependency.OriginalReference.ToString())
							},
							{
								"TargetDirectory",
								Value(dependencyState.MacroTargetDirectory.ToString())
							},
						});

						dependencyTypeTable.emplace(
							dependencyState.Name,
							Value(ValueTable({
								{ "Context", std::move(contextTable) },
							})));
					}
					else
					{
						Log::Error("Dependency does not exist in build cache: {}", dependencyPackageInfo.PackageRoot.ToString());
						throw std::runtime_error(
							std::format(
								"Dependency does not exist in build cache: {}",
								dependencyPackageInfo.PackageRoot.ToString()));
					}
				}

				result.emplace(dependencyType, Value(std::move(dependencyTypeTable)));
			}

			return result;
		}

		DependencyTargetSet GenerateAccessSet(
			const PackageInfo& packageInfo,
			const Path& macroPackageDirectory,
			const Path& macroTargetDirectory,
			const Path& realTargetDirectory)
		{
			auto targetSet = DependencyTargetSet();

			// Allow reading from the package root (Recipe) during generate
			targetSet.GenerateCurrentReadDirectories.insert(packageInfo.PackageRoot);

			// Allow reading and writing to the package target (Input/Output) during generate
			targetSet.GenerateCurrentReadDirectories.insert(realTargetDirectory);
			targetSet.GenerateCurrentWriteDirectories.insert(realTargetDirectory);

			// Only this build has read access to the package source directory during evaluate
			targetSet.EvaluateCurrentReadDirectories.insert(macroPackageDirectory);
			targetSet.EvaluateCurrentMacros.emplace(
				macroPackageDirectory.ToString(),
				packageInfo.PackageRoot.ToString());

			// This and all recursive packages will have read access to the target directory
			// and the current package has write permissions
			targetSet.EvaluateRecursiveReadDirectories.insert(macroTargetDirectory);
			targetSet.EvaluateCurrentWriteDirectories.insert(macroTargetDirectory);
			targetSet.EvaluateRecursiveMacros.emplace(
				macroTargetDirectory.ToString(),
				realTargetDirectory.ToString());

			for (auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
			{
				for (auto& dependency : dependencyTypeSet)
				{
					// Load this package recipe
					auto dependencyPackageId = dependency.PackageId;
					if (dependency.IsSubGraph)
					{
						auto& dependencyPackageGraph = _packageProvider.GetPackageGraph(dependency.PackageGraphId);
						dependencyPackageId = dependencyPackageGraph.RootPackageId;
					}

					auto& dependencyPackageInfo = _packageProvider.GetPackageInfo(dependencyPackageId);

					// Grab the build state for upstream dependencies
					auto findBuildCache = _buildCache.find(dependencyPackageInfo.Id);
					if (findBuildCache != _buildCache.end())
					{
						// Combine the child dependency target and the recursive children
						auto& dependencyState = findBuildCache->second;

						if (dependency.IsSubGraph)
						{
							// Check known types for subgraphs
							if (dependencyType == _dependencyTypeBuild)
							{
								// Replace with unique macro to prevent collisions
								auto macroBuildTargetDirectory = Path(
									std::format("/(BUILD_TARGET_{})/", dependencyPackageInfo.Name.ToString()));
								targetSet.GenerateSubGraphMacros.emplace(
									dependencyState.MacroTargetDirectory.ToString(),
									macroBuildTargetDirectory.ToString());

								// Allow read access for all direct build dependencies target directories
								// and macros during generate. This is needed to load the shared properties.
								targetSet.GenerateCurrentReadDirectories.insert(dependencyState.RealTargetDirectory);
								targetSet.GenerateCurrentMacros.emplace(
									macroBuildTargetDirectory.ToString(),
									dependencyState.RealTargetDirectory.ToString());
							}
							else if (dependencyType == _dependencyTypeTool)
							{
								// Replace with unique macro to prevent collisions
								auto macroToolTargetDirectory = Path(
									std::format("/(TOOL_TARGET_{})/", dependencyPackageInfo.Name.ToString()));
								targetSet.GenerateSubGraphMacros.emplace(
									dependencyState.MacroTargetDirectory.ToString(),
									macroToolTargetDirectory.ToString());

								// Allow read access for all indirect tool dependencies target directories
								// and macros during generate and evaluate. This is needed to load the shared properties and run them.
								targetSet.GenerateCurrentReadDirectories.insert(dependencyState.RealTargetDirectory);
								targetSet.EvaluateCurrentReadDirectories.insert(dependencyState.RealTargetDirectory);
								targetSet.EvaluateCurrentMacros.emplace(
									macroToolTargetDirectory.ToString(),
									dependencyState.RealTargetDirectory.ToString());
							}
							else
							{
								throw std::runtime_error(
									std::format("Invalid subgraph dependency type: {}", dependencyType));
							}
						}
						else
						{
							// Within the same graph share all read directories and macros to upstream dependencies
							targetSet.EvaluateRecursiveReadDirectories.insert(
								dependencyState.RecursiveChildMacroTargetDirectorySet.begin(),
								dependencyState.RecursiveChildMacroTargetDirectorySet.end());
							targetSet.EvaluateRecursiveMacros.insert(
								dependencyState.RecursiveChildMacros.begin(),
								dependencyState.RecursiveChildMacros.end());

							// Allow read access for all direct runtime dependencies target directories
							// and macros during generate. This is needed to load the shared properties.
							targetSet.GenerateCurrentReadDirectories.insert(dependencyState.RealTargetDirectory);
							targetSet.GenerateCurrentMacros.emplace(
								dependencyState.MacroTargetDirectory.ToString(),
								dependencyState.RealTargetDirectory.ToString());
						}
					}
					else
					{
						Log::Error("Dependency does not exist in build cache: {}", dependencyPackageInfo.PackageRoot.ToString());
						throw std::runtime_error(
							std::format("Dependency does not exist in build cache: {}", dependencyPackageInfo.PackageRoot.ToString()));
					}
				}
			}

			return targetSet;
		}

		ValueList BuildDirectoryStructure(const Path& directory)
		{
			auto directoryState = _fileSystemState.GetDirectoryState(directory);

			auto result = ValueList();
			BuildDirectoryStructure(directoryState, result);

			return result;
		}

		void BuildDirectoryStructure(DirectoryState& activeDirectory, ValueList& result)
		{
			for (auto& file : activeDirectory.Files)
			{
				result.push_back(file);
			}

			if (!activeDirectory.ChildDirectories.empty())
			{
				auto childDirectories = ValueTable();
				for (auto& childDirectory : activeDirectory.ChildDirectories)
				{
					auto childDirectoryStructure = ValueList();
					BuildDirectoryStructure(childDirectory.second, childDirectoryStructure);
					childDirectories.emplace(
						childDirectory.first,
						std::move(childDirectoryStructure));
				}

				result.push_back(std::move(childDirectories));
			}
		}
	};
}
