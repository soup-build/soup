﻿// <copyright file="IFileSystem.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Opal.System;

/// <summary>
/// The file system interface
/// Interface mainly used to allow for unit testing client code.
/// </summary>
public interface IFileSystem
{
	/// <summary>
	/// Gets the current user profile directory.
	/// </summary>
	Path GetUserProfileDirectory();

	/// <summary>
	/// Gets the current directory for the running processes.
	/// </summary>
	Path GetCurrentDirectory();

	/// <summary>
	/// Gets a value indicating whether the directory/file exists.
	/// </summary>
	/// <param name="path">The path.</param>
	bool Exists(Path path);

	/// <summary>
	/// Get the last write time of the file/directory
	/// TODO: This should be a better representation of datetime...
	/// </summary>
	/// <param name="path">The path.</param>
	int GetLastWriteTime(Path path);

	/// <summary>
	/// Set the last write time of the file/directory
	/// TODO: This should be a better representation of datetime...
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="value">The value.</param>
	void SetLastWriteTime(Path path, int value);

	/// <summary>
	/// Open the requested file as a stream to read.
	/// </summary>
	/// <param name="path">The path.</param>
	IInputFile OpenRead(Path path);

	/// <summary>
	/// Open the requested file as a stream to write.
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="truncate">A value indicating if open should truncate file.</param>
	IOutputFile OpenWrite(Path path, bool truncate);

	/// <summary>
	/// Rename the source file to the destination.
	/// </summary>
	/// <param name="source">The source path.</param>
	/// <param name="destination">The destination path.</param>
	void Rename(Path source, Path destination);

	/// <summary>
	/// Copy the source file to the destination.
	/// </summary>
	/// <param name="source">The source path.</param>
	/// <param name="destination">The destination path.</param>
	void CopyFile2(Path source, Path destination);

	/// <summary>
	/// Create the directory at the requested path.
	/// </summary>
	/// <param name="path">The path.</param>
	void CreateDirectory2(Path path);

	/// <summary>
	/// Get the children of a directory.
	/// </summary>
	/// <param name="path">The path.</param>
	IReadOnlyList<DirectoryEntry> GetChildren(Path path);

	/// <summary>
	/// Get the child directories of a directory.
	/// </summary>
	/// <param name="path">The path.</param>
	IReadOnlyList<DirectoryEntry> GetChildDirectories(Path path);

	/// <summary>
	/// Get the child files of a directory.
	/// </summary>
	/// <param name="path">The path.</param>
	IReadOnlyList<DirectoryEntry> GetChildFiles(Path path);

	/// <summary>
	/// Delete the directory.
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="recursive">A value indicating if the delete is recursive.</param>
	void DeleteDirectory(Path path, bool recursive);

	/// <summary>
	/// Delete the file.
	/// </summary>
	/// <param name="path">The path.</param>
	void DeleteFile(Path path);
}
