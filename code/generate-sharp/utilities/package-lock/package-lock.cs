// <copyright file="package-lock.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using System;

namespace Soup.Build.Utilities;

/// <summary>
/// The package lock container
/// </summary>
public class PackageLock
{
	public static string Property_Artifacts => "Artifacts";
	public static string Property_Digest => "Digest";
	public static string Property_Version => "Version";
	private static string Property_Closure => "Closure";
	private static string Property_Build => "Build";
	private static string Property_Builds => "Builds";
	private static string Property_Tool => "Tool";
	private static string Property_Tools => "Tools";

	/// <summary>
	/// Initializes a new instance of the <see cref="PackageLock"/> class.
	/// </summary>
	public PackageLock()
	{
		this.Document = new SMLDocument();
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="PackageLock"/> class.
	/// </summary>
	public PackageLock(SMLDocument table)
	{
		this.Document = table;
	}

	/// <summary>
	/// Gets or sets the version
	/// </summary>
	public bool HasVersion()
	{
		return HasValue(this.Document, Property_Version);
	}

	public long GetVersion()
	{
		if (!HasVersion())
			throw new InvalidOperationException("No version.");

		var value = GetValue(this.Document, Property_Version).AsInteger();
		return value.Value;
	}

	public void SetVersion(long value)
	{
		this.Document.AddItemWithSyntax(Property_Version, value);
	}

	/// <summary>
	/// Gets or sets the table of closures
	/// </summary>
	public bool HasClosure()
	{
		return HasValue(this.Document, Property_Closure);
	}

	public SMLTable GetClosure()
	{
		if (!HasClosure())
			throw new InvalidOperationException("No closure.");

		var values = GetValue(this.Document, Property_Closure).AsTable();
		return values;
	}

	public bool HasBuildClosures()
	{
		return HasValue(this.Document, Property_Builds);
	}

	public SMLTable GetBuildClosures()
	{
		if (!HasBuildClosures())
			throw new InvalidOperationException("No build closures.");

		var values = GetValue(this.Document, Property_Builds).AsTable();
		return values;
	}

	public void EnsureBuildClosure(string closure)
	{
		var closures = EnsureHasTable(this.Document, Property_Builds);
		_ = EnsureHasTable(closures, closure, 1);
	}

	public bool HasToolClosures()
	{
		return HasValue(this.Document, Property_Tools);
	}

	public SMLTable GetToolClosures()
	{
		if (!HasBuildClosures())
			throw new InvalidOperationException("No tool closures.");

		var values = GetValue(this.Document, Property_Tools).AsTable();
		return values;
	}

	public void EnsureToolClosure(string closure)
	{
		var closures = EnsureHasTable(this.Document, Property_Tools);
		_ = EnsureHasTable(closures, closure, 1);
	}

	public void AddProject(
		Path workingDirectory,
		string language,
		PackageName uniqueName,
		ResolvedRuntimePackageReference package,
		string? buildClosure,
		string? toolClosure)
	{
		var closureTable = EnsureHasTable(this.Document, Property_Closure);
		var projectLanguageTable = EnsureHasTable(closureTable, language, 1);

		var projectTable = projectLanguageTable.AddInlineTableWithSyntax(uniqueName.ToString(), 2);

		if (package.IsLocal)
		{
			var relativePath = package.Path.GetRelativeTo(workingDirectory);
			projectTable.AddInlineItemWithSyntax(Property_Version, relativePath.ToString());
		}
		else
		{
			if (package.Version is null)
				throw new InvalidOperationException("Missing version on external package reference");
			projectTable.AddInlineItemWithSyntax(Property_Version, package.Version);
			projectTable.AddInlineItemWithSyntax(Property_Digest, package.Digest);
		}

		if (buildClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Build, buildClosure);
		}

		if (toolClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Tool, toolClosure);
		}
	}

	public void AddBuildProject(
		Path workingDirectory,
		string closure,
		string language,
		PackageName uniqueName,
		ResolvedBuildPackageReference package,
		string? buildClosure,
		string? toolClosure)
	{
		var closures = EnsureHasTable(this.Document, Property_Builds);
		var closureTable = EnsureHasTable(closures, closure, 1);
		var projectLanguageTable = EnsureHasTable(closureTable, language, 2);

		var projectTable = package.IsLocal ?
			projectLanguageTable.AddInlineTableWithSyntax(uniqueName.ToString(), 3) :
			projectLanguageTable.AddTableWithSyntax(uniqueName.ToString(), 3);

		if (package.IsLocal)
		{
			var relativePath = package.Path.GetRelativeTo(workingDirectory);
			projectTable.AddInlineItemWithSyntax(Property_Version, relativePath.ToString());
		}
		else
		{
			if (package.Version is null)
				throw new InvalidOperationException("Missing version on external package reference");

			projectTable.AddItemWithSyntax(Property_Version, package.Version, 4);
			projectTable.AddItemWithSyntax(Property_Digest, package.Digest, 4);

			if (package.ArtifactDigests is not null && package.ArtifactDigests.Count > 0)
			{

				var artifactTable = projectTable.AddTableWithSyntax(Property_Artifacts, 4);
				foreach (var (hostPlatform, digest) in package.ArtifactDigests)
				{
					artifactTable.AddItemWithSyntax(hostPlatform, digest, 5);
				}
			}
		}

		if (buildClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Build, buildClosure);
		}

		if (toolClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Tool, toolClosure);
		}
	}

	public void AddToolProject(
		Path workingDirectory,
		string closure,
		string language,
		PackageName uniqueName,
		ResolvedBuildPackageReference package,
		string? buildClosure,
		string? toolClosure)
	{
		var closures = EnsureHasTable(this.Document, Property_Tools);
		var closureTable = EnsureHasTable(closures, closure, 1);
		var projectLanguageTable = EnsureHasTable(closureTable, language, 2);

		var projectTable = package.IsLocal ?
			projectLanguageTable.AddInlineTableWithSyntax(uniqueName.ToString(), 3) :
			projectLanguageTable.AddTableWithSyntax(uniqueName.ToString(), 3);

		if (package.IsLocal)
		{
			var relativePath = package.Path.GetRelativeTo(workingDirectory);
			projectTable.AddInlineItemWithSyntax(Property_Version, relativePath.ToString());
		}
		else
		{
			if (package.Version is null)
				throw new InvalidOperationException("Missing version on external package reference");

			projectTable.AddItemWithSyntax(Property_Version, package.Version, 4);
			projectTable.AddItemWithSyntax(Property_Digest, package.Digest, 4);

			if (package.ArtifactDigests is not null && package.ArtifactDigests.Count > 0)
			{

				var artifactTable = projectTable.AddTableWithSyntax(Property_Artifacts, 4);
				foreach (var (hostPlatform, digest) in package.ArtifactDigests)
				{
					artifactTable.AddItemWithSyntax(hostPlatform, digest, 5);
				}
			}
		}

		if (buildClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Build, buildClosure);
		}

		if (toolClosure != null)
		{
			projectTable.AddInlineItemWithSyntax(Property_Tool, toolClosure);
		}
	}

	/// <summary>
	/// Raw access
	/// </summary>
	public SMLDocument Document { get; }

	private static bool HasValue(SMLDocument document, string key)
	{
		return document.Values.ContainsKey(key);
	}

	private static SMLValue GetValue(SMLDocument document, string key)
	{
		if (document.Values.TryGetValue(key, out var value))
		{
			return value.Value;
		}
		else
		{
			throw new InvalidOperationException("Requested recipe value does not exist in the table.");
		}
	}

	private static SMLTable EnsureHasTable(SMLDocument document, string name)
	{
		if (document.Values.TryGetValue(name, out var value))
		{
			if (value.Value.Type != SMLValueType.Table)
				throw new InvalidOperationException($"The package lock already has a non-table dependencies property: {name}");

			// Find the Syntax for the table
			return value.Value.AsTable();
		}
		else
		{
			// Create a new table
			return document.AddTableWithSyntax(name);
		}
	}

	private static SMLTable EnsureHasTable(SMLTable table, string name, int indentLevel)
	{
		if (table.Values.TryGetValue(name, out var value))
		{
			if (value.Value.Type != SMLValueType.Table)
				throw new InvalidOperationException($"The package lock already has a non-table dependencies property: {name}");

			// Find the Syntax for the table
			return value.Value.AsTable();
		}
		else
		{
			// Create a new table
			return table.AddTableWithSyntax(name, indentLevel);
		}
	}
}