// <copyright file="BuildRunner.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

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
		RecipeBuildLocationManager& _locationManager;

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
			FileSystemState& fileSystemState,
			RecipeBuildLocationManager& locationManager) :
			_arguments(arguments),
			_userDataPath(std::move(userDataPath)),
			_systemReadAccess(systemReadAccess),
			_recipeCache(recipeCache),
			_packageProvider(packageProvider),
			_evaluateEngine(evaluateEngine),
			_fileSystemState(fileSystemState),
			_locationManager(locationManager),
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
		void BuildPackageAndDependencies(const PackageGraph& packageGraph, const PackageInfo& packageInfo)
		{
			if (packageInfo.IsPrebuilt)
			{
				if (_buildCache.contains(packageInfo.Id))
				{
					Log::Diag("Prebuilt Package was already processed");
				}
				else
				{
					// Cache the build state for upstream dependencies
					Log::Diag("Package was prebuilt: {}", packageInfo.Name.ToString());
					_buildCache.emplace(
						packageInfo.Id,
						RecipeBuildCacheState(
							packageInfo.Name.ToString(),
							Path(std::format("/(TARGET_{})/", packageInfo.Name.ToString())),
							packageInfo.TargetDirectory,
							packageInfo.TargetDirectory + Path("./.soup/"),
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

				// Build the target recipe
				CheckBuildPackage(packageGraph, packageInfo);
			}
		}

		/// <summary>
		/// The core build that will either invoke the recipe builder directly
		/// or load a previous state
		/// </summary>
		void CheckBuildPackage(const PackageGraph& packageGraph, const PackageInfo& packageInfo)
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
					RunBuild(packageGraph, packageInfo);
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
		void RunBuild(const PackageGraph& packageGraph, const PackageInfo& packageInfo)
		{
			Log::Info("Build '{}'", packageInfo.Name.ToString());

			// Build up the expected output directory for the build to be used to cache state
			auto macroPackageDirectory = Path(
				std::format("/(PACKAGE_{})/", packageInfo.Name.ToString()));
			auto macroTargetDirectory = Path(
				std::format("/(TARGET_{})/", packageInfo.Name.ToString()));
			auto realTargetDirectory = _locationManager.GetOutputDirectory(
				packageInfo.Name,
				packageInfo.PackageRoot,
				*packageInfo.Recipe,
				packageGraph.GlobalParameters,
				_recipeCache);
			auto soupTargetDirectory = realTargetDirectory + BuildConstants::SoupTargetDirectory();

			// Build up the set of directories and macros that grant access to the generate/evaluate phases
			auto packageAccessSet = GenerateAccessSet(
				packageInfo,
				macroPackageDirectory,
				macroTargetDirectory,
				realTargetDirectory);

			// Preload target
			// TODO: Ideally this should be done in the preload step, but easier here with the graph id
			_fileSystemState.PreloadDirectory(realTargetDirectory, false);

			//////////////////////////////////////////////
			// GENERATE
			/////////////////////////////////////////////
			auto previouseEvaluateGraph = OperationGraph();
			auto updatedEvaluateGraph = OperationGraph();
			if (!_arguments.SkipGenerate)
			{
				std::tie(previouseEvaluateGraph, updatedEvaluateGraph) = RunGenerate(
					packageInfo,
					macroPackageDirectory,
					macroTargetDirectory,
					realTargetDirectory,
					soupTargetDirectory,
					packageGraph.GlobalParameters,
					packageAccessSet);
			}

			//////////////////////////////////////////////
			// EVALUATE
			/////////////////////////////////////////////
			if (!_arguments.SkipEvaluate)
			{
				RunEvaluate(
					previouseEvaluateGraph,
					updatedEvaluateGraph,
					realTargetDirectory,
					soupTargetDirectory);
			}

			// Cache the build state for upstream dependencies
			_buildCache.emplace(
				packageInfo.Id,
				RecipeBuildCacheState(
					packageInfo.Name.ToString(),
					std::move(macroTargetDirectory),
					std::move(realTargetDirectory),
					std::move(soupTargetDirectory),
					std::move(packageAccessSet.EvaluateRecursiveReadDirectories),
					std::move(packageAccessSet.EvaluateRecursiveMacros)));
		}

		/// <summary>
		/// Setup and run the individual components of the Generate phase
		/// </summary>
		std::tuple<OperationGraph, OperationGraph> RunGenerate(
			const PackageInfo& packageInfo,
			const Path& macroPackageDirectory,
			const Path& macroTargetDirectory,
			const Path& realTargetDirectory,
			const Path& soupTargetDirectory,
			const ValueTable& globalParameters,
			const DependencyTargetSet& packageAccessSet)
		{
			//////////////////////////////////////////////
			// Load Previous Run
			/////////////////////////////////////////////
			auto generateResultFile = soupTargetDirectory + BuildConstants::GenerateResultFileName();
			Log::Info("Checking for existing Generate Result");
			Log::Diag(generateResultFile.ToString());
			auto previousGenerateResult = GenerateResult();
			auto hasExistingResult = GenerateResultManager::TryLoadState(
				generateResultFile,
				previousGenerateResult,
				_fileSystemState);

			auto previousEvaluateGraph = OperationGraph();
			if (hasExistingResult)
			{
				Log::Info("Previous generate result found");

				if (previousGenerateResult.IsPreprocessor())
				{
					// Load the previous operation graph and results if they exist
					auto evaluateGraphFile = soupTargetDirectory + BuildConstants::EvaluateGraphFileName();
					Log::Info("Checking for existing Evaluate Operation Graph");
					Log::Diag(evaluateGraphFile.ToString());
					auto hasExistingGraph = OperationGraphManager::TryLoadState(
						evaluateGraphFile,
						previousEvaluateGraph,
						_fileSystemState);
					if (hasExistingGraph)
					{
						Log::Info("Previous operation graph found");
					}
					else
					{
						Log::Info("No previous operation graph found");
					}
				}
			}
			else
			{
				Log::Info("No previous generate result found");
			}

			//////////////////////////////////////////////
			// Generate Core
			/////////////////////////////////////////////

			// Ensure the target directories exists
			if (!System::IFileSystem::Current().Exists(soupTargetDirectory))
			{
				Log::Info("Create Directory: {}", soupTargetDirectory.ToString());
				System::IFileSystem::Current().CreateDirectory(soupTargetDirectory);
			}

			auto ranGenerateCore = RunGenerateCore(
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
			auto updatedGenerateResult = GenerateResult();
			if (ranGenerateCore)
			{
				Log::Info("Loading new Generate Result");
				if (!GenerateResultManager::TryLoadState(
					generateResultFile,
					updatedGenerateResult,
					_fileSystemState))
				{
					throw std::runtime_error("Missing required generate result after generate evaluated.");
				}
			}

			//////////////////////////////////////////////
			// Evaluate Preprocessor
			/////////////////////////////////////////////
			if (updatedGenerateResult.IsPreprocessor())
			{
				auto ranPreprocessors = RunPreprocessorOperations(
					previousGenerateResult.GetEvaluateGraph(),
					updatedGenerateResult.GetEvaluateGraph(),
					realTargetDirectory,
					soupTargetDirectory);

				auto ranGenerateCorePhase2 = RunGenerateCore(
					false,
					packageInfo,
					macroPackageDirectory,
					macroTargetDirectory,
					realTargetDirectory,
					soupTargetDirectory,
					globalParameters,
					packageAccessSet);

				// TODO : Do I need this check
				if (ranPreprocessors || ranGenerateCorePhase2)
				{
					// Load the update operation graph and results if they exist
					auto evaluateGraphFile = soupTargetDirectory + BuildConstants::EvaluateGraphFileName();
					Log::Info("Load Evaluate Operation Graph");
					if (!OperationGraphManager::TryLoadState(
						evaluateGraphFile,
						previousEvaluateGraph,
						_fileSystemState))
					{
						throw std::runtime_error("Missing required evaluate operation graph after proxies evaluated.");
					}
				}

				// TODO: Get phase 2 results
				return std::make_tuple(
					previousGenerateResult.GetEvaluateGraph(),
					updatedGenerateResult.GetEvaluateGraph());
			}
			else
			{
				return std::make_tuple(
					previousGenerateResult.GetEvaluateGraph(),
					updatedGenerateResult.GetEvaluateGraph());
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

			// Set the temporary folder under the target folder
			auto temporaryDirectory = realTargetDirectory + BuildConstants::TemporaryFolderName();

			// Evaluate the Generate phase
			// TODO: For now it is easy to run generate with same graph, but this is extra work to merge a known same graph... Not much, but ¯\_(ツ)_/¯
			auto generateResultsFile = soupTargetDirectory + BuildConstants::GenerateResultsFileName();
			return RunIncrementalEvaluate(
				generateGraph,
				generateGraph,
				generateResultsFile,
				temporaryDirectory,
				generateAllowedReadAccess,
				generateAllowedWriteAccess);
		}

		bool RunPreprocessorOperations(
			const OperationGraph& previousGraph,
			const OperationGraph& updatedGraph,
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
			auto proxiesResultsFile = soupTargetDirectory + BuildConstants::ProxiesResultsFileName();
			return RunIncrementalEvaluate(
				previousGraph,
				updatedGraph,
				proxiesResultsFile,
				temporaryDirectory,
				allowedReadAccess,
				allowedWriteAccess);
		}

		/// <summary>
		/// Run the finalizer generate phase
		/// </summary>
		bool RunGenerateFinalizer( 
			const PackageInfo& packageInfo,
			const Path& realTargetDirectory,
			const Path& soupTargetDirectory,
			const DependencyTargetSet& packageAccessSet)
		{
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
			generateArguments.push_back("Finalizer");
			generateArguments.push_back(soupTargetDirectory.ToString());
			auto generateOperation = OperationInfo(
				generateOperationId,
				std::format("Generate Finalizer: [{}]{}", packageInfo.Recipe->GetLanguage().GetName(), packageInfo.Name.ToString()),
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

			// Set the temporary folder under the target folder
			auto temporaryDirectory = realTargetDirectory + BuildConstants::TemporaryFolderName();

			// Evaluate the Generate phase
			// TODO: For now it is easy to run generate with same graph, but this is extra work to merge a known same graph... Not much, but ¯\_(ツ)_/¯
			auto generateResultsFile = soupTargetDirectory + BuildConstants::GenerateResultsFileName();
			return RunIncrementalEvaluate(
				generateGraph,
				generateGraph,
				generateResultsFile,
				temporaryDirectory,
				generateAllowedReadAccess,
				generateAllowedWriteAccess);
		}

		void RunEvaluate(
			const OperationGraph& previousGraph,
			const OperationGraph& updatedGraph,
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
			auto evaluateResultsFile = soupTargetDirectory + BuildConstants::EvaluateResultsFileName();
			auto ranEvaluate = RunIncrementalEvaluate(
				previousGraph,
				updatedGraph,
				evaluateResultsFile,
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
		/// Run an generate phase
		/// </summary>
		bool RunIncrementalEvaluate(
			const OperationGraph& previousGraph,
			const OperationGraph& updatedGraph,
			const Path& resultsFile,
			const Path& temporaryDirectory,
			const std::vector<Path>& allowedReadAccess,
			const std::vector<Path>& allowedWriteAccess)
		{
			// Load the previous build results if it exists
			Log::Info("Checking for existing Operation Results");
			Log::Diag(resultsFile.ToString());
			auto operationResults = OperationResults();
			if (OperationResultsManager::TryLoadState(
				resultsFile,
				operationResults,
				_fileSystemState))
			{
				Log::Diag("Map previous operation graph observed results");
				auto updatedResults = MergeOperationResults(
					previousGraph,
					operationResults,
					updatedGraph);

				operationResults = std::move(updatedResults);
			}
			else
			{
				Log::Info("No previous results found");
			}

			try
			{
				// Evaluate the outdated operations
				bool ranEvaluate = _evaluateEngine.Evaluate(
					updatedGraph,
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
