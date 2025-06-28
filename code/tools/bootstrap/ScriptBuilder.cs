// <copyright file="ScriptBuilder.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Path = Opal.Path;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Soup.Build.Utilities;
using Soup.Tools;

namespace Soup.Build.Bootstrap;

public class ScriptBuilder
{
	private readonly Path packageFolder;
	private readonly Path scriptPath;

	private readonly FileSystemState fileSystemState;

	public ScriptBuilder(Path packageFolder, Path scriptPath)
	{
		this.packageFolder = packageFolder;
		this.scriptPath = scriptPath;
		this.fileSystemState = new FileSystemState();
	}

	public async Task BuildScriptAsync()
	{
		var dependencyGraph = LoadDependencyGraph();

		using var file = LifetimeManager.Get<IFileSystem>().OpenWrite(this.scriptPath, true);

		using var writer = new StreamWriter(file.GetOutStream());

		await writer.WriteLineAsync("#!/bin/bash");
		await writer.WriteLineAsync();
		await writer.WriteLineAsync("# Stop on first error");
		await writer.WriteLineAsync("set -e");

		await writer.WriteLineAsync();
		await writer.WriteLineAsync("SCRIPTS_DIR=$(dirname \"$0\")");
		await writer.WriteLineAsync("ROOT_DIR=$SCRIPTS_DIR/../..");
		await writer.WriteLineAsync("USER_ROOT=/home/$USER");

		var knownPathManager = new KnownPathManager([
			(LifetimeManager.Get<IFileSystem>().GetCurrentDirectory().ToString(), "$ROOT_DIR/"),
			(LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory().ToString(), "$USER_ROOT/"),
		]);

		var walker = new PackageGraphWalker(dependencyGraph);
		foreach (var package in walker.WalkGraph())
		{
			var targetDirectory = dependencyGraph.GetPackageTargetDirectory(dependencyGraph.RootPackageGraphId, package.Id);
			await WritePackageBuildOperationsAsync(writer, knownPathManager, package, targetDirectory);
		}
	}

	private async Task WritePackageBuildOperationsAsync(
		StreamWriter writer,
		KnownPathManager knownPathManager,
		PackageInfo package,
		Path targetDirectory)
	{
		var operationGraph = LoadOperationGraph(targetDirectory);

		await writer.WriteLineAsync();
		await writer.WriteLineAsync($"# Setup {package.Name}");

		var resolveTargetDirectory = knownPathManager.ResolvePath(targetDirectory);
		await writer.WriteLineAsync($"echo mkdir -p \"{resolveTargetDirectory}\"");
		await writer.WriteLineAsync($"mkdir -p \"{resolveTargetDirectory}\"");

		await writer.WriteLineAsync();
		await writer.WriteLineAsync($"# Build {package.Name}");

		var walker = new OperationGraphWalker(operationGraph);
		foreach (var operation in walker.WalkGraph())
		{
			await writer.WriteLineAsync($"echo \"{operation.Title}\"");

			var command = knownPathManager.ResolvePath(operation.Command.Executable);
			var arguments = knownPathManager.ResolveValues(operation.Command.Arguments);
			if (command.EndsWith("/mkdir.exe"))
			{
				command = "mkdir";
				arguments = arguments.Prepend("-p").ToList();
			}
			else if (command.EndsWith("/copy.exe"))
			{
				command = "cp";
			}
			else if (command.EndsWith("/writefile.exe"))
			{
				command = "echo";
				var echoArguments = new List<string>(arguments.Skip(1));
				echoArguments.Add(">");
				echoArguments.Add(arguments[0]);
				arguments = echoArguments;
			}

			var argumentsString = string.Join(" ", arguments.Select(EscapeString));
			var resolvedWorkingDirectory = knownPathManager.ResolvePath(operation.Command.WorkingDirectory);
			await writer.WriteLineAsync($"(cd \"{resolvedWorkingDirectory}\" && \"{command}\" {argumentsString})");
		}
	}

	private static string EscapeString(string value)
	{
		return value.Replace("\n", "\\n").Replace("\r", "\\r");
	}

	private PackageProvider LoadDependencyGraph()
	{
		string soupRoot;
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			soupRoot = "C:/Program Files/SoupBuild/Soup/Soup/";
		}
		else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
		{
			soupRoot = "/home/mwasp/dev/repos/soup/out/run/";
		}
		else
		{
			throw new NotSupportedException("Unknown OS Platform");
		}

		// Run in the soup install folder to make the tool think it is there.
		var cacheCurrentDirectory = Directory.GetCurrentDirectory();
		var fullPackagePath = Path.Parse(cacheCurrentDirectory) + this.packageFolder;
		var globalParameters = new ValueTable()
		{
			{ "Flavor", new Value("Release") }
		};
		Directory.SetCurrentDirectory(soupRoot);
		var packageProvider = SoupTools.LoadBuildGraph(fullPackagePath, globalParameters);
		Directory.SetCurrentDirectory(cacheCurrentDirectory);

		return packageProvider;
	}

	private OperationGraph LoadOperationGraph(
		Path targetDirectory)
	{
		var soupTargetDirectory = targetDirectory + new Path("./.soup/");

		var generatePhase1ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase1ResultFileName;
		if (!GenerateResultManager.TryLoadState(
			generatePhase1ResultFile, this.fileSystemState, out var generatePhase1Result))
		{
			throw new InvalidOperationException($"Failed to load generate phase 1 result: {generatePhase1ResultFile}");
		}

		// Check for the optional evaluate graph if the initial phase was preprocessor
		if (generatePhase1Result.IsPreprocessor)
		{
			var generatePhase2ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase2ResultFileName;
			if (!OperationGraphManager.TryLoadState(
				generatePhase2ResultFile, this.fileSystemState, out var generatePhase2Result))
			{
				throw new InvalidOperationException($"Failed to load generate phase 2 result: {generatePhase2ResultFile}");
			}

			return generatePhase2Result;
		}
		else
		{
			return generatePhase1Result.EvaluateGraph;
		}
	}
}