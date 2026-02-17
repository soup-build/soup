// <copyright file="package-provider.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <stdexcept>
#include <map>
#include <string>
#include <vector>

export module Soup.Core:PackageProvider;

import Opal;
import :PackageName;
import :PackageReference;
import :Recipe;
import :Value;

using namespace Opal;

export namespace Soup::Core
{
	using PackageId = int;
	using PackageGraphId = int;

	class PackageChildInfo
	{
	public:
		PackageChildInfo() :
			OriginalReference(),
			IsSubGraph(false),
			PackageId(-1),
			PackageGraphId(-1)
		{
		}

		PackageChildInfo(
			PackageReference originalReference,
			bool isSubGraph,
			PackageId packageId,
			PackageGraphId packageGraphId) :
			OriginalReference(originalReference),
			IsSubGraph(isSubGraph),
			PackageId(packageId),
			PackageGraphId(packageGraphId)
		{
		}

		PackageReference OriginalReference;
		bool IsSubGraph;
		::Soup::Core::PackageId PackageId;
		::Soup::Core::PackageGraphId PackageGraphId;

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageChildInfo& rhs) const
		{
			return OriginalReference == rhs.OriginalReference &&
				IsSubGraph == rhs.IsSubGraph &&
				PackageId == rhs.PackageId &&
				PackageGraphId == rhs.PackageGraphId;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageChildInfo& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using PackageChildrenMap = std::map<std::string, std::vector<PackageChildInfo>>;

	class PackageInfo
	{
	public:
		PackageInfo(
			PackageId id,
			PackageName name,
			std::optional<std::string> artifactDigest,
			Path packageRoot,
			const Recipe* recipe,
			PackageChildrenMap dependencies) :
			Id(id),
			Name(std::move(name)),
			ArtifactDigest(std::move(artifactDigest)),
			PackageRoot(std::move(packageRoot)),
			Recipe(recipe),
			Dependencies(std::move(dependencies))
		{
		}

		PackageId Id;
		PackageName Name;
		std::optional<std::string> ArtifactDigest;
		Path PackageRoot;
		const ::Soup::Core::Recipe* Recipe;
		PackageChildrenMap Dependencies;

		bool IsPrebuilt() const
		{
			return ArtifactDigest.has_value();
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageInfo& rhs) const
		{
			auto artifactDigestEqual = ArtifactDigest.has_value() == rhs.ArtifactDigest.has_value();
			if (ArtifactDigest.has_value())
			{
				artifactDigestEqual = ArtifactDigest.value() == rhs.ArtifactDigest.value();
			}

			return Id == rhs.Id &&
				Name == rhs.Name &&
				artifactDigestEqual &&
				PackageRoot == rhs.PackageRoot &&
				Recipe == rhs.Recipe &&
				Dependencies == rhs.Dependencies;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageInfo& rhs) const
		{
			return !(*this == rhs);
		}
	};

	class PackageGraph
	{
	public:
		PackageGraph(
			PackageGraphId id,
			PackageId rootPackageId,
			ValueTable globalParameters) :
			Id(id),
			RootPackageId(rootPackageId),
			GlobalParameters(std::move(globalParameters))
		{
		}

		PackageGraphId Id;
		PackageId RootPackageId;
		ValueTable GlobalParameters;

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageGraph& rhs) const
		{
			return Id == rhs.Id &&
				RootPackageId == rhs.RootPackageId &&
				GlobalParameters == rhs.GlobalParameters;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageGraph& rhs) const
		{
			return !(*this == rhs);
		}
	};

	/// <summary>
	/// The package provider that maintains the in memory representation of all build state
	/// </summary>
	using PackageGraphLookupMap = std::map<PackageGraphId, PackageGraph>;
	using PackageLookupMap = std::map<PackageId, PackageInfo>;
	using PackageTargetDirectories = std::map<PackageGraphId, std::map<PackageId, Path>>;
	class PackageProvider
	{
	private:
		PackageGraphId _rootPackageGraphId;
		PackageGraphLookupMap _packageGraphLookup;
		PackageLookupMap _packageLookup;
		PackageTargetDirectories _packageTargetDirectories;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="PackageProvider"/> class.
		/// </summary>
		PackageProvider(
			PackageGraphId rootPackageGraphId,
			PackageGraphLookupMap packageGraphLookup,
			PackageLookupMap packageLookup,
			PackageTargetDirectories packageTargetDirectories) :
			_rootPackageGraphId(rootPackageGraphId),
			_packageGraphLookup(std::move(packageGraphLookup)),
			_packageLookup(std::move(packageLookup)),
			_packageTargetDirectories(std::move(packageTargetDirectories))
		{
		}

		PackageGraphId GetRootPackageGraphId()
		{
			return _rootPackageGraphId;
		}

		const PackageGraphLookupMap& GetPackageGraphLookup()
		{
			return _packageGraphLookup;
		}

		const PackageLookupMap& GetPackageLookup()
		{
			return _packageLookup;
		}

		const PackageTargetDirectories& GetPackageTargetDirectories()
		{
			return _packageTargetDirectories;
		}

		const PackageGraph& GetRootPackageGraph()
		{
			return GetPackageGraph(_rootPackageGraphId);
		}

		const PackageGraph& GetPackageGraph(PackageGraphId packageGraphId)
		{
			// The PackageGraph must already be loaded
			auto findPackageGraph = _packageGraphLookup.find(packageGraphId);
			if (findPackageGraph != _packageGraphLookup.end())
			{
				return findPackageGraph->second;
			}
			else
			{
				throw std::runtime_error(
					std::format("PackageGraphId [{}] not found in lookup", packageGraphId));
			}
		}

		const PackageInfo& GetPackageInfo(PackageId packageId)
		{
			// The PackageInfo must already be loaded
			auto findPackageInfo = _packageLookup.find(packageId);
			if (findPackageInfo != _packageLookup.end())
			{
				return findPackageInfo->second;
			}
			else
			{
				throw std::runtime_error(
					std::format("packageId [{}] not found in lookup", packageId));
			}
		}


		const Path& GetTargetDirectory(PackageGraphId packageGraphId, PackageId packageId)
		{
			// The PackageGraph must already be loaded
			auto findPackageGraphTargetDirectory = _packageTargetDirectories.find(packageGraphId);
			if (findPackageGraphTargetDirectory != _packageTargetDirectories.end())
			{
				auto findPackageTargetDirectory = findPackageGraphTargetDirectory->second.find(packageId);
				if (findPackageTargetDirectory != findPackageGraphTargetDirectory->second.end())
				{
					return findPackageTargetDirectory->second;
				}
				else
				{
					throw std::runtime_error(
						std::format("packageId [{}] not found in lookup", packageId));
				}
			}
			else
			{
				throw std::runtime_error(
					std::format("PackageGraphId [{}] not found in lookup", packageGraphId));
			}
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageProvider& rhs) const
		{
			return _rootPackageGraphId == rhs._rootPackageGraphId &&
				_packageGraphLookup == rhs._packageGraphLookup &&
				_packageLookup == rhs._packageLookup &&
				_packageTargetDirectories == rhs._packageTargetDirectories;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageProvider& rhs) const
		{
			return !(*this == rhs);
		}
	};
}