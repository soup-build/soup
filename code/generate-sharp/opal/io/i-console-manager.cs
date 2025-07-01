// <copyright file="i-console-manager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Opal.IO;

/// <summary>
/// The input manager interface
/// Interface mainly used to allow for unit testing client code.
/// </summary>
public interface IConsoleManager
{
	/// <summary>
	/// Get the standard input stream.
	/// </summary>
	IConsoleInput GetStandardInput();
}
