// <copyright file="package-local-or-public-exact-reference-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Text.Json.Serialization;

namespace Soup.Build.Api.Client;

/// <summary>
/// A class representing a local or public reference to a build dependency package.
/// </summary>
public class PackageBuildDependencyReferenceModel
{
	/// <summary>
	/// Gets or sets the local package id.
	/// </summary>
	[JsonPropertyName("localId")]
	[JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingDefault)]
	public int? LocalId { get; set; }

	/// <summary>
	/// Gets or sets the public package reference.
	/// </summary>
	[JsonPropertyName("public")]
	[JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingDefault)]
	public PackagePublicExactReferenceModel? Public { get; set; }

	/// <summary>
	/// Gets or sets the artifact reference.
	/// </summary>
	[JsonPropertyName("artifact")]
	[JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingDefault)]
	public PackageArtifactReferenceModel? Artifact { get; set; }
}
