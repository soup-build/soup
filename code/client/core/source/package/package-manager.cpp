// <copyright file="package-manager.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <sstream>
#include <string>
#include <vector>

export module Soup.Core:PackageManager;

import Opal;
import :HandledException;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The package manager wrapper that sends requests along to the managed implementation
	/// </summary>
	export class PackageManager
	{
	public:
		/// <summary>
		/// Restore packages
		/// </summary>
		static void RestorePackages(const Path& workingDirectory)
		{
			Log::Info("RestorePackages");

			auto arguments = std::vector<std::string>(
			{
				"restore-packages",
				workingDirectory.ToString(),
			});

			RunCommand(std::move(arguments));
		}

		/// <summary>
		/// Initialize a package
		/// </summary>
		static void InitializePackage(const Path& workingDirectory)
		{
			Log::Info("InitializePackage");

			auto arguments = std::vector<std::string>(
			{
				"initialize-package",
				workingDirectory.ToString(),
			});

			RunCommand(std::move(arguments));
		}

		/// <summary>
		/// Install a package
		/// </summary>
		static void InstallPackageReference(const Path& workingDirectory, const std::string& packageReference)
		{
			Log::Info("InstallPackageReference");

			auto arguments = std::vector<std::string>(
			{
				"install-package",
				workingDirectory.ToString(),
				packageReference,
			});

			RunCommand(std::move(arguments));
		}

		/// <summary>
		/// Publish a package
		/// </summary>
		static void PublishPackage(const Path& workingDirectory)
		{
			Log::Info("PublishPackage");

			auto arguments = std::vector<std::string>(
			{
				"publish-package",
				workingDirectory.ToString(),
			});

			RunCommand(std::move(arguments));
		}

		/// <summary>
		/// Publish a package artifact
		/// </summary>
		static void PublishArtifact(
			const Path& workingDirectory,
			const Path& targetDirectory)
		{
			Log::Info("PublishArtifact");

			auto arguments = std::vector<std::string>(
			{
				"publish-artifact",
				workingDirectory.ToString(),
				targetDirectory.ToString(),
			});

			RunCommand(std::move(arguments));
		}

	private:
		static void RunCommand(std::vector<std::string> arguments)
		{
			auto moduleName = System::IProcessManager::Current().GetCurrentProcessFileName();
			auto moduleFolder = moduleName.GetParent();
			#if defined(_WIN32)
			auto packageManagerFolder = moduleFolder + Path("./package-manager/");
			auto executable = packageManagerFolder + Path("./package-manager.exe");
			#elif defined(__linux__)
			auto packageManagerFolder = moduleFolder + Path("../lib/soup/package-manager/");
			auto executable = packageManagerFolder + Path("./package-manager");
			#else
			#error "Unknown platform"
			#endif

			// Log diagnostic information
			std::stringstream message;
			message << "  " << executable.ToString();
			for (auto& argument : arguments)
				message << " " << argument;

			Log::Info("Running PackageManager");
			Log::Diag(message.str());

			// Execute the requested target
			auto process = System::IProcessManager::Current().CreateProcess(
				executable,
				std::move(arguments),
				packageManagerFolder,
				false);
			process->Start();
			process->WaitForExit();

			auto exitCode = process->GetExitCode();

			if (exitCode != 0)
			{
				Log::Error("Package Manager Failed");
				throw HandledException(exitCode);
			}
		}
	};
}
