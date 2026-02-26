// <copyright file="package-reference.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using System;
using System.Collections.Generic;
using Path = Opal.Path;

namespace Soup.Build;

/// <summary>
/// A resolved package reference object which will consist of a name version pair that
/// refers to a published package or a path to a local recipe
/// </summary>
public partial class ResolvedBuildPackageReference : IEquatable<ResolvedBuildPackageReference>
{
	private readonly PackageIdentifier? packageIdentifier;
	private readonly SemanticVersion? version;
#pragma warning disable IDE0032 // Use auto property
	private readonly string? digest;
	private readonly Dictionary<string, string>? artifactDigests;
#pragma warning restore IDE0032 // Use auto property
	private readonly Path? path;

	/// <summary>
	/// Initializes a new instance of the <see cref="ResolvedBuildPackageReference"/> class.
	/// </summary>
	public ResolvedBuildPackageReference()
	{
		this.packageIdentifier = null;
		this.version = null;
		this.digest = null;
		this.artifactDigests = null;
		this.path = null;
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="ResolvedBuildPackageReference"/> class.
	/// </summary>
	public ResolvedBuildPackageReference(
		string? language,
		string? owner,
		string name,
		SemanticVersion version,
		string digest,
		Dictionary<string, string>? artifactDigests)
	{
		this.packageIdentifier = new PackageIdentifier(language, owner, name);
		this.version = version;
		this.digest = digest;
		this.artifactDigests = artifactDigests;
		this.path = null;
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="ResolvedBuildPackageReference"/> class.
	/// </summary>
	public ResolvedBuildPackageReference(Path path)
	{
		this.packageIdentifier = null;
		this.version = null;
		this.digest = null;
		this.path = path;
	}

	/// <summary>
	/// Gets a value indicating whether the reference is local or not
	/// </summary>
	public bool IsLocal => this.packageIdentifier is null;

	/// <summary>
	/// Gets or sets the Language.
	/// </summary>
	public string? Language
	{
		get
		{
			if (this.packageIdentifier is null)
				throw new InvalidOperationException("Cannot get the language of a local reference.");
			return this.packageIdentifier.Language;
		}
	}

	/// <summary>
	/// Gets or sets the Owner.
	/// </summary>
	public string? Owner
	{
		get
		{
			if (this.packageIdentifier is null)
				throw new InvalidOperationException("Cannot get the owner of a local reference.");
			return this.packageIdentifier.Owner;
		}
	}

	/// <summary>
	/// Gets or sets the Name.
	/// </summary>
	public string Name
	{
		get
		{
			if (this.packageIdentifier is null)
				throw new InvalidOperationException("Cannot get the name of a local reference.");
			return this.packageIdentifier.Name;
		}
	}

	/// <summary>
	/// Gets or sets the Version.
	/// </summary>
	public SemanticVersion? Version
	{
		get
		{
			if (this.IsLocal)
				throw new InvalidOperationException("Cannot get the version of a local reference.");
			return this.version;
		}
	}

	/// <summary>
	/// Gets or sets the digest.
	/// </summary>
	public string Digest
	{
		get
		{
			if (this.digest is null)
				throw new InvalidOperationException("Cannot get the digest of a local reference.");
			return this.digest;
		}
	}

	/// <summary>
	/// Gets or sets the artifact digests.
	/// </summary>
	public Dictionary<string, string>? ArtifactDigests
	{
		get
		{
			return this.artifactDigests;
		}
	}

	/// <summary>
	/// Gets or sets the Path.
	/// </summary>
	public Path Path
	{
		get
		{
			if (this.path is null)
				throw new InvalidOperationException("Cannot get the path of a non-local reference.");
			return this.path;
		}
	}

	/// <summary>
	/// Equality operator
	/// </summary>
	public bool Equals(ResolvedBuildPackageReference? other)
	{
		if (other is null)
			return false;
		return this.packageIdentifier == other.packageIdentifier &&
			this.version == other.version &&
			this.path == other.path;
	}

	public override bool Equals(object? obj)
	{
		return Equals(obj as ResolvedBuildPackageReference);
	}

	public override int GetHashCode()
	{
		var identifierHash = this.packageIdentifier is null ? 0 : this.packageIdentifier.GetHashCode() * 0x100000;
		var versionHash = this.version is null ? 0 : this.version.GetHashCode() * 0x1000;
		var pathHash = this.path is null ? 0 : this.path.GetHashCode();
		return identifierHash + versionHash + pathHash;
	}

	public static bool operator ==(ResolvedBuildPackageReference? lhs, ResolvedBuildPackageReference? rhs)
	{
		return lhs is null ? rhs is null : lhs.Equals(rhs);
	}

	public static bool operator !=(ResolvedBuildPackageReference? lhs, ResolvedBuildPackageReference? rhs)
	{
		return !(lhs == rhs);
	}

	/// <summary>
	/// Convert to string
	/// </summary>
	public override string ToString()
	{
		// If the reference is a path then just return that
		if (this.path is not null)
		{
			return this.path.ToString();
		}
		else
		{
			// Build up the language/owner/name/version reference
			if (this.version is not null)
			{
				return $"{this.packageIdentifier}@{this.version}";
			}
			else
			{
				return $"{this.packageIdentifier}";
			}
		}
	}
}