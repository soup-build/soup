﻿// <copyright file="PackageManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright

using Opal;
using Opal.System;
using Soup.Build.Utilities;
using System;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using Path = Opal.Path;

namespace Soup.Build.PackageManager;

/// <summary>
/// The package manager
/// </summary>
[SuppressMessage("Naming", "CA1724:Type names should not match namespaces", Justification = "Primary class")]
public class PackageManager
{
	private static readonly Path StagingFolder = new Path("./.staging/");

	private readonly Uri apiEndpoint;

	private readonly HttpClient httpClient;

	private readonly IClosureManager closureManager;

	public PackageManager(
		Uri apiEndpoint,
		HttpClient httpClient,
		IClosureManager closureManager)
	{
		this.apiEndpoint = apiEndpoint;
		this.httpClient = httpClient;
		this.closureManager = closureManager;
	}

	/// <summary>
	/// Restore packages
	/// </summary>
	public async Task RestorePackagesAsync(Path workingDirectory)
	{
		var userProfileDirectory = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory();
		var packageStore = userProfileDirectory + new Path("./.soup/packages/");
		var lockStore = userProfileDirectory + new Path("./.soup/locks/");

		Log.Diag("Using Package Store: " + packageStore.ToString());
		Log.Diag("Using Lock Store: " + lockStore.ToString());

		// Create the staging directory
		var stagingPath = EnsureStagingDirectoryExists(packageStore);

		try
		{
			await this.closureManager.GenerateAndRestoreRecursiveLocksAsync(
				workingDirectory,
				packageStore,
				lockStore,
				stagingPath);

			// Cleanup the working directory
			Log.Diag("Deleting staging directory");
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingPath, true);
		}
		catch (Exception)
		{
			// Cleanup the staging directory and accept that we failed
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingPath, true);
			throw;
		}
	}

	/// <summary>
	/// Install a package
	/// </summary>
	public async Task InstallPackageReferenceAsync(Path workingDirectory, string packageReference)
	{
		var recipePath =
			workingDirectory +
			BuildConstants.RecipeFileName;
		var (isSuccess, recipe) = await RecipeExtensions.TryLoadRecipeFromFileAsync(recipePath);
		if (!isSuccess)
		{
			throw new InvalidOperationException($"Could not load the recipe file: {recipePath}");
		}

		var userProfileDirectory = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory();
		var packageStore = userProfileDirectory + new Path("./.soup/packages/");
		var lockStore = userProfileDirectory + new Path("./.soup/locks/");
		Log.Diag("Using Package Store: " + packageStore.ToString());
		Log.Diag("Using Lock Store: " + lockStore.ToString());

		// Parse the package reference to get the name
		var targetPackageReference = PackageReference.Parse(packageReference);
		string packageName = packageReference;
		if (!targetPackageReference.IsLocal)
		{
			packageName = targetPackageReference.Name;
		}

		// Check if the package is already installed
		var packageNameNormalized = packageName.ToUpperInvariant();
		if (recipe.HasRuntimeDependencies)
		{
			foreach (var dependency in recipe.RuntimeDependencies)
			{
				if (!dependency.IsLocal)
				{
					var dependencyNameNormalized = dependency.Name.ToUpperInvariant();
					if (dependencyNameNormalized == packageNameNormalized)
					{
						Log.Warning("Package already installed.");
						return;
					}
				}
			}
		}

		// Get the latest version if no version provided
		if (targetPackageReference.Version == null)
		{
			var ownerName = targetPackageReference.Owner ?? throw new InvalidOperationException("Owner was not set");
			var packageModel = await GetPackageModelAsync(recipe.Language.Name, ownerName, packageName);
			if (packageModel.Latest is null)
				throw new InvalidOperationException("Package did not have a latest version");
			var latestVersion = new SemanticVersion(
				packageModel.Latest.Major,
				packageModel.Latest.Minor,
				packageModel.Latest.Patch);
			Log.HighPriority("Latest Version: " + latestVersion.ToString());
			targetPackageReference = new PackageReference(null, packageModel.Owner, packageModel.Name, latestVersion);
		}

		if (targetPackageReference.Version == null)
			throw new InvalidOperationException("Target package version was null");

		// Register the package in the recipe
		Log.Info("Adding reference to recipe");
		recipe.AddRuntimeDependency(targetPackageReference.ToString());

		// Save the state of the recipe
		await RecipeExtensions.SaveToFileAsync(recipePath, recipe);

		// Create the staging directory
		var stagingDirectory = EnsureStagingDirectoryExists(packageStore);

		try
		{
			await this.closureManager.GenerateAndRestoreRecursiveLocksAsync(
				workingDirectory,
				packageStore,
				lockStore,
				stagingDirectory);

			// Cleanup the working directory
			Log.Info("Deleting staging directory");
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingDirectory, true);
		}
		catch (Exception)
		{
			// Cleanup the staging directory and accept that we failed
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingDirectory, true);
			throw;
		}
	}

	/// <summary>
	/// Publish a package
	/// </summary>
	[SuppressMessage("Style", "IDE0010:Add missing cases", Justification = "Allow default fallthrough")]
	public async Task PublishPackageAsync(Path workingDirectory)
	{
		Log.Info($"Publish Project: {workingDirectory}");

		var recipePath =
			workingDirectory +
			BuildConstants.RecipeFileName;
		var (isSuccess, recipe) = await RecipeExtensions.TryLoadRecipeFromFileAsync(recipePath);
		if (!isSuccess)
		{
			throw new InvalidOperationException($"Could not load the recipe file: {recipePath}");
		}

		var packageStore = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory() +
			new Path("./.soup/packages/");
		Log.Info("Using Package Store: " + packageStore.ToString());

		// Create the staging directory
		var stagingPath = EnsureStagingDirectoryExists(packageStore);

		try
		{
			var archivePath = stagingPath + new Path($"./{recipe.Name}.zip");

			// Create the archive of the package
			using (var zipArchive = LifetimeManager.Get<IZipManager>().OpenCreate(archivePath))
			{
				AddPackageFiles(workingDirectory, zipArchive);
			}

			// Authenticate the user
			Log.Info("Request Authentication Token");
			var accessToken = await LifetimeManager.Get<IAuthenticationManager>().EnsureSignInAsync();
			var ownerName = "_";

			// Publish the archive
			Log.Info("Publish package");
			var packageClient = new Api.Client.PackagesClient(this.httpClient, accessToken)
			{
				BaseUrl = this.apiEndpoint,
			};

			// Check if the package exists
			bool packageExists = false;
			try
			{
				var package = await packageClient.GetPackageAsync(recipe.Language.Name, ownerName, recipe.Name);
				packageExists = true;
			}
			catch (Api.Client.ApiException ex)
			{
				if (ex.StatusCode == HttpStatusCode.NotFound)
				{
					Log.Info("Package does not exist");
					packageExists = false;
				}
				else
				{
					throw;
				}
			}

			// Create the package if it does not exist
			if (!packageExists)
			{
				Log.Info("Creating package");
				var createPackageModel = new Api.Client.PackageCreateOrUpdateModel()
				{
					Description = string.Empty,
				};
				_ = await packageClient.CreateOrUpdatePackageAsync(
					recipe.Language.Name,
					ownerName,
					recipe.Name,
					createPackageModel);
			}

			var packageVersionClient = new Api.Client.PackageVersionsClient(this.httpClient, accessToken)
			{
				BaseUrl = this.apiEndpoint,
			};

			using (var readArchiveFile = LifetimeManager.Get<IFileSystem>().OpenRead(archivePath))
			{
				try
				{
					await packageVersionClient.PublishPackageVersionAsync(
						recipe.Language.Name,
						ownerName,
						recipe.Name,
						recipe.Version.ToString(),
						new Api.Client.FileParameter(readArchiveFile.GetInStream(), string.Empty, "application/zip"));
					Log.Info("Package published");
				}
				catch (Api.Client.ApiException ex)
				{
					switch (ex.StatusCode)
					{
						case HttpStatusCode.BadRequest:
							if (ex is Api.Client.ApiException<Api.Client.ProblemDetails> problemDetailsEx)
								Log.Error(problemDetailsEx.Result.Detail ?? "Bad request");
							else
								Log.Error("Bad request");
							break;
						case HttpStatusCode.Forbidden:
							Log.Error("You do not have permission to edit this package");
							break;
						case HttpStatusCode.Conflict:
							Log.Info("Package version already exists");
							break;
						default:
							throw;
					}
				}
			}

			// Cleanup the staging directory
			Log.Info("Cleanup staging directory");
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingPath, true);
		}
		catch (Exception)
		{
			// Cleanup the staging directory and accept that we failed
			Log.Info("Publish Failed: Cleanup staging directory");
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingPath, true);
			throw;
		}
	}

	private async Task<Api.Client.PackageModel> GetPackageModelAsync(
		string languageName,
		string ownerName,
		string packageName)
	{
		var client = new Api.Client.PackagesClient(this.httpClient, null)
		{
			BaseUrl = this.apiEndpoint,
		};

		return await client.GetPackageAsync(languageName, ownerName, packageName);
	}

	private static void AddPackageFiles(Path workingDirectory, IZipArchive archive)
	{
		var ignoreFileList = new string[]
		{
				"PackageLock.sml",
		};
		var ignoreFolderList = new string[]
		{
				"out",
				".git",
		};
		foreach (var child in LifetimeManager.Get<IFileSystem>().GetChildren(workingDirectory))
		{
			if (child.IsDirectory)
			{
				// Ignore undesirables
				if (!ignoreFolderList.Contains(child.Path.FileName))
				{
					AddAllFilesRecursive(child.Path, workingDirectory, archive);
				}
			}
			else
			{
				// Ignore undesirables
				if (!ignoreFileList.Contains(child.Path.FileName))
				{
					var relativePath = child.Path.GetRelativeTo(workingDirectory);
					var relativeName = relativePath.ToString()[2..];
					archive.CreateEntryFromFile(child.Path, relativeName);
				}
			}
		}
	}

	private static void AddAllFilesRecursive(Path directory, Path workingDirectory, IZipArchive archive)
	{
		foreach (var child in LifetimeManager.Get<IFileSystem>().GetChildren(directory))
		{
			if (child.IsDirectory)
			{
				AddAllFilesRecursive(child.Path, workingDirectory, archive);
			}
			else
			{
				var relativePath = child.Path.GetRelativeTo(workingDirectory);
				var relativeName = relativePath.ToString()[2..];
				archive.CreateEntryFromFile(child.Path, relativeName);
			}
		}
	}

	/// <summary>
	/// Ensure the staging directory exists
	/// </summary>
	private static Path EnsureStagingDirectoryExists(Path packageStore)
	{
		var stagingDirectory = packageStore + StagingFolder;
		if (LifetimeManager.Get<IFileSystem>().Exists(stagingDirectory))
		{
			Log.Warning("The staging directory already exists from a previous failed run");
			LifetimeManager.Get<IFileSystem>().DeleteDirectory(stagingDirectory, true);
		}

		// Create the folder
		LifetimeManager.Get<IFileSystem>().CreateDirectory2(stagingDirectory);

		return stagingDirectory;
	}
}