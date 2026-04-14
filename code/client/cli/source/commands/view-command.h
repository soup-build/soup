// <copyright file="view-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "view-options.h"

namespace Soup::Client
{
	/// <summary>
	/// View Command
	/// </summary>
	class ViewCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ViewCommand"/> class.
		/// </summary>
		ViewCommand(ViewOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("ViewsCommand::Run");

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

			// Platform specific defaults
			#if defined(_WIN32)
				auto hostPlatform = "Windows";
			#elif defined(__linux__)
				auto hostPlatform = "Linux";
			#else
				#error "Unknown Platform"
			#endif

			// Load user config state
			auto userDataPath = Core::Build::Constants::GetSoupUserDataPath();
			
			auto recipeCache = Core::RecipeCache();

			// Setup the build parameters
			auto globalParameters = Core::ValueTable();

			// Process well known parameters
			if (!_options.Flavor.empty())
				globalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			if (!_options.Architecture.empty())
				globalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			auto packageProvider = Core::Build::LoadBuildGraph(
				workingDirectory,
				_options.Owner,
				globalParameters,
				userDataPath,
				hostPlatform,
				recipeCache);
			
			View::TUI::Run(packageProvider);
		}

	private:
		ViewOptions _options;
	};
}
