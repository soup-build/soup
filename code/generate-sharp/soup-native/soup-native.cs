// <copyright file="soup-tools.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text.Json;
using Soup.Build.Utilities;
using Path = Opal.Path;

namespace Soup.Native;

public static partial class SoupTools
{
	[LibraryImport("SoupTools", StringMarshalling = StringMarshalling.Custom, StringMarshallingCustomType = typeof(AnsiStringMarshaller))]
	[UnmanagedCallConv(CallConvs = new Type[] { typeof(CallConvCdecl) })]
	[DefaultDllImportSearchPaths(DllImportSearchPath.UserDirectories)]
	private static partial string LoadBuildGraph(string workingDirectory, [In] byte[] globalParametersBuffer, long globalParametersLength);

	public static PackageProvider LoadBuildGraph(Path workingDirectory, ValueTable globalParameters)
	{
		using var memoryStream = new System.IO.MemoryStream();
		using var writer = new System.IO.BinaryWriter(memoryStream);

		ValueTableWriter.Serialize(globalParameters, writer);

		var loadResult = LoadBuildGraph(workingDirectory.ToString(), memoryStream.GetBuffer(), memoryStream.Length);

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

		var packageTargetDirectories = result.Graph.PackageTargetDirectories.ToDictionary(
			key => key.Key,
			value => (IDictionary<int, Path>)value.Value.ToDictionary(key => key.Key, value => new Path(value.Value)));

		var provider = new PackageProvider()
		{
			RootPackageGraphId = result.Graph.RootPackageGraphId,
			PackageGraphLookup = packageGraphLookup,
			PackageLookup = packageLookup,
			PackageTargetDirectories = packageTargetDirectories,
		};

		return provider;
	}
}