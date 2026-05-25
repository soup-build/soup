module;

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>
#include <set>
#include <string>
#include <vector>

export module Soup.BuildDatabase;

import :PackageLoadState;

import json11;
import Opal;
import Soup.Core;

using namespace Opal;
using namespace Soup::Core;

std::string GetModuleNameFromFile(const Path &file) {
	auto result = std::string(file.GetFileStem());
	std::replace(result.begin(), result.end(), '-', ':');
	return result;
}

json11::Json GeneratePackageBuildSet(FileSystemState &fileSystemState,
									 PackageProvider &packageProvider,
									 int graphId, int packageId) {
	auto operationGraph =
		LoadPackage(fileSystemState, packageProvider, graphId, packageId);

	auto translationUnits = json11::Json::array();
	if (operationGraph.has_value()) {
		for (auto &[operationId, operation] :
			 operationGraph.value().GetOperations()) {

			// Hack: Only look at clang operations for now
			if (operation.Command.Executable.GetFileName().starts_with(
					"clang")) {
				auto arguments = json11::Json::array();
				arguments.push_back(operation.Command.Executable.ToString());
				for (auto &argument : operation.Command.Arguments) {
					arguments.push_back(argument);
				}

				auto requiredModules = json11::Json::array();
				auto sourceFile = std::string();
				for (auto &inputFile : operation.DeclaredInput) {
					auto file = fileSystemState.GetFilePath(inputFile);
					if (file.GetFileExtension() == ".pcm") {
						requiredModules.push_back(GetModuleNameFromFile(file));
					} else if (file.GetFileExtension() == ".cpp") {
						sourceFile = file.ToString();
					}
				}

				auto producedModules = json11::Json::object();
				auto objectFile = std::string();
				for (auto &outputFile : operation.DeclaredOutput) {
					auto file = fileSystemState.GetFilePath(outputFile);
					if (file.GetFileExtension() == ".pcm") {
						producedModules.emplace(GetModuleNameFromFile(file),
												file.ToString());
					} else if (file.GetFileExtension() == ".o") {
						objectFile = file.ToString();
					}
				}

				translationUnits.push_back(json11::Json::object({
					{"language", "c++"},
					{"arguments", std::move(arguments)},
					{"object", std::move(objectFile)},
					{"provides", std::move(producedModules)},
					{"requires", std::move(requiredModules)},
					{"source", std::move(sourceFile)},
					{"work-directory",
					 operation.Command.WorkingDirectory.ToString()},
				}));
			}
		}
	}

	return translationUnits;
}

void ConvertToJson(FileSystemState &fileSystemState,
				   PackageProvider &packageProvider,
				   const PackageInfo &packageInfo, std::set<int> &knownPackages,
				   json11::Json::array &sets) {
	auto rootPackageGraphId = packageProvider.GetRootPackageGraphId();
	knownPackages.insert(packageInfo.Id);

	auto visibleSets = json11::Json::array();
	for (auto &[dependencyType, dependencyTypeSet] : packageInfo.Dependencies) {
		for (auto &dependency : dependencyTypeSet) {
			if (!dependency.IsSubGraph) {
				auto &dependencyPackageInfo =
					packageProvider.GetPackageInfo(dependency.PackageId);
				visibleSets.push_back(dependencyPackageInfo.Name.ToString() +
									  "@Debug");
				// Stop at the edge of the graph and ignore duplicates
				if (!knownPackages.contains(dependency.PackageId)) {
					ConvertToJson(fileSystemState, packageProvider,
								  dependencyPackageInfo, knownPackages, sets);
				}
			}
		}
	}

	auto translationUnits = GeneratePackageBuildSet(
		fileSystemState, packageProvider, rootPackageGraphId, packageInfo.Id);

	auto setName = packageInfo.Name.ToString() + "@Debug";
	auto set = json11::Json::object({
		{"family-name", packageInfo.Name.ToString()},
		{"name", setName},
		{"baseline-arguments", json11::Json::array({})},
		{"translation-units", std::move(translationUnits)},
		{"visible-sets", std::move(visibleSets)},
	});

	sets.push_back(std::move(set));
}

export std::string LoadBuildGraphContent(const Path &workingDirectory) {
	// Setup the filter
	auto defaultTypes =
		// static_cast<uint32_t>(TraceEventFlag::Diagnostic) |
		// static_cast<uint32_t>(TraceEventFlag::Information) |
		// static_cast<uint32_t>(TraceEventFlag::HighPriority) |
		static_cast<uint32_t>(TraceEventFlag::Warning) |
		static_cast<uint32_t>(TraceEventFlag::Error) |
		static_cast<uint32_t>(TraceEventFlag::Critical);
	auto filter = std::make_shared<EventTypeFilter>(
		static_cast<TraceEventFlag>(defaultTypes));

	// Setup the console listener
	Log::RegisterListener(
		std::make_shared<ConsoleTraceListener>("Log", filter, false, false));

	// Setup the real services
	System::ISystem::Register(std::make_shared<System::STLSystem>());
	System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());

// Platform specific defaults
#if defined(_WIN32)
	auto hostPlatform = "Windows";
#elif defined(__linux__)
	auto hostPlatform = "Linux";
#else
#error "Unknown Platform"
#endif

	auto globalParameters = ValueTable();

	// Load user config state
	auto userDataPath = Build::Constants::GetSoupUserDataPath();

	auto recipeCache = RecipeCache();

	auto packageProvider =
		Build::LoadBuildGraph(workingDirectory, std::nullopt, globalParameters,
							  userDataPath, hostPlatform, recipeCache);

	auto fileSystemState = FileSystemState();

	auto &packageGraph = packageProvider.GetRootPackageGraph();
	auto &rootPackageInfo =
		packageProvider.GetPackageInfo(packageGraph.RootPackageId);

	auto sets = json11::Json::array();
	auto knownPackages = std::set<int>();
	ConvertToJson(fileSystemState, packageProvider, rootPackageInfo,
				  knownPackages, sets);

	json11::Json jsonResult = json11::Json::object(
		{{"version", 1}, {"revision", 0}, {"sets", std::move(sets)}});
	auto value = jsonResult.dump();
	return value;
}
