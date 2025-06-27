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

		var walker = new PackageGraphWalker(dependencyGraph);
		foreach (var package in walker.WalkGraph())
		{
			var targetDirectory = dependencyGraph.GetPackageTargetDirectory(dependencyGraph.RootPackageGraphId, package.Id);
			await WritePackageBuildOperationsAsync(writer, package, targetDirectory);
		}
	}

	private async Task WritePackageBuildOperationsAsync(StreamWriter writer, PackageInfo package, Path targetDirectory)
	{
		var operationGraph = await LoadOperationGraphAsync(new Path(package.PackageRoot), package.Owner);

		await writer.WriteLineAsync();
		await writer.WriteLineAsync($"# Setup {package.Name}");

		await writer.WriteLineAsync($"echo mkdir -p \"{targetDirectory}\"");
		await writer.WriteLineAsync($"mkdir -p \"{targetDirectory}\"");

		await writer.WriteLineAsync();
		await writer.WriteLineAsync($"# Build {package.Name}");

		var walker = new OperationGraphWalker(operationGraph);
		foreach (var operation in walker.WalkGraph())
		{
			await writer.WriteLineAsync($"echo \"{operation.Title}\"");

			var command = operation.Command.Executable.ToString();
			var arguments = operation.Command.Arguments;
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
			await writer.WriteLineAsync($"(cd \"{operation.Command.WorkingDirectory}\" && \"{command}\" {argumentsString})");
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
		var globalParameters = new ValueTable();
		globalParameters.Add("Flavor", new Value("Release"));
		Directory.SetCurrentDirectory(soupRoot);
		var packageProvider = SoupTools.LoadBuildGraph(fullPackagePath, globalParameters);
		Directory.SetCurrentDirectory(cacheCurrentDirectory);

		return packageProvider;
	}

	private async Task<OperationGraph> LoadOperationGraphAsync(
		Path packageFolder, string? owner)
	{
		var targetPath = await GetTargetPathAsync(packageFolder, owner);

		var soupTargetDirectory = targetPath + new Path("./.soup/");

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

	private async Task<Path> GetTargetPathAsync(Path packageDirectory, string? owner)
	{
		string soupExe;
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			soupExe = "C:/Program Files/SoupBuild/Soup/Soup/Soup.exe";
		}
		else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
		{
			soupExe = "soup";
		}
		else
		{
			throw new NotSupportedException("Unknown OS Platform");
		}

		var optionalOwnerFlag = owner is null ? string.Empty : $" -owner {owner}";
		var processInfo = new ProcessStartInfo(soupExe, $"target {packageDirectory}{optionalOwnerFlag}")
		{
			RedirectStandardOutput = true,
			CreateNoWindow = true,
		};
		using var process = new Process()
		{
			StartInfo = processInfo,
		};

		_ = process.Start();

		await process.WaitForExitAsync();

		if (process.ExitCode != 0)
		{
			throw new InvalidOperationException($"Soup process exited with error: {process.ExitCode}");
		}

		var output = await process.StandardOutput.ReadToEndAsync();
		return new Path(output);
	}
}