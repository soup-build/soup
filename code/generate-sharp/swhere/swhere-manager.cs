// <copyright file="swhere-manager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Soup.Build.Utilities;
using System;
using System.Collections.Generic;
using System.Runtime.Versioning;
using System.Threading.Tasks;
using Path = Opal.Path;

namespace Soup.Build.Discover;

public static class SwhereManager
{
	public static async Task DiscoverAsync(OSPlatform platform, bool includePrerelease)
	{
		// Load up the Local User Config
		var localUserConfigPath = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory() +
			new Path("./.soup/local-user-config.sml");
		var (loadConfigResult, userConfig) =
			await LocalUserConfigExtensions.TryLoadLocalUserConfigFromFileAsync(localUserConfigPath);
		if (!loadConfigResult)
		{
			Log.Info("No existing local user config.");
		}

		await DiscoverSharedPlatformAsync(platform, userConfig);
		switch (platform)
		{
			case OSPlatform.Windows:
#pragma warning disable CA1416 // Validate platform compatibility
				await DiscoverWindowsPlatformAsync(includePrerelease, userConfig);
#pragma warning restore CA1416 // Validate platform compatibility
				break;
			case OSPlatform.Linux:
				await DiscoverLinuxPlatformAsync(platform, userConfig);
				break;
			default:
				throw new InvalidOperationException("Unknown platform");
		}

		// Save the result
		await LocalUserConfigExtensions.SaveToFileAsync(localUserConfigPath, userConfig);

		// Create Root Recipe if missing
		var rootRecipePath = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory() +
			new Path($"./.soup/{BuildConstants.RootRecipeFileName}");
		Log.Info("Check Root Recipe: " + rootRecipePath.ToString());
		if (!LifetimeManager.Get<IFileSystem>().Exists(rootRecipePath))
		{
			Log.Info("Create Root Recipe");
			using var file = LifetimeManager.Get<IFileSystem>().OpenWrite(rootRecipePath, true);
			using var writer = new System.IO.StreamWriter(file.GetOutStream());
			await writer.WriteLineAsync("OutputRoot: './out/'");
		}
	}

	private static async Task DiscoverSharedPlatformAsync(OSPlatform platform, LocalUserConfig userConfig)
	{
		await DiscoverDotNetAsync(platform, userConfig);
		DiscoverNuget(userConfig);
	}

