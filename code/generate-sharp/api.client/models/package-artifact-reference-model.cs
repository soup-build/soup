// <copyright file="package-feed-exact-reference-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Text.Json.Serialization;

namespace Soup.Build.Api.Client;

/// <summary>
/// A class representing an artifact feed reference to an exact digest.
/// </summary>
public class PackageArtifactReferenceModel
{
	/// <summary>
	/// Gets or sets the digest.
	/// </summary>
	[JsonPropertyName("digest")]
	[JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingDefault)]
	public required string Digest { get; set; }
}
