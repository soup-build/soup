module;

#include <filesystem>
#include <format>
#include <memory>
#include <spanstream>
#include <string>

export module Soup.BuildDatabase;

import json11;
import Opal;
import Soup.Core;

using namespace Opal;
using namespace Soup::Core;

json11::Json ConvertToJson(const PackageGraphLookupMap &lookup) {
	auto result = json11::Json::array();

	for (const auto &value : lookup) {
		auto jsonValue = json11::Json::object({
			{"Id", value.second.Id},
			{"RootPackageId", value.second.RootPackageId},
			// TODO : { "GlobalParameters", value.second.GlobalParameters },
		});

		result.push_back(std::move(jsonValue));
	}

	return result;
}

json11::Json ConvertToJson(const PackageLookupMap &lookup) {
	auto result = json11::Json::array();

	for (const auto &value : lookup) {
		auto dependencies = json11::Json::object();
		for (const auto &dependency : value.second.Dependencies) {
			auto dependencyChildren = json11::Json::array();
			for (const auto &dependencyChild : dependency.second) {
				auto jsonChildValue = json11::Json::object({
					{"OriginalReference",
					 dependencyChild.OriginalReference.ToString()},
					{"IsSubGraph", dependencyChild.IsSubGraph},
					{"PackageId", dependencyChild.PackageId},
					{"PackageGraphId", dependencyChild.PackageGraphId},
				});
				dependencyChildren.push_back(std::move(jsonChildValue));
			}

			dependencies.emplace(dependency.first,
								 std::move(dependencyChildren));
		}

		auto ownerValue = json11::Json();
		if (value.second.Name.HasOwner()) {
			ownerValue = value.second.Name.GetOwner();
		}

		auto jsonValue = json11::Json::object({
			{"Id", value.second.Id},
			{"Name", value.second.Name.GetName()},
			{"Owner", std::move(ownerValue)},
			{"IsPrebuilt", value.second.IsPrebuilt()},
			{"PackageRoot", value.second.PackageRoot.ToString()},
			{"Dependencies", std::move(dependencies)},
		});
		result.push_back(std::move(jsonValue));
	}

	return result;
}

json11::Json
ConvertToJson(const PackageTargetDirectories &packageGraphTargetDirectories) {
	auto result = json11::Json::object();

	for (const auto &[packageGraphId, packageTargetDirectories] :
		 packageGraphTargetDirectories) {
		auto packageTargetDirectoriesResult = json11::Json::object();
		for (const auto &[packageId, targetDirectory] :
			 packageTargetDirectories) {
			packageTargetDirectoriesResult.emplace(std::to_string(packageId),
												   targetDirectory.ToString());
		}

		result.emplace(std::to_string(packageGraphId),
					   std::move(packageTargetDirectoriesResult));
	}

	return result;
}

Value JsonToValue(const json11::Json &json);

ValueList JsonToValueList(const json11::Json::array &json) {
	auto result = ValueList();
	for (const auto &value : json) {
		result.push_back(JsonToValue(value));
	}

	return result;
}

ValueTable JsonToValueTable(const json11::Json::object &json) {
	auto result = ValueTable();
	for (const auto &[key, value] : json) {
		result.emplace(key, JsonToValue(value));
	}

	return result;
}

Value JsonToValue(const json11::Json &json) {
	switch (json.type()) {
		case json11::Json::NUL:
			throw std::runtime_error("Null is not supported as value");
		case json11::Json::NUMBER:
			return Value(json.number_value());
		case json11::Json::BOOL:
			return Value(json.bool_value());
		case json11::Json::STRING:
			return Value(json.string_value());
		case json11::Json::ARRAY:
			return Value(JsonToValueList(json.array_items()));
		case json11::Json::OBJECT:
			return Value(JsonToValueTable(json.object_items()));
		default:
			throw std::runtime_error("Unknown json type");
	}
}

export std::string LoadBuildGraphContent(const Path &workingDirectory) {
	try {
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
		Log::RegisterListener(std::make_shared<ConsoleTraceListener>(
			"Log", filter, false, false));

		// Setup the real services
		System::ISystem::Register(std::make_shared<System::STLSystem>());
		System::IFileSystem::Register(
			std::make_shared<System::STLFileSystem>());

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

		auto packageProvider = Build::LoadBuildGraph(
			workingDirectory, std::nullopt, globalParameters, userDataPath,
			hostPlatform, recipeCache);

		auto packageGraphs =
			ConvertToJson(packageProvider.GetPackageGraphLookup());
		auto packages = ConvertToJson(packageProvider.GetPackageLookup());
		auto packageTargetDirectories =
			ConvertToJson(packageProvider.GetPackageTargetDirectories());

		json11::Json jsonGraphResult = json11::Json::object({
			{"RootPackageGraphId", packageProvider.GetRootPackageGraphId()},
			{"PackageGraphs", std::move(packageGraphs)},
			{"Packages", std::move(packages)},
			{"PackageTargetDirectories", std::move(packageTargetDirectories)},
		});

		json11::Json jsonResult =
			json11::Json::object({"version" : 1, "revision" : 0, "sets" : []});
		auto value = jsonResult.dump();
		return value;
	} catch (const HandledException &ex) {
		json11::Json jsonResult = json11::Json::object({
			{"Message", std::format("HANDLED ERROR: {0}", ex.GetExitCode())},
			{"IsSuccess", false},
		});
		return jsonResult.dump();
	} catch (const std::exception &ex) {
		json11::Json jsonResult = json11::Json::object({
			{"Message", ex.what()},
			{"IsSuccess", false},
		});
		return jsonResult.dump();
	}
}