	[SupportedOSPlatform("windows")]
	private static async Task DiscoverWindowsPlatformAsync(bool includePrerelease, LocalUserConfig userConfig)
	{
		Log.HighPriority("Discover Windows Platform");

		var msvc = await VSWhereUtilities.TryFindMSVCInstallAsync(includePrerelease);
		if (msvc is not null)
		{
			var msvcSDK = userConfig.EnsureSDK("MSVC");
			msvcSDK.SourceDirectories =
			[
				msvc.Value.Path,
			];
			msvcSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "Version", msvc.Value.Version },
					{ "VCToolsRoot", msvc.Value.Path.ToString() },
				});
		}

		var windowsSDK = WindowsSDKUtilities.TryFindWindows10Kit();

		if (windowsSDK is not null)
		{
			var windowsSDKConfig = userConfig.EnsureSDK("Windows");
			windowsSDKConfig.SourceDirectories =
			[
				windowsSDK.Value.InstallPath,
			];
			windowsSDKConfig.SetProperties(
				new Dictionary<string, string>()
				{
					{ "Version", windowsSDK.Value.Version },
					{ "RootPath", windowsSDK.Value.InstallPath.ToString() },
				});
		}
		else
		{
			Log.Warning("No Windows SDKs installed");
		}

		var netFXToolsPath = WindowsSDKUtilities.TryFindNetFXTools();
		if (netFXToolsPath is not null)
		{
			var netFXToolsSDK = userConfig.EnsureSDK("NetFXTools");
			netFXToolsSDK.SourceDirectories =
			[
				netFXToolsPath,
			];
			netFXToolsSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "ToolsRoot", netFXToolsPath.ToString() },
				});
		}
		else
		{
			Log.Warning("No NetFx SDK installed");
		}
	}

	private static async Task DiscoverLinuxPlatformAsync(OSPlatform platform, LocalUserConfig userConfig)
	{
		await DiscoverGCCAsync(platform, userConfig);
		await DiscoverClangAsync(platform, userConfig);
	}

	private static async Task DiscoverGCCAsync(OSPlatform platform, LocalUserConfig userConfig)
	{
		Log.HighPriority("Discover GCC");

		// Find the GCC SDKs
		var cCompilerPath = await WhereIsUtilities.TryFindExecutableAsync(platform, "gcc");
		var cppCompilerPath = await WhereIsUtilities.TryFindExecutableAsync(platform, "g++");

		if (cCompilerPath is null || cppCompilerPath is null)
		{
			Log.HighPriority("GCC not installed");
		}
		else
		{
			var gccSDK = userConfig.EnsureSDK("GCC");
			gccSDK.SourceDirectories = [];
			gccSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "CCompiler", cCompilerPath.ToString() },
					{ "CppCompiler", cppCompilerPath.ToString() },
				});
		}
	}

	private static async Task DiscoverClangAsync(OSPlatform platform, LocalUserConfig userConfig)
	{
		Log.HighPriority("Discover Clang");

		// Find the Clang SDKs
		var cCompilerPath = await WhereIsUtilities.TryFindExecutableAsync(platform, "clang");
		var cppCompilerPath = await WhereIsUtilities.TryFindExecutableAsync(platform, "clang++");
		var archiverPath = await WhereIsUtilities.TryFindExecutableAsync(platform, "ar");

		if (cCompilerPath is null || cppCompilerPath is null || archiverPath is null)
		{
			Log.HighPriority("Clang not installed");
		}
		else
		{
			var clangSDK = userConfig.EnsureSDK("Clang");
			clangSDK.SourceDirectories = [];
			clangSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "CCompiler", cCompilerPath.ToString() },
					{ "CppCompiler", cppCompilerPath.ToString() },
					{ "Archiver", archiverPath.ToString() },
				});
		}
	}


	private static async Task DiscoverDotNetAsync(OSPlatform platform, LocalUserConfig userConfig)
	{
		Log.HighPriority("Discover DotNet");

		var dotnet = await DotNetSDKUtilities.TryFindDotNetAsync(platform);
		if (dotnet is not null)
		{
			var dotnetSDK = userConfig.EnsureSDK("DotNet");
			dotnetSDK.SourceDirectories = dotnet.Value.SourceDirectories;
			dotnetSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "DotNetExecutable", dotnet.Value.DotNetExecutable.ToString() },
				});

			var sdksTable = dotnetSDK.Properties.EnsureTableWithSyntax("SDKs", 3);
			foreach (var sdk in dotnet.Value.SDKVersions)
			{
				sdksTable.AddItemWithSyntax(sdk.Version, sdk.InstallDirectory.ToString(), 4);
			}

			var runtimesTable = dotnetSDK.Properties.EnsureTableWithSyntax("Runtimes", 3);
			foreach (var runtime in dotnet.Value.Runtimes)
			{
				var runtimeTable = runtimesTable.EnsureTableWithSyntax(runtime.Key, 4);
				foreach (var runtimeVersion in runtime.Value)
				{
					runtimeTable.AddItemWithSyntax(runtimeVersion.Version, runtimeVersion.InstallDirectory.ToString(), 5);
				}
			}

			var packsTable = dotnetSDK.Properties.EnsureTableWithSyntax("TargetingPacks", 3);
			foreach (var pack in dotnet.Value.TargetingPacks)
			{
				var packTable = packsTable.EnsureTableWithSyntax(pack.Key, 4);
				foreach (var packVersion in pack.Value)
				{
					var packVersionTable = packTable.AddTableWithSyntax(packVersion.Version, 5);
					packVersionTable.AddItemWithSyntax("Path", packVersion.InstallDirectory.ToString(), 6);
					var analyzerArray = packVersionTable.AddArrayWithSyntax("Analyzer", 6);
					var managedArray = packVersionTable.AddArrayWithSyntax("Managed", 6);
					if (packVersion.FrameworkList is not null)
					{
						foreach (var file in packVersion.FrameworkList.Files)
						{
							switch (file.Type)
							{
								case "Analyzer":
									analyzerArray.AddItemWithSyntax(file.Path.ToString(), 7);
									break;
								case "Managed":
									managedArray.AddItemWithSyntax(file.Path.ToString(), 7);
									break;
								default:
									Log.Warning($"Unknown File Type: {file.Type}");
									break;
							}
						}
					}
				}
			}
		}
	}

	private static void DiscoverNuget(LocalUserConfig userConfig)
	{
		Log.HighPriority("Discover Nuget");

		var (hasNuget, nugetPackagesPath, nugetPackages) = NugetSDKUtilities.FindNugetPackages();
		if (hasNuget)
		{
			var nugetSDK = userConfig.EnsureSDK("Nuget");
			nugetSDK.SourceDirectories =
			[
				nugetPackagesPath,
			];

			nugetSDK.SetProperties(
				new Dictionary<string, string>()
				{
					{ "PackagesDirectory", nugetPackagesPath.ToString() },
				});

			var packagesTable = nugetSDK.Properties.EnsureTableWithSyntax("Packages", 3);
			packagesTable.Values.Clear();
			foreach (var package in nugetPackages)
			{
				var packageTable = packagesTable.EnsureTableWithSyntax(package.Id, 4);
				foreach (var packageVersion in package.Versions)
				{
					var packageVersionTable = packageTable.EnsureTableWithSyntax(packageVersion.Version, 5);
					if (packageVersion.TargetFrameworks.Count > 0)
					{
						var targetFrameworksTable = packageVersionTable.EnsureTableWithSyntax("TargetFrameworks", 6);
						foreach (var targetFramework in packageVersion.TargetFrameworks)
						{
							var targetFrameworkTable = targetFrameworksTable.EnsureTableWithSyntax(targetFramework.Name, 7);

							if (targetFramework.Dependencies.Count > 0)
							{
								var dependenciesArray = targetFrameworkTable.EnsureArrayWithSyntax("Dependencies", 8);
								foreach (var dependency in targetFramework.Dependencies)
								{
									var dependencyTable = dependenciesArray.AddInlineTableWithSyntax(9);
									dependencyTable.AddInlineItemWithSyntax("Id", dependency.Id);
									dependencyTable.AddInlineItemWithSyntax("Version", dependency.Version);
								}
							}

							if (targetFramework.Libraries.Count > 0)
							{
								var librariesArray = targetFrameworkTable.EnsureArrayWithSyntax("Libraries", 8);
								foreach (var library in targetFramework.Libraries)
								{
									librariesArray.AddItemWithSyntax(library, 9);
								}
							}
						}
					}
				}
			}
		}
	}
}