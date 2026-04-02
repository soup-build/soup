// <copyright file="vs-where-utilities.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Path = Opal.Path;

namespace Soup.Build.Discover;

public static class VSWhereUtilities
{
	/// <summary>
	/// Attempt to find MSVC compiler installations
	/// </summary>
	public static async Task<List<(SemanticVersion Version, Path Path)>> TryFindMSVCInstallsAsync(bool includePrerelease)
	{
		// Find the location of the Windows SDK
		var visualStudioInstallRoots = await TryFindVSInstallRootsAsync(
			"Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
			includePrerelease);

		var result = new List<(SemanticVersion Version, Path Path)>();
		foreach (var visualStudioInstallRoot in visualStudioInstallRoots)
		{
			Log.HighPriority("Using VS Installation: " + visualStudioInstallRoot.ToString());

			// Use the default version
			var visualCompilerVersion = await FindDefaultVCToolsVersionAsync(visualStudioInstallRoot);
			Log.HighPriority("Using VC Version: " + visualCompilerVersion);

			// Calculate the final VC tools folder
			var visualCompilerVersionFolder =
				visualStudioInstallRoot + new Path($"./VC/Tools/MSVC/{visualCompilerVersion}/");

			result.Add((visualCompilerVersion, visualCompilerVersionFolder));
		}

		return result;
	}

	private static async Task<List<Path>> TryFindVSInstallRootsAsync(string requires, bool includePrerelease)
	{
		Log.HighPriority($"Discover VS Component: {requires}");

		// Find a copy of visual studio that has the required VisualCompiler
		var executablePath = new Path("C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe");
		var workingDirectory = new Path("./");
		var argumentList = new List<string>()
		{
			"-products",
			"*",
			"-requires",
			requires,
			"-property",
			"installationPath",
		};

		// Check if we should include pre-release versions
		if (includePrerelease)
		{
			argumentList.Add("-prerelease");
		}

		// Execute the requested target
		var arguments = CombineArguments(argumentList);
		Log.Info(executablePath.ToString() + " " + arguments);
		if (!LifetimeManager.Get<IFileSystem>().Exists(executablePath))
		{
			Log.HighPriority("VSWhere is not installed on the host machine");
			return [];
		}

		var process = LifetimeManager.Get<IProcessManager>().CreateProcess(
			executablePath.ToString(),
			arguments,
			workingDirectory);
		process.Start();
		await process.WaitForExitAsync();

		var stdOut = process.GetStandardOutput();
		var stdErr = process.GetStandardError();
		var exitCode = process.GetExitCode();

		if (!string.IsNullOrEmpty(stdErr))
		{
			Log.Error("VSWhere failed.");
			Log.Error(stdErr);
			throw new HandledException();
		}

		if (exitCode != 0)
		{
			// TODO: Return error code
			Log.Error("VSWhere failed.");
			throw new HandledException();
		}

		// The first line is the path
		using var reader = new System.IO.StringReader(stdOut);
		var line = await reader.ReadLineAsync();
		var result = new List<Path>();
		while (line is not null)
		{
			result.Add(Path.Parse($"{line}\\"));
			line = await reader.ReadLineAsync();
		}

		if (result.Count == 0)
		{
			Log.HighPriority("No results.");
		}

		return result;
	}

	private static async Task<SemanticVersion> FindDefaultVCToolsVersionAsync(
		Path visualStudioInstallRoot)
	{
		// Check the default tools version
		var visualCompilerToolsDefaultVersionFile =
			visualStudioInstallRoot + new Path("./VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt");
		if (!LifetimeManager.Get<IFileSystem>().Exists(visualCompilerToolsDefaultVersionFile))
		{
			Log.Error("VisualCompilerToolsDefaultVersionFile file does not exist: " + visualCompilerToolsDefaultVersionFile.ToString());
			throw new HandledException();
		}

		// Read the entire file into a string
		using var file = LifetimeManager.Get<IFileSystem>().OpenRead(visualCompilerToolsDefaultVersionFile);
		using var reader = new System.IO.StreamReader(file.GetInStream(), null, true, -1, true);
		// The first line is the version
		var version = await reader.ReadLineAsync();
		if (version is null)
		{
			Log.Error("Failed to parse version from file.");
			throw new HandledException();
		}

		return SemanticVersion.Parse(version);
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