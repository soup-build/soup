// <copyright file="package-lock.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <stdexcept>
#include <map>
#include <optional>
#include <string>
#include <vector>

export module Soup.Core:PackageLock;

import Opal;
import :PackageName;
import :PackageReference;
import :PackageWithArtifactReference;
import :RecipeValue;
import :SequenceMap;

using namespace Opal;

export namespace Soup::Core
{
	class PackageClosureValue
	{
	public:
		PackageClosureValue(
			PackageReference reference,
			std::string build,
			std::string tool) :
			Reference(std::move(reference)),
			Build(std::move(build)),
			Tool(std::move(tool))
		{
		}

		PackageReference Reference;
		std::string Build;
		std::string Tool;
	};

	/// <summary>
	/// The package lock container
	/// </summary>
	using PackageClosure = std::map<std::string, std::map<PackageName, PackageClosureValue>>;
	using BuildSet = std::map<std::string, std::map<PackageName, PackageWithArtifactReference>>;
	using BuildSets = std::map<std::string, BuildSet>;
	class PackageLock
	{
	private:
		static constexpr const char* Property_Artifacts = "Artifacts";
		static constexpr const char* Property_Closure = "Closure";
		static constexpr const char* Property_Version = "Version";
		static constexpr const char* Property_Build = "Build";
		static constexpr const char* Property_Builds = "Builds";
		static constexpr const char* Property_Tool = "Tool";
		static constexpr const char* Property_Tools = "Tools";

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="PackageLock"/> class.
		/// </summary>
		PackageLock() :
			_table()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="PackageLock"/> class.
		/// </summary>
		PackageLock(RecipeTable table) :
			_table(std::move(table))
		{
		}

		/// <summary>
		/// Gets or sets the version
		/// </summary>
		bool HasVersion()
		{
			return HasValue(_table, Property_Version);
		}

		int64_t GetVersion()
		{
			if (!HasVersion())
				throw std::runtime_error("No version.");

			auto value = GetValue(_table, Property_Version).AsInteger();
			return value;
		}

		/// <summary>
		/// Gets the closure
		/// </summary>
		PackageClosure GetClosure()
		{
			if (!HasValue(_table, Property_Closure))
				throw std::runtime_error("No closures.");

			auto& closureValue = GetValue(_table, Property_Closure).AsTable();
			auto result = PackageClosure();
			for (const auto& [languageKey, languageValue] : closureValue)
			{
				auto languageLock = std::map<PackageName, PackageClosureValue>();
				for (auto& [projectUniqueName, projectValue] : languageValue.AsTable())
				{
					auto& projectTable = projectValue.AsTable();

					auto projectName = PackageName::Parse(projectUniqueName);

					if (!HasValue(projectTable, Property_Version))
						throw std::runtime_error("No Version on project table.");
					auto& versionValue = GetValue(projectTable, Property_Version);

					PackageReference reference;
					if (versionValue.IsVersion())
					{
						auto version = versionValue.AsVersion();
						reference = PackageReference(std::nullopt, projectName.GetOwner(), projectName.GetName(), version);
					}
					else if (versionValue.IsString())
					{
						SemanticVersion version;
						if (SemanticVersion::TryParse(versionValue.AsString(), version))
						{
							reference = PackageReference(std::nullopt, projectName.GetOwner(), projectName.GetName(), version);
						}
						else
						{
							// Assume that the version value is a path
							reference = PackageReference(Path(versionValue.AsString()));
						}
					}
					else
					{
						throw std::runtime_error("Package version must be a Version or String.");
					}

					if (!HasValue(projectTable, Property_Build))
						throw std::runtime_error("Missing closure package build set reference.");
					auto buildValue = GetValue(projectTable, Property_Build).AsString();

					if (!HasValue(projectTable, Property_Tool))
						throw std::runtime_error("Missing closure package build set reference.");
					auto toolValue = GetValue(projectTable, Property_Tool).AsString();

					auto lockValue = PackageClosureValue(std::move(reference), std::move(buildValue), std::move(toolValue));
					languageLock.emplace(projectName, std::move(lockValue));
				}

				result.emplace(languageKey, std::move(languageLock));
			}

			return result;
		}

