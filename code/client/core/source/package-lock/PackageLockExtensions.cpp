// <copyright file="PackageLockExtensions.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <memory>

export module Soup.Core:PackageLockExtensions;

import Opal;
import :PackageLock;
import :RecipeSML;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The package lock extensions
	/// </summary>
	export class PackageLockExtensions
	{
	public:
		/// <summary>
		/// Attempt to load from file
		/// </summary>
		static bool TryLoadFromFile(
			const Path& packageLockFile,
			PackageLock& result)
		{
			// Open the file to read from
			Log::Diag("Load PackageLock: {}", packageLockFile.ToString());
			std::shared_ptr<System::IInputFile> file;
			if (!System::IFileSystem::Current().TryOpenRead(packageLockFile, true, file))
			{
				Log::Info("PackageLock file does not exist");
				return false;
			}

			// Read the contents of the recipe file
			try
			{
				result = PackageLock(
					RecipeSML::Deserialize(
						packageLockFile,
						file->GetInStream()));
				return true;
			}
			catch (std::exception& ex)
			{
				Log::Error("Deserialize Threw: {}", ex.what());
				Log::Info("Failed to parse PackageLock.");
				return false;
			}
		}
	};
}