﻿
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text.Json;
using Path = Opal.Path;

namespace Soup.View;

public static partial class SoupTools
{
	[LibraryImport("SoupTools", StringMarshalling = StringMarshalling.Custom, StringMarshallingCustomType = typeof(AnsiStringMarshaller))]
	[UnmanagedCallConv(CallConvs = new Type[] { typeof(CallConvCdecl) })]
	[DefaultDllImportSearchPaths(DllImportSearchPath.UserDirectories)]
	private static partial string LoadBuildGraph(string workingDirectory);

	public static PackageProvider LoadBuildGraph(Path workingDirectory)
	{
		var loadResult = LoadBuildGraph(workingDirectory.ToString());

		var result = JsonSerializer.Deserialize(loadResult, LoadBuildGraphResultContext.Default.LoadBuildGraphResult) ??
			throw new InvalidOperationException("Failed to deserialize the result");

		if (!result.IsSuccess)
			throw new InvalidOperationException(result.Message);

		if (result.Graph is null)
			throw new InvalidOperationException("Successful load build graph did not have a graph");

		var packageGraphLookup = new Dictionary<int, PackageGraph>();
		foreach (var graph in result.Graph.PackageGraphs)
		{
			packageGraphLookup.Add(graph.Id, graph);
		}

		var packageLookup = new Dictionary<int, PackageInfo>();
		foreach (var package in result.Graph.Packages)
		{
			packageLookup.Add(package.Id, package);
		}

		var provider = new PackageProvider()
		{
			RootPackageGraphId = result.Graph.RootPackageGraphId,
			PackageGraphLookup = packageGraphLookup,
			PackageLookup = packageLookup,
		};

		return provider;
	}
}