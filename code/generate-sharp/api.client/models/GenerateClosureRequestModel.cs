﻿// <copyright file="GenerateClosureRequestModel.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text.Json.Serialization;

namespace Soup.Build.Api.Client;

/// <summary>
/// A container for the generate closure request.
/// </summary>
public class GenerateClosureRequestModel
{
	/// <summary>
	/// Gets or sets the roots package.
	/// </summary>
	[JsonPropertyName("rootPackage")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required]
	public required PackageLocalReferenceModel RootPackage { get; set; }

	/// <summary>
	/// Gets or sets the collection of local packages that will be resolved.
	/// </summary>
	[JsonPropertyName("localPackages")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required]
	public ICollection<PackageLocalReferenceModel> LocalPackages { get; init; } = [];

	/// <summary>
	/// Gets or sets the collection of known public packages that will be resolved.
	/// </summary>

	[JsonPropertyName("publicPackages")]

	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required]
	public ICollection<PackagePublicReferenceModel> PublicPackages { get; init; } = [];

	/// <summary>
	/// Gets or sets the collection of preferred versions for public packages.
	/// </summary>
	[JsonPropertyName("preferredVersions")]
	[JsonIgnore(Condition = JsonIgnoreCondition.Never)]
	[Required]
	public ICollection<PackagePublicExactReferenceModel> PreferredVersions { get; init; } = [];
}