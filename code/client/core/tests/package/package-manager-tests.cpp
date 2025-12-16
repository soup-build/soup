// <copyright file="package-manager-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:PackageManagerTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class PackageManagerTests
	{
	public:
		// [[Fact]]
		void RestorePackages()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto workingDirectory = Path("C:/TestLocation");
			PackageManager::RestorePackages(workingDirectory);

			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: RestorePackages",
					"INFO: Running PackageManager",
					#ifdef _WIN32
						"DIAG:   C:/testlocation/PackageManager/soup.build.package-manager.exe restore-packages C:/TestLocation",
					#else
						"DIAG:   C:/lib/package-manager/package-manager restore-packages C:/TestLocation",
					#endif
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					#ifdef _WIN32
						"CreateProcess: 1 0 [C:/testlocation/PackageManager/] C:/testlocation/PackageManager/soup.build.package-manager.exe restore-packages C:/TestLocation",
					#else
						"CreateProcess: 1 0 [C:/lib/package-manager/] C:/lib/package-manager/package-manager restore-packages C:/TestLocation",
					#endif
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetExitCode: 1",
				}),
				processManager->GetRequests(),
				"Verify process requests match expected.");
		}

		// [[Fact]]
		void InitializePackage()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto workingDirectory = Path("C:/TestLocation");
			PackageManager::InitializePackage(workingDirectory);

			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: InitializePackage",
					"INFO: Running PackageManager",
					#ifdef _WIN32
						"DIAG:   C:/testlocation/PackageManager/soup.build.package-manager.exe initialize-package C:/TestLocation",
					#else
						"DIAG:   C:/lib/package-manager/package-manager initialize-package C:/TestLocation",
					#endif
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					#ifdef _WIN32
						"CreateProcess: 1 0 [C:/testlocation/PackageManager/] C:/testlocation/PackageManager/soup.build.package-manager.exe initialize-package C:/TestLocation",
					#else
						"CreateProcess: 1 0 [C:/lib/package-manager/] C:/lib/package-manager/package-manager initialize-package C:/TestLocation",
					#endif
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetExitCode: 1",
				}),
				processManager->GetRequests(),
				"Verify process requests match expected.");
		}

		// [[Fact]]
		void InstallPackageReference_Version()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto workingDirectory = Path("C:/TestLocation");
			auto packageName = "TheirPackage@2.2.2";
			PackageManager::InstallPackageReference(workingDirectory, packageName);

			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: InstallPackageReference",
					"INFO: Running PackageManager",
					#ifdef _WIN32
						"DIAG:   C:/testlocation/PackageManager/soup.build.package-manager.exe install-package C:/TestLocation TheirPackage@2.2.2",
					#else
						"DIAG:   C:/lib/package-manager/package-manager install-package C:/TestLocation TheirPackage@2.2.2",
					#endif
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					#ifdef _WIN32
						"CreateProcess: 1 0 [C:/testlocation/PackageManager/] C:/testlocation/PackageManager/soup.build.package-manager.exe install-package C:/TestLocation TheirPackage@2.2.2",
					#else
						"CreateProcess: 1 0 [C:/lib/package-manager/] C:/lib/package-manager/package-manager install-package C:/TestLocation TheirPackage@2.2.2",
					#endif
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetExitCode: 1",
				}),
				processManager->GetRequests(),
				"Verify process requests match expected.");
		}

		// [[Fact]]
		void InstallPackageReference_NoVersion()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto workingDirectory = Path("C:/TestLocation");
			auto packageName = "TheirPackage";
			PackageManager::InstallPackageReference(workingDirectory, packageName);

			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: InstallPackageReference",
					"INFO: Running PackageManager",
					#ifdef _WIN32
						"DIAG:   C:/testlocation/PackageManager/soup.build.package-manager.exe install-package C:/TestLocation TheirPackage",
					#else
						"DIAG:   C:/lib/package-manager/package-manager install-package C:/TestLocation TheirPackage",
					#endif
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					#ifdef _WIN32
						"CreateProcess: 1 0 [C:/testlocation/PackageManager/] C:/testlocation/PackageManager/soup.build.package-manager.exe install-package C:/TestLocation TheirPackage",
					#else
						"CreateProcess: 1 0 [C:/lib/package-manager/] C:/lib/package-manager/package-manager install-package C:/TestLocation TheirPackage",
					#endif
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetExitCode: 1",
				}),
				processManager->GetRequests(),
				"Verify process requests match expected.");
		}

		// [[Fact]]
		void PublishPackage()
		{
			// Register the test listener
			auto testListener = std::make_shared<TestTraceListener>();
			auto scopedTraceListener = ScopedTraceListenerRegister(testListener);

			// Register the test process manager
			auto processManager = std::make_shared<MockProcessManager>();
			auto scopedProcessManager = ScopedProcessManagerRegister(processManager);

			auto workingDirectory = Path("C:/TestLocation");
			PackageManager::PublishPackage(workingDirectory);

			Assert::AreEqual(
				std::vector<std::string>({
					"INFO: PublishPackage",
					"INFO: Running PackageManager",
					#ifdef _WIN32
						"DIAG:   C:/testlocation/PackageManager/soup.build.package-manager.exe publish-package C:/TestLocation",
					#else
						"DIAG:   C:/lib/package-manager/package-manager publish-package C:/TestLocation",
					#endif
				}),
				testListener->GetMessages(),
				"Verify log messages match expected.");

			Assert::AreEqual(
				std::vector<std::string>({
					"GetCurrentProcessFileName",
					#ifdef _WIN32
						"CreateProcess: 1 0 [C:/testlocation/PackageManager/] C:/testlocation/PackageManager/soup.build.package-manager.exe publish-package C:/TestLocation",
					#else
						"CreateProcess: 1 0 [C:/lib/package-manager/] C:/lib/package-manager/package-manager publish-package C:/TestLocation",
					#endif
					"ProcessStart: 1",
					"WaitForExit: 1",
					"GetExitCode: 1",
				}),
				processManager->GetRequests(),
				"Verify process requests match expected.");
		}
	};
}
