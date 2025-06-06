﻿// <copyright file="IClosureManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Threading.Tasks;
using Path = Opal.Path;

namespace Soup.Build.PackageManager;

/// <summary>
/// The closure manager interface
/// Interface mainly used to allow for unit testing client code.
/// </summary>
public interface IClosureManager
{
	Task GenerateAndRestoreRecursiveLocksAsync(
		Path workingDirectory,
		Path packageStoreDirectory,
		Path packageLockStoreDirectory,
		Path stagingDirectory);
}
