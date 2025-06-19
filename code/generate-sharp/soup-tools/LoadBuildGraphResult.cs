// <copyright file="LoadBuildGraphResult.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Soup.Tools;

public class LoadBuildGraph
{
	public int RootPackageGraphId { get; set; }
	public IList<PackageGraph> PackageGraphs { get; init; } = [];
	public IList<PackageInfo> Packages { get; init; } = [];
}

public class LoadBuildGraphResult
{
	public bool IsSuccess { get; set; }
	public string? Message { get; set; }
	public LoadBuildGraph? Graph { get; set; }
}