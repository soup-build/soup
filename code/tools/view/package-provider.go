// <copyright file="package-provider.go" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

package main

type PackageChildrenMap map[string][]PackageChildInfo
type PackageGraphLookupMap map[int]PackageGraph
type PackageLookupMap map[int]PackageInfo
type PackageTargetDirectories map[int]map[int]string

type PackageId string
type PackageGraphId string

type PackageChildInfo struct {
	OriginalReference string
	IsSubGraph bool
	PackageId PackageId
	PackageGraphId PackageGraphId
}

type PackageInfo struct {
	Id PackageId
	Name string
	Owner string
	IsPrebuilt bool
	PackageRoot string
	Dependencies PackageChildrenMap
}

type PackageGraph struct {
	Id PackageGraphId
	RootPackageId PackageId
	// TODO: public ValueTable GlobalParameters { get; set; }
}

/// <summary>
/// The package provider that maintains the in memory representation of all build state
/// </summary>
type PackageProvider struct {
	RootPackageGraphId PackageGraphId
	PackageGraphLookup PackageGraphLookupMap
	PackageLookup PackageLookupMap
	PackageTargetDirectories PackageTargetDirectories

	// public PackageGraph GetRootPackageGraph()
	// {
	// 	return GetPackageGraph(this.RootPackageGraphId);
	// }

	// public PackageGraph GetPackageGraph(PackageGraphId packageGraphId)
	// {
	// 	// The PackageGraph must already be loaded
	// 	if (this.PackageGraphLookup.TryGetValue(packageGraphId, out var result))
	// 	{
	// 		return result;
	// 	}
	// 	else
	// 	{
	// 		throw new InvalidOperationException($"PackageGraphId [{packageGraphId}] not found in lookup");
	// 	}
	// }

	// public PackageInfo GetPackageInfo(PackageId packageId)
	// {
	// 	// The PackageInfo must already be loaded
	// 	if (this.PackageLookup.TryGetValue(packageId, out var result))
	// 	{
	// 		return result;
	// 	}
	// 	else
	// 	{
	// 		throw new InvalidOperationException($"packageId [{packageId}] not found in lookup");
	// 	}
	// }

	// public Opal.Path GetPackageTargetDirectory(PackageGraphId packageGraphId, PackageId packageId)
	// {
	// 	// The PackageInfo must already be loaded
	// 	if (this.PackageTargetDirectories.TryGetValue(packageGraphId, out var packageTargetDirectorySet))
	// 	{
	// 		if (packageTargetDirectorySet.TryGetValue(packageId, out var result))
	// 		{
	// 			return result;
	// 		}
	// 		else
	// 		{
	// 			throw new InvalidOperationException($"packageId [{packageId}] not found in package target directories");
	// 		}
	// 	}
	// 	else
	// 	{
	// 		throw new InvalidOperationException($"packageGraphId [{packageGraphId}] not found in package target directories");
	// 	}
	// }
}
