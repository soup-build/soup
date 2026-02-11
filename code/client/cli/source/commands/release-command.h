// <copyright file="release-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "release-options.h"

namespace Soup::Client
{
	/// <summary>
	/// Release Command
	/// </summary>
	class ReleaseCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ReleaseCommand"/> class.
		/// </summary>
		ReleaseCommand(ReleaseOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("ReleaseCommand::Run");

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

			Build(workingDirectory);

			auto targetDirectory = GetTargetDirectory(workingDirectory);

			Core::PackageManager::PublishArtifact(workingDirectory, targetDirectory);
		}

	private:
		void Build(const Path& workingDirectory)
		{
			auto startTime = std::chrono::high_resolution_clock::now();

			// Setup the build arguments
			auto arguments = Core::RecipeBuildArguments();
			arguments.WorkingDirectory = std::move(workingDirectory);
			arguments.ForceRebuild = false;
			arguments.SkipGenerate = false;
			arguments.SkipEvaluate = false;
			arguments.DisableMonitor = false;
			arguments.PartialMonitor = false;

			// Platform specific defaults
			#if defined(_WIN32)
				arguments.HostPlatform = "Windows";
			#elif defined(__linux__)
				arguments.HostPlatform = "Linux";
			#else
				#error "Unknown Platform"
			#endif

			// Process well known parameters
			if (!_options.Flavor.empty())
				arguments.GlobalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			if (!_options.Architecture.empty())
				arguments.GlobalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			// TODO: Generic parameters

			// Now build the current project
			Log::HighPriority("Build:");

			// Find the built in folder root
			auto processFilename = System::IProcessManager::Current().GetCurrentProcessFileName();
			auto processDirectory = processFilename.GetParent();

			// Load user config state
			auto userDataPath = Core::BuildEngine::GetSoupUserDataPath();
			
			auto recipeCache = Core::RecipeCache();

			auto packageProvider = Core::BuildEngine::LoadBuildGraph(
				arguments.WorkingDirectory,
				_options.Owner,
				arguments.GlobalParameters,
				userDataPath,
				recipeCache);

			Core::BuildEngine::Execute(
				packageProvider,
				std::move(arguments),
				userDataPath,
				recipeCache);

			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

			std::ostringstream durationMessage;
			if (duration.count() >= 60)
			{
				durationMessage << std::fixed << std::setprecision(2);
				durationMessage << duration.count() / 60  << " minutes";
			}
			else if (duration.count() >= 10)
			{
				durationMessage << std::fixed << std::setprecision(0);
				durationMessage << duration.count() << " seconds";
			}
			else
			{
				durationMessage << std::fixed << std::setprecision(3);
				durationMessage << duration.count() << " seconds";
			}

			Log::HighPriority(durationMessage.str());
		}

		Path GetTargetDirectory(const Path& workingDirectory)
		{
			// Load the recipe
			auto recipeCache = Core::RecipeCache();
			auto recipePath =
				workingDirectory +
				Core::BuildConstants::RecipeFileName();
			const Core::Recipe* recipe;
			if (!recipeCache.TryGetOrLoadRecipe(recipePath, recipe))
			{
				Log::Error("The Recipe does not exist: {}", recipePath.ToString());
				Log::HighPriority("Make sure the path is correct and try again");

				// Nothing we can do, exit
				throw Core::HandledException(1234);
			}

			// Build up the unique name
			auto packageName = Core::PackageName(std::nullopt, recipe->GetName());

			// Setup the build parameters
			auto globalParameters = Core::ValueTable();

			// Process well known parameters
			if (!_options.Flavor.empty())
				globalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			if (!_options.Architecture.empty())
				globalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			// TODO: Generic parameters

			// Load the value table to get the exe path
			auto knownLanguages = Core::BuildEngine::GetKnownLanguages();
			auto locationManager = Core::RecipeBuildLocationManager(knownLanguages);
			auto targetDirectory = locationManager.GetOutputDirectory(
				packageName,
				workingDirectory,
				*recipe,
				globalParameters,
				recipeCache);

			return targetDirectory;
		}

	private:
		ReleaseOptions _options;
	};
}
