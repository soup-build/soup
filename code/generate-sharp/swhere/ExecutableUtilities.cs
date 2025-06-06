﻿// <copyright file="ExecutableUtilities .cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Path = Opal.Path;

namespace Soup.Build.Discover;

public static class ExecutableUtilities
{
	public static async Task<string?> RunExecutableAsync(string executable, IList<string> arguments)
	{
		var workingDirectory = new Path("./");

		// Execute the requested target
		var argumentsValue = CombineArguments(arguments);
		Log.Info($"{executable} {argumentsValue}");

		var process = LifetimeManager.Get<IProcessManager>().CreateProcess(
			executable,
			argumentsValue,
			workingDirectory);
		process.Start();
		await process.WaitForExitAsync();

		var stdOut = process.GetStandardOutput();
		var stdErr = process.GetStandardError();
		var exitCode = process.GetExitCode();

		if (!string.IsNullOrEmpty(stdErr))
		{
			Log.HighPriority(stdErr);
		}

		if (exitCode != 0)
		{
			return null;
		}
		else
		{
			return stdOut;
		}
	}

	private static string CombineArguments(IList<string> args)
	{
		var argumentString = new StringBuilder();
		bool isFirst = true;
		foreach (var arg in args)
		{
			if (!isFirst)
				_ = argumentString.Append(' ');

			_ = argumentString.Append(arg);
			isFirst = false;
		}

		return argumentString.ToString();
	}
}