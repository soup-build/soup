// <copyright file="build-history-checker.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <vector>

export module Soup.Core:BuildHistoryChecker;

import Opal;
import :FileSystemState;

using namespace Opal;

namespace Soup::Core
{
	export class BuildHistoryChecker
	{
	private:
		FileSystemState& _fileSystemState;

	public:
		BuildHistoryChecker(FileSystemState& fileSystemState) :
			_fileSystemState(fileSystemState)
		{
		}

		/// <summary>
		/// Perform a check if the last evaluate time is outdated with
		/// respect to the input files
		/// </summary>
		bool IsOutdated(
			std::chrono::time_point<std::chrono::file_clock> lastEvaluateTime,
			FileId inputFile)
		{
			auto lastWriteTime = _fileSystemState.GetLastWriteTime(inputFile);
			const auto& targetFilePath = _fileSystemState.GetFilePath(inputFile);

			// Perform the final check
			if (!lastWriteTime.has_value())
			{
				// The input was missing
				Log::Info("File Missing [{}]", targetFilePath.ToString());
				return true;
			}
			else if (targetFilePath.HasFileName())
			{
				if (lastWriteTime.value() > lastEvaluateTime)
				{
					Log::Info("File altered after last evaluate [{}]", targetFilePath.ToString());
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Only check existence for directories
				return false;
			}
		}

		/// <summary>
		/// Perform a check if the requested inputs are outdated with
		/// respect to the last build time
		/// </summary>
		bool IsOutdated(
			std::chrono::time_point<std::chrono::file_clock> lastEvaluateTime,
			const std::vector<FileId>& inputFiles)
		{
			for (auto& file : inputFiles)
			{
				if (IsOutdated(lastEvaluateTime, file))
				{
					return true;
				}
			}

			return false;
		}
	};
}