		/// <summary>
		/// Gets the build sets
		/// </summary>
		BuildSets GetBuildSets(std::string_view hostPlatform)
		{
			return GetBuildPackageSets(Property_Builds, hostPlatform);
		}

		/// <summary>
		/// Gets the tool sets
		/// </summary>
		BuildSets GetToolSets(std::string_view hostPlatform)
		{
			return GetBuildPackageSets(Property_Tools, hostPlatform);
		}

		/// <summary>
		/// Raw access
		/// </summary>
		RecipeTable& GetTable()
		{
			return _table;
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageLock& rhs) const
		{
			return _table == rhs._table;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageLock& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		BuildSets GetBuildPackageSets(
			std::string_view property,
			std::string_view hostPlatform)
		{
			if (!HasValue(_table, property))
				throw std::runtime_error("No build sets.");

			auto& values = GetValue(_table, property).AsTable();
			auto result = BuildSets();
			for (const auto& [buildSetKey, buildSetValue] : values)
			{
				auto buildSet = BuildSet();
				for (const auto& [languageKey, languageValue] : buildSetValue.AsTable())
				{
					auto languageLock = std::map<PackageName, PackageWithArtifactReference>();
					for (auto& [projectUniqueName, projectValue] : languageValue.AsTable())
					{
						auto& projectTable = projectValue.AsTable();

						auto projectName = PackageName::Parse(projectUniqueName);

						if (!HasValue(projectTable, Property_Version))
							throw std::runtime_error("No Version on project table.");
						auto& versionValue = GetValue(projectTable, Property_Version);

						std::optional<std::string> artifactDigest;
						if (HasValue(projectTable, Property_Artifacts))
						{
							auto& hostPlatformTable = GetValue(projectTable, Property_Artifacts).AsTable();
							if (HasValue(hostPlatformTable, hostPlatform))
							{
								artifactDigest = GetValue(hostPlatformTable, hostPlatform).AsString();
							}
						}

						PackageWithArtifactReference reference;
						if (versionValue.IsVersion())
						{
							auto version = versionValue.AsVersion();
							reference = PackageWithArtifactReference(
								PackageReference(std::nullopt, projectName.GetOwner(), projectName.GetName(), version),
								artifactDigest);
						}
						else if (versionValue.IsString())
						{
							SemanticVersion version;
							if (SemanticVersion::TryParse(versionValue.AsString(), version))
							{
								reference = PackageWithArtifactReference(
									PackageReference(std::nullopt, projectName.GetOwner(), projectName.GetName(), version),
									artifactDigest);
							}
							else
							{
								// Assume that the version value is a path
								reference = PackageWithArtifactReference(
									PackageReference(Path(versionValue.AsString())),
									std::nullopt);
							}
						}
						else
						{
							throw std::runtime_error("Package version must be a Version or String.");
						}

						languageLock.emplace(projectName, std::move(reference));
					}

					buildSet.emplace(languageKey, std::move(languageLock));
				}

				result.emplace(buildSetKey, std::move(buildSet));
			}

			return result;
		}

		bool HasValue(const RecipeTable& table, std::string_view key) const
		{
			return table.Contains(key.data());
		}

		const RecipeValue& GetValue(const RecipeTable& table, std::string_view key) const
		{
			const RecipeValue* value;
			if (table.TryGet(key.data(), value))
			{
				return *value;
			}
			else
			{
				throw std::runtime_error("Requested package lock value does not exist in the table.");
			}
		}

		RecipeValue& GetValue(RecipeTable& table, std::string_view key)
		{
			RecipeValue* value;
			if (table.TryGet(key.data(), value))
			{
				return *value;
			}
			else
			{
				throw std::runtime_error("Requested recipe value does not exist in the table.");
			}
		}

	private:
		RecipeTable _table;
	};
}
