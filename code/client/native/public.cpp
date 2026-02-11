// Include all standard library headers in the global module
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <spanstream>

#ifdef _WIN32
#include <combaseapi.h>
#define SOUP_TOOLS_API __declspec(dllexport)
#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif
#else
#define SOUP_TOOLS_API
#define CoTaskMemAlloc(p) malloc(p)
#define CoTaskMemFree(p) free(p)
#endif

import json11;
import Opal;
import Soup.Core;

using namespace Opal;
using namespace Soup::Core;

json11::Json ConvertToJson(const PackageGraphLookupMap& lookup)
{
	auto result = json11::Json::array();

	for (const auto& value : lookup)
	{
		auto jsonValue = json11::Json::object({
			{ "Id", value.second.Id },
			{ "RootPackageId", value.second.RootPackageId },
			// TODO : { "GlobalParameters", value.second.GlobalParameters },
		});

		result.push_back(std::move(jsonValue));
	}

	return result;
}

json11::Json ConvertToJson(const PackageLookupMap& lookup)
{
	auto result = json11::Json::array();

	for (const auto& value : lookup)
	{
		auto dependencies = json11::Json::object();
		for (const auto& dependency : value.second.Dependencies)
		{
			auto dependencyChildren = json11::Json::array();
			for (const auto& dependencyChild : dependency.second)
			{
				auto jsonChildValue = json11::Json::object({
					{ "OriginalReference", dependencyChild.OriginalReference.ToString() },
					{ "IsSubGraph", dependencyChild.IsSubGraph },
					{ "PackageId", dependencyChild.PackageId },
					{ "PackageGraphId", dependencyChild.PackageGraphId },
				});
				dependencyChildren.push_back(std::move(jsonChildValue));
			}

			dependencies.emplace(dependency.first, std::move(dependencyChildren));
		}

		auto ownerValue = json11::Json();
		if (value.second.Name.HasOwner())
		{
			ownerValue = value.second.Name.GetOwner();
		}

		auto jsonValue = json11::Json::object({
			{ "Id", value.second.Id },
			{ "Name", value.second.Name.GetName() },
			{ "Owner", std::move(ownerValue) },
			{ "IsPrebuilt", value.second.IsPrebuilt() },
			{ "PackageRoot", value.second.PackageRoot.ToString() },
			{ "Dependencies",  std::move(dependencies) },
		});
		result.push_back(std::move(jsonValue));
	}

	return result;
}

json11::Json ConvertToJson(const PackageTargetDirectories& packageGraphTargetDirectories)
{
	auto result = json11::Json::object();

	for (const auto& [packageGraphId, packageTargetDirectories] : packageGraphTargetDirectories)
	{
		auto packageTargetDirectoriesResult = json11::Json::object();
		for (const auto& [packageId, targetDirectory] : packageTargetDirectories)
		{
			packageTargetDirectoriesResult.emplace(std::to_string(packageId), targetDirectory.ToString());
		}

		result.emplace(std::to_string(packageGraphId), std::move(packageTargetDirectoriesResult));
	}

	return result;
}

Value JsonToValue(const json11::Json& json);

ValueList JsonToValueList(const json11::Json::array& json)
{
	auto result = ValueList();
	for (const auto& value : json)
	{
		result.push_back(JsonToValue(value));
	}

	return result;
}

ValueTable JsonToValueTable(const json11::Json::object& json)
{
	auto result = ValueTable();
	for (const auto& [key, value] : json)
	{
		result.emplace(key, JsonToValue(value));
	}

	return result;
}

Value JsonToValue(const json11::Json& json)
{
	switch (json.type())
	{
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

std::string LoadBuildGraphContent(std::string_view workingDirectoryString, std::istream& globalParametersStream)
{
	try
	{
		// Setup the filter
		auto defaultTypes =
			static_cast<uint32_t>(TraceEventFlag::Diagnostic) |
			static_cast<uint32_t>(TraceEventFlag::Information) |
			static_cast<uint32_t>(TraceEventFlag::HighPriority) |
			static_cast<uint32_t>(TraceEventFlag::Warning) |
			static_cast<uint32_t>(TraceEventFlag::Error) |
			static_cast<uint32_t>(TraceEventFlag::Critical);
		auto filter = std::make_shared<EventTypeFilter>(
				static_cast<TraceEventFlag>(defaultTypes));

		// Setup the console listener
		Log::RegisterListener(
			std::make_shared<ConsoleTraceListener>(
				"Log",
				filter,
				false,
				false));

		// Setup the real services
		System::ISystem::Register(std::make_shared<System::STLSystem>());
		System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());

		auto workingDirectory = Path(workingDirectoryString);
		auto globalParameters = ValueTableReader::Deserialize(globalParametersStream);

		// Find the built in folder root
		auto rootDirectory = System::IFileSystem::Current().GetCurrentDirectory();

		// Load user config state
		auto userDataPath = BuildEngine::GetSoupUserDataPath();
		
		auto recipeCache = RecipeCache();

		auto packageProvider = BuildEngine::LoadBuildGraph(
			workingDirectory,
			globalParameters,
			userDataPath,
			recipeCache);

		auto packageGraphs = ConvertToJson(packageProvider.GetPackageGraphLookup());
		auto packages = ConvertToJson(packageProvider.GetPackageLookup());
		auto packageTargetDirectories = ConvertToJson(packageProvider.GetPackageTargetDirectories());

		json11::Json jsonGraphResult = json11::Json::object({
			{ "RootPackageGraphId", packageProvider.GetRootPackageGraphId() },
			{ "PackageGraphs", std::move(packageGraphs) },
			{ "Packages", std::move(packages) },
			{ "PackageTargetDirectories", std::move(packageTargetDirectories) },
		});

		json11::Json jsonResult = json11::Json::object({
			{ "Message", "" },
			{ "IsSuccess", true },
			{ "Graph", std::move(jsonGraphResult) },
		});
		auto value = jsonResult.dump();
		return value;
	}
	catch (const HandledException& ex)
	{
		json11::Json jsonResult = json11::Json::object({
			{ "Message", std::format("HANDLED ERROR: {0}", ex.GetExitCode()) },
			{ "IsSuccess", false },
		});
		return jsonResult.dump();
	}
	catch(const std::exception& ex)
	{
		json11::Json jsonResult = json11::Json::object({
			{ "Message", ex.what() },
			{ "IsSuccess", false },
		});
		return jsonResult.dump();
	}
}

extern "C"
{
	SOUP_TOOLS_API const char* LoadBuildGraph(
		const char* workingDirectory,
		const char* globalParametersBuffer,
		size_t globalParametersLength)
	{
		auto globalParameters = std::ispanstream(
			std::span<char>(const_cast<char*>(globalParametersBuffer), globalParametersLength));

		auto value = LoadBuildGraphContent(workingDirectory, globalParameters);

		auto result = (char*)CoTaskMemAlloc(value.size() + 1);
		value.copy(result, value.size());
		result[value.size()] = 0;
		return result;
	}
}