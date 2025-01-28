// <copyright file="LocalUserConfigExtensions.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <memory>

export module Soup.Core:LocalUserConfigExtensions;

import Opal;
import :LocalUserConfig;
import :RecipeSML;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The local user config extensions
	/// </summary>
	export class LocalUserConfigExtensions
	{
	public:
		/// <summary>
		/// Attempt to load from file
		/// </summary>
		static bool TryLoadLocalUserConfigFromFile(
			const Path& localUserConfigFile,
			LocalUserConfig& result)
		{
			// Open the file to read from
			Log::Diag("Load Local User Config: {}", localUserConfigFile.ToString());
			std::shared_ptr<System::IInputFile> file;
			if (!System::IFileSystem::Current().TryOpenRead(localUserConfigFile, true, file))
			{
				Log::Warning("Local User Config file does not exist");
				return false;
			}

			// Read the contents of the local user config file
			try
			{
				result = LocalUserConfig(
					RecipeSML::Deserialize(
						localUserConfigFile,
						file->GetInStream()));
				return true;
			}
			catch (std::exception& ex)
			{
				Log::Error("Deserialize Threw: {}", ex.what());
				Log::Info("Failed to parse local user config.");
				return false;
			}
		}
	};
}
