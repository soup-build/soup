// <copyright file="IZipManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Path = Opal.Path;

namespace Soup.Build.PackageManager;

/// <summary>
/// The zip manager interface
/// Interface mainly used to allow for unit testing client code.
/// </summary>
public interface IZipManager
{
	void ExtractToDirectory(Path sourceArchiveFileName, Path destinationDirectoryName);

	IZipArchive OpenCreate(Path archivePath);
}