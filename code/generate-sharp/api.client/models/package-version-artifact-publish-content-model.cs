// <copyright file="package-create-or-update-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Soup.Build.Api.Client;

/// <summary>
/// A container for the package creation or update request.
/// </summary>
public class PackageVersionArtifactPublishContentModel
{
	/// <summary>
	/// Gets or sets the context.
	/// </summary>
	public required IDictionary<string, string> Context { get; init; }

	/// <summary>
	/// Gets or sets the parameters.
	/// </summary>
	public required IDictionary<string, string> Parameters { get; init; }
}
