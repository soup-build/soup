// <copyright file="load-build-graph-result-context.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Text.Json.Serialization;

namespace Soup.Native;

[JsonSourceGenerationOptions(
	WriteIndented = false,
	PropertyNamingPolicy = JsonKnownNamingPolicy.Unspecified,
	GenerationMode = JsonSourceGenerationMode.Metadata)]
[JsonSerializable(typeof(LoadBuildGraphResult))]
internal sealed partial class LoadBuildGraphResultContext : JsonSerializerContext
{
}