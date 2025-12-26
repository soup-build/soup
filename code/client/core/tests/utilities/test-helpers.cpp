// <copyright file="test-helpers.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <string>
#include <unordered_map>
#include <vector>

export module Soup.Core:TestHelpers;

import :FileSystemState;

import Opal;

using namespace Opal;

namespace Soup::Core::UnitTests
{
	export class TestHelpers
	{
	public:
		static std::unordered_map<std::string, DirectoryState, string_hash, std::equal_to<>> BuildDirectoryLookup(
			std::vector<Path> paths)
		{
			auto result = std::unordered_map<std::string, DirectoryState, string_hash, std::equal_to<>>();

			for (auto file : paths)
			{
				auto activeDirectory = EnsureDirectoryExists(result, file.GetRoot());

				const auto directories = file.DecomposeDirectories();
				for (auto directory : directories)
				{
					activeDirectory = EnsureDirectoryExists(activeDirectory->ChildDirectories, directory);
				}

				if (file.HasFileName())
				{
					activeDirectory->Files.insert(
						std::string(file.GetFileName()));
				}
			}

			return result;
		}

		static DirectoryState* EnsureDirectoryExists(
			std::unordered_map<std::string, DirectoryState, string_hash, std::equal_to<>>& activeDirectory,
			const std::string_view name)
		{
			auto findResult = activeDirectory.find(name);
			if (findResult != activeDirectory.end())
			{
				return &findResult->second;
			}
			else
			{
				auto insertResult = activeDirectory.emplace(name, DirectoryState());
				return &insertResult.first->second;
			}
		}
	};
}