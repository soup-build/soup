// <copyright file="run-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "run-options.h"

namespace Soup::Client
{
	/// <summary>
	/// Run Command
	/// </summary>
	class RunCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="RunCommand"/> class.
		/// </summary>
		RunCommand(RunOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("RunCommand::Run");

			auto workingDirectory = Path();
			if (_options.Path.empty())
			{
				// Build in the current directory
				workingDirectory = System::IFileSystem::Current().GetCurrentDirectory();
			}
			else
			{
				// Parse the path in any system valid format
				workingDirectory = Path::Parse(std::format("{}/", _options.Path));

				// Check if this is relative to current directory
				if (!workingDirectory.HasRoot())
				{
					workingDirectory = System::IFileSystem::Current().GetCurrentDirectory() + workingDirectory;
				}
			}

			auto recipeCache = Core::RecipeCache();

			// Setup the build parameters
			auto globalParameters = Core::ValueTable();

			// Process well known parameters
			if (!_options.Flavor.empty())
				globalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			if (!_options.Architecture.empty())
				globalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			Build(workingDirectory, recipeCache, globalParameters);
			Run(workingDirectory, recipeCache, globalParameters);
		}

	private:
		void Build(
			const Path& workingDirectory,
			Core::RecipeCache& recipeCache,
			const Core::ValueTable& globalParameters)
		{
			auto systemReadAccess = std::vector<Path>();

			// Platform specific defaults
			#if defined(_WIN32)
				auto hostPlatform = "Windows";

				// Allow read access from system directories
				systemReadAccess.push_back(
					Path("C:/Windows/"));
			#elif defined(__linux__)
				auto hostPlatform = "Linux";
			#else
				#error "Unknown Platform"
			#endif

			// Setup the build arguments
			auto arguments = Core::RecipeBuildArguments();
			arguments.Parallelization = Core::Build::Constants::GetDefaultParallelization();
			arguments.WorkingDirectory = workingDirectory;
			arguments.ForceRebuild = false;
			arguments.SkipGenerate = false;
			arguments.SkipEvaluate = false;
			arguments.DisableMonitor = false;
			arguments.PartialMonitor = false;
			arguments.HostPlatform = hostPlatform;
			arguments.GlobalParameters = globalParameters;

			// TODO: Generic parameters

			// Now build the current project
			Log::Info("Begin Build:");

			// Find the built in folder root
			auto processFilename = System::IProcessManager::Current().GetCurrentProcessFileName();
			auto processDirectory = processFilename.GetParent();

			// Load user config state
			auto userDataPath = Core::Build::Constants::GetSoupUserDataPath();

			auto packageProvider = Core::Build::LoadBuildGraph(
				arguments.WorkingDirectory,
				std::nullopt,
				arguments.GlobalParameters,
				userDataPath,
				hostPlatform,
				recipeCache);

			Core::Build::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				systemReadAccess,
				recipeCache);

			Log::Info("End Build:");
		}

		void Run(
			const Path& workingDirectory,
			Core::RecipeCache& recipeCache,
			const Core::ValueTable& globalParameters)
		{
			// Load the recipe
			auto recipePath =
				workingDirectory +
				Core::Build::Constants::RecipeFileName();
			const Core::Recipe* recipe;
			if (!recipeCache.TryGetOrLoadRecipe(recipePath, recipe))
			{
				Log::Error("The Recipe does not exist: {}", recipePath.ToString());
				Log::HighPriority("Make sure the path is correct and try again");

				// Nothing we can do, exit
				throw Core::HandledException(1234);
			}

			// Build up the unique name
			auto packageName = PackageName(std::nullopt, recipe->GetName());

			// TODO: Generic parameters

			// Load the value table to get the exe path
			auto knownLanguages = Core::Build::GetKnownLanguages();
			auto locationManager = Core::RecipeBuildLocationManager(knownLanguages);
			auto targetDirectory = locationManager.GetOutputDirectory(
				packageName,
				workingDirectory,
				*recipe,
				globalParameters,
				recipeCache);
			auto soupTargetDirectory = targetDirectory + Core::Build::Constants::SoupTargetDirectory();

			// Load the shared state file
			auto generateInputFile = soupTargetDirectory + Core::Build::Constants::GenerateInputFileName();
			auto generateInputTable = Core::ValueTable();
			if (!Core::ValueTableManager::TryLoadState(generateInputFile, generateInputTable))
			{
				Log::Error("Failed to load the generate input file: {}", generateInputFile.ToString());
				return;
			}

			// Load the input macro definition
			auto macros = std::map<std::string, std::string>();
			for (auto& [key, value] : generateInputTable.at("EvaluateMacros").AsTable())
				macros.emplace(key, value.AsString());

			// Setup a macro manager to resolve macros
			auto macroManager = Core::MacroManager( macros);

			// Load the shared state file
			auto sharedStateFile = soupTargetDirectory + Core::Build::Constants::GenerateSharedStateFileName();
			auto sharedStateTable = Core::ValueTable();
			if (!Core::ValueTableManager::TryLoadState(sharedStateFile, sharedStateTable))
			{
				Log::Error("Failed to load the shared state file: {}", sharedStateFile.ToString());
				return;
			}

			// Get the executable from the build target file property
			// Check for any dynamic libraries in the shared state
			if (!sharedStateTable.contains("Build"))
			{
				Log::Error("Shared state does not have a build table");
				return;
			}

			auto& buildTable = sharedStateTable.at("Build").AsTable();
			if (!buildTable.contains("RunExecutable"))
			{
				Log::Error("Build table does not have a RunExecutable property");
				return;
			}

			if (!buildTable.contains("RunArguments"))
			{
				Log::Error("Build table does not have a RunArguments property");
				return;
			}

			auto runExecutable = Path(macroManager.ResolveMacros(buildTable.at("RunExecutable").AsString()));
			Log::Info("Executable: {}", runExecutable.ToString());
			if (!System::IFileSystem::Current().Exists(runExecutable))
			{
				Log::Error("The run executable does not exist");
				return;
			}

			auto arguments = std::vector<std::string>();
			auto runArguments = buildTable.at("RunArguments").AsList();
			for (auto& value : runArguments)
			{
				arguments.push_back(macroManager.ResolveMacros(value.AsString()));
			}

			for (auto& argument : _options.Arguments)
			{
				arguments.push_back(argument);
			}

			// Execute the requested target
			Log::Info("CreateProcess");
			auto runDirectory = Path();
			auto process = System::IProcessManager::Current().CreateProcess(
				runExecutable,
				std::move(arguments),
				runDirectory,
				false);
			process->Start();
			process->WaitForExit();

			auto exitCode = process->GetExitCode();

			if (exitCode != 0)
			{
				Log::Diag("Process exit nonzero " + std::to_string(exitCode));
				throw Core::HandledException(exitCode);
			}
		}

	private:
		RunOptions _options;
	};
}
