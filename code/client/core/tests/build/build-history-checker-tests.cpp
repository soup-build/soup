// <copyright file="build-history-checker-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

export module Soup.Core.Tests:BuildHistoryCheckerTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Soup::Core::UnitTests
{
	export class BuildHistoryCheckerTests
	{
	public:
		// [[Fact]]
		void IsOutdated_ZeroInput()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				2,
				std::unordered_map<FileId, Path>({}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_UnknownInputFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				3,
				std::unordered_map<FileId, Path>({
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
				}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: File Missing [C:/Root/Input.cpp]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({
					"TryGetLastWriteTime: C:/Root/Input.cpp",
				}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_DeletedInputFile()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test file system
			auto fileSystem = std::make_shared<MockFileSystem>();
			auto scopedFileSystem = ScopedFileSystemRegister(fileSystem);

			// Create the file state
			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				3,
				std::unordered_map<FileId, Path>({
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
					{ 2, std::nullopt },
				}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: File Missing [C:/Root/Input.cpp]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			// Verify expected file system requests
			Assert::AreEqual(
				std::vector<std::string>({}),
				fileSystem->GetRequests(),
				"Verify file system requests match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_Outdated()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto inputTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 13min);

			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				3,
				std::unordered_map<FileId, Path>({
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
					{ 2, inputTime },
				}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);

			// Verify the results
			Assert::IsTrue(result, "Verify the result is true.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: File altered after last evaluate [C:/Root/Input.cpp]",
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_SingleInput_UpToDate()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto inputTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 11min);

			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				3,
				std::unordered_map<FileId, Path>({
					{ 2, Path("C:/Root/Input.cpp") },
				}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
					{ 2, inputTime },
				}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({
				2,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}

		// [[Fact]]
		void IsOutdated_MultipleInputs_RelativeAndAbsolute()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Create the file state
			auto inputTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 11min);

			// Initialize the file system state
			auto fileSystemState = FileSystemState(
				4,
				std::unordered_map<FileId, Path>({
					{ 2, Path("C:/Root/Input.cpp") },
					{ 3, Path("C:/Input.h") },
				}),
				{},
				std::unordered_map<FileId, std::optional<std::chrono::time_point<std::chrono::file_clock>>>({
					{ 2, inputTime },
					{ 3, inputTime },
				}));

			// Setup the input parameters
			auto evaluateTime = std::chrono::clock_cast<std::chrono::file_clock>(
				std::chrono::sys_days(May/22/2015) + 9h + 12min);
			auto inputFiles = std::vector<FileId>({
				2,
				3,
			});

			// Perform the check
			auto uut = BuildHistoryChecker(fileSystemState);
			bool result = uut.IsOutdated(evaluateTime, inputFiles);

			// Verify the results
			Assert::IsFalse(result, "Verify the result is false.");

			// Verify expected logs
			Assert::AreEqual(
				std::vector<std::string>({}),
				testListener->GetMessages(),
				"Verify log messages match expected.");
		}
	};
}
