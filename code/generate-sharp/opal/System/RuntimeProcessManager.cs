﻿// <copyright file="RuntimeProcessManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;

namespace Opal.System;

/// <summary>
/// A windows platform specific process executable using system
/// </summary>
public class RuntimeProcessManager : IProcessManager
{
	/// <summary>
	/// Initializes a new instance of the <see cref='RuntimeProcessManager'/> class.
	/// </summary>
	public RuntimeProcessManager()
	{
	}

	/// <summary>
	/// Gets the current process file name
	/// </summary>
	public Path GetCurrentProcessFileName()
	{
		return new Path(AppContext.BaseDirectory);
	}

	/// <summary>
	/// Creates a process for the provided executable path
	/// </summary>
	public IProcess CreateProcess(
		string executable,
		string arguments,
		Path workingDirectory)
	{
		return new RuntimeProcess(
			executable,
			arguments,
			workingDirectory);
	}
}