﻿// <copyright file="InitializeCommand.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "ICommand.h"
#include "InitializeOptions.h"

namespace Soup::Client
{
	/// <summary>
	/// Initialize Command
	/// </summary>
	class InitializeCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="InitializeCommand"/> class.
		/// </summary>
		InitializeCommand(InitializeOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("InitializeCommand::Run");
			Log::HighPriority("The initialize utility will walk through the creation of the most basic Console recipe.\n");

			// Use the current directory as the default names
			auto workingDirectory = System::IFileSystem::Current().GetCurrentDirectory2();
			auto recipePath = 
				workingDirectory +
				Core::BuildConstants::RecipeFileName();

			// Todo: Opal path should have a way to get individual directories
			auto workingFolderValue = workingDirectory.ToString();
			workingFolderValue.pop_back(); // Remove directory separator
			auto workingFolder = Path(workingFolderValue);

			auto recipe = Core::Recipe(
				workingFolder.GetFileName(),
				Core::LanguageReference("C++", SemanticVersion(0, 1, 0)),
				SemanticVersion(1, 0, 0),
				std::nullopt,
				std::nullopt,
				std::nullopt);

			recipe.SetRootValue("Type", "Executable");
			recipe.SetRootValue("Source", std::vector<std::string>({ "Main.cpp", }));

			UpdateDefaultValues(recipe);

			// Save the state of the recipe if it has changed
			Core::RecipeExtensions::SaveToFile(recipePath, recipe);

			// Save a simple main method
			auto mainFileContent =
R"(#include <iostream>
int main()
{
	std::cout << "Hello World" << std::endl;
	return 0;
})";

			auto mainFilePath = Path("Main.cpp");
			auto mainFile = System::IFileSystem::Current().OpenWrite(mainFilePath, false);
			mainFile->GetOutStream() << mainFileContent;
			mainFile->Close();
		}

	private:
		void UpdateDefaultValues(Core::Recipe& recipe)
		{
			Log::HighPriority("Name: (" + recipe.GetName() + ")");
			auto newName = std::string();
			std::getline(std::cin, newName);
			if (!newName.empty())
			{
				recipe.SetName(newName);
			}

			bool setVersion = false;
			while (!setVersion)
			{
				Log::HighPriority("Version: (" + recipe.GetVersion().ToString() + ")");
				auto newVersion = std::string();
				std::getline(std::cin, newVersion);
				if (newVersion.empty())
				{
					// Use the default
					setVersion = true;
				}
				else
				{
					auto value = SemanticVersion();
					if (SemanticVersion::TryParse(newVersion, value))
					{
						recipe.SetVersion(value);
						setVersion = true;
					}
					else
					{
						Log::Warning("Invalid version: \"" + newVersion + "\"");
					}
				}
			}
		}

	private:
		InitializeOptions _options;
	};
}
