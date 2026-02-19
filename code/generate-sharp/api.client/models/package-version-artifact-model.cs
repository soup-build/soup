// <copyright file="package-version-artifact-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.ComponentModel.DataAnnotations;
using System.Text.Json.Serialization;

namespace Soup.Build.Api.Client;

/// <summary>
/// Package version artifact model.
/// </summary>
public class PackageVersionArtifactModel
{
	/// <summary>
	/// Gets or sets the version.
	/// </summary>
	[JsonPropertyName("digest")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required]
	public required string Digest { get; set; }

	/// <summary>
	/// Gets or sets the date published.
	/// </summary>
	[JsonPropertyName("datePublished")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required(AllowEmptyStrings = true)]
	public System.DateTimeOffset DatePublished { get; set; }

	/// <summary>
	/// Gets or sets the total downloads count.
	/// </summary>
	[JsonPropertyName("totalDownloads")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	public int TotalDownloads { get; set; }
}