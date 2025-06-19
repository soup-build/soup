// <copyright file="ScriptBuilder.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Path = Opal.Path;
using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Soup.Build.Utilities;

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
		var (recipe, operationGraph) = await LoadOperationGraphAsync(this.packageFolder, null);

		using var file = LifetimeManager.Get<IFileSystem>().OpenWrite(this.scriptPath, true);

		using var writer = new StreamWriter(file.GetOutStream());

		await writer.WriteLineAsync("#!/bin/bash");
		await writer.WriteLineAsync();
		await writer.WriteLineAsync("# Stop on first error");
		await writer.WriteLineAsync("set - e");
		await writer.WriteLineAsync();

		await writer.WriteLineAsync($"# Build {recipe.Name}");

		var walker = new OperationGraphWalker(operationGraph);
		foreach (var operation in walker.WalkGraph())
		{
			await writer.WriteLineAsync($"echo \"{operation.Title}\"");
			var arguments = string.Join(" ", operation.Command.Arguments);
			await writer.WriteLineAsync($"(cd \"{operation.Command.WorkingDirectory}\" && \"{operation.Command.Executable}\" {arguments})");
		}
	}

	private async Task<(Recipe Recipe, OperationGraph OperationGraph)> LoadOperationGraphAsync(
		Path packageFolder, string? owner)
	{
		var recipeFile = packageFolder + BuildConstants.RecipeFileName;
		var (loadRecipeResult, recipe) = await RecipeExtensions.TryLoadRecipeFromFileAsync(recipeFile);
		if (!loadRecipeResult)
		{
			throw new InvalidOperationException($"Failed to load Recipe file: {packageFolder}");
		}

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

			return (recipe, generatePhase2Result);
		}
		else
		{
			return (recipe, generatePhase1Result.EvaluateGraph);
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