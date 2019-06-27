﻿// <copyright file="RecipeBuildGenerator.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "RecipeExtensions.h"

namespace Soup
{
    /// <summary>
    /// The recipe build generator that knows how to build a recipe 
    /// and all of its dependencies
    /// </summary>
    export class RecipeBuildGenerator
    {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="RecipeBuildGenerator"/> class.
        /// </summary>
        RecipeBuildGenerator(std::shared_ptr<ICompiler> compiler) :
            _compiler(std::move(compiler))
        {
        }

        /// <summary>
        /// The Core Execute task
        /// </summary>
        void Execute(const Path& workingDirectory, const Recipe& recipe, bool force)
        {
            // Enable log event ids to track individual builds
            int projectId = 1;
            Log::EnsureListener().SetShowEventId(true);

            // TODO: A scoped cleanup would be nice
            try
            {
                projectId = BuildAllDependenciesRecursively(projectId, workingDirectory, recipe, force);
                CoreBuild(projectId, workingDirectory, recipe, force);

                Log::EnsureListener().SetShowEventId(false);
            }
            catch(...)
            {
                Log::EnsureListener().SetShowEventId(false);
            }
        }

    private:
        /// <summary>
        /// Build the dependecies for the provided recipe recursively
        /// </summary>
        int BuildAllDependenciesRecursively(
            int projectId,
            const Path& workingDirectory,
            const Recipe& recipe,
            bool force)
        {
            for (auto dependecy : recipe.GetDependencies())
            {
                // Load this package recipe
                auto packagePath = dependecy.GetPath();
                auto packageRecipePath = packagePath + Path(Constants::RecipeFileName);
                Recipe dependecyRecipe = {};
                if (!RecipeExtensions::TryLoadFromFile(packageRecipePath, dependecyRecipe))
                {
                    Log::Error("Failed to load the dependency package: {packagePath}");
                    throw std::runtime_error("Failed to load dependency.");
                }

                // Build all recursive dependencies
                projectId = BuildAllDependenciesRecursively(projectId, packagePath, dependecyRecipe, force);

                // Build this dependecy
                CoreBuild(projectId, packagePath, dependecyRecipe, force);

                // Move to the next build project id
                projectId++;
            }

            // Return the updated project id after building all dependencies
            return projectId;
        }

        /// <summary>
        /// The Core Execute task
        /// </summary>
        void CoreBuild(int projectId, const Path& workingDirectory, const Recipe& recipe, bool force)
        {
            Log::SetActiveId(projectId);
            Log::Info("Building '" + recipe.GetName() + "'");

            // Initialize the required target paths
            auto compilerFolder = Path(_compiler->GetName());
            auto buildPath =
                Path(Constants::ProjectGenerateFolderName) +
                Path(Constants::StoreBuildFolderName);
            auto outputDirectory = buildPath + Path("out");
            auto objectDirectory = outputDirectory + Path("obj") + compilerFolder;
            auto binaryDirectory = outputDirectory + Path("bin") + compilerFolder;

            // Determine the include paths
            // var folderWithHeadersSet = Directory.EnumerateFiles(path, "*.h", SearchOption.AllDirectories).Select(file => Path.GetDirectoryName(file)).ToHashSet();
            // var uniqueFolders = folderWithHeadersSet.ToList();

            // Build up arguments to build this individual recipe
            auto arguments = BuildArguments();
            arguments.Target = BuildTargetType::Executable;
            arguments.WorkingDirectory = workingDirectory;
            arguments.ObjectDirectory = objectDirectory;
            arguments.BinaryDirectory = binaryDirectory;
            arguments.ModuleSourceFile = Path(recipe.GetPublic());
            arguments.SourceFiles = std::vector<Path>({});
            arguments.IncludeDirectories = std::vector<Path>({});
            arguments.IncludeModules = std::vector<Path>({});
            arguments.IsIncremental = true;

            // Perform the build
            auto buildEngine = BuildEngine(_compiler);
            buildEngine.Execute(arguments);
        }

        // /// <summary>
        // /// Compile the module file
        // /// </summary>
        // private async Task<bool> CheckCompileModuleAsync(
        //     string path,
        //     Recipe recipe,
        //     BuildState buildState,
        //     IList<string> uniqueFolders,
        //     string objectDirectory,
        //     string binaryDirectory,
        //     bool force)
        // {
        //     var outputFilename = $"{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ModuleFileExtension}";
        //     var outputFile = Path.Combine(objectDirectory, outputFilename);
        //     bool requiresBuild = true;
        //     if (!force)
        //     {
        //         // Add all of the direct dependencies as module references
        //         var modules = new List<string>();
        //         var defines = new List<string>();
        //         await BuildDependencyModuleReferences(path, binaryDirectory, recipe, modules, defines);

        //         // The dependencies for this file are all of the direct module references
        //         var dependencies = new List<string>();
        //         dependencies.AddRange(modules);

        //         if (!BuildRequiredChecker.IsSourceFileOutdated(path, buildState, outputFile, recipe.Public, dependencies))
        //         {
        //             // TODO : This is a hack. We need to actually look through all of the imports for the module file
        //             Log.Info("Module file is up to date.");
        //             requiresBuild = false;
        //         }
        //     }

        //     if (requiresBuild)
        //     {
        //         await CompileModuleAsync(
        //             path,
        //             recipe,
        //             buildState,
        //             uniqueFolders,
        //             objectDirectory,
        //             binaryDirectory);
        //     }

        //     return requiresBuild;
        // }

        // /// <summary>
        // /// Compile the module file
        // /// </summary>
        // private async Task CompileModuleAsync(
        //     string path,
        //     Recipe recipe,
        //     BuildState buildState,
        //     IList<string> uniqueFolders,
        //     string objectDirectory,
        //     string binaryDirectory)
        // {
        //     Log.Info("Compile Module");

        //     if (string.IsNullOrEmpty(recipe.Public))
        //     {
        //         throw new InvalidOperationException("The public file was not set.");
        //     }

        //     var modules = new List<string>();
        //     var defines = new List<string>();

        //     defines.Add("SOUP_BUILD");

        //     // Set the active version namespace
        //     defines.Add(BuildRecipeNamespaceDefine(recipe));

        //     // Add all of the direct dependencies as module references
        //     // and set their version defintions
        //     await BuildDependencyModuleReferences(path, binaryDirectory, recipe, modules, defines);

        //     var args = new CompileArguments()
        //     {
        //         Standard = Compiler.LanguageStandard.Latest,
        //         RootDirectory = path,
        //         OutputDirectory = objectDirectory,
        //         PreprocessorDefinitions = defines,
        //         SourceFiles = new List<string>() { recipe.Public },
        //         IncludeDirectories = uniqueFolders,
        //         Modules = modules,
        //         ExportModule = true,
        //         GenerateIncludeTree = true,
        //     };

        //     // Ensure the object directory exists
        //     var objectDirectry = Path.Combine(args.RootDirectory, objectDirectory);
        //     if (!Directory.Exists(objectDirectry))
        //     {
        //         Directory.CreateDirectory(objectDirectry);
        //     }

        //     // Compile each file
        //     var result = await _compiler.CompileAsync(args);

        //     // Save the build state
        //     if (result.HeaderIncludeFiles != null)
        //         buildState.UpdateIncludeTree(result.HeaderIncludeFiles);
        // }

        // /// <summary>
        // /// Compile the supporting source files
        // /// </summary>
        // private async Task<bool> CheckCompileSourceAsync(
        //     string path,
        //     Recipe recipe,
        //     BuildState buildState,
        //     IList<string> uniqueFolders,
        //     string objectDirectory,
        //     string binaryDirectory,
        //     bool force)
        // {
        //     var modules = new List<string>();
        //     var defines = new List<string>();

        //     defines.Add("SOUP_BUILD");
        //     if (recipe.Type == RecipeType.Library)
        //     {
        //         // Add a reference to our own modules interface definition
        //         var modulePath = Path.Combine(
        //             objectDirectory,
        //             $"{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ModuleFileExtension}");
        //         modules.Add(modulePath);
        //         defines.Add(BuildRecipeNamespaceDefine(recipe));
        //     }

        //     // Add all of the direct dependencies as module references
        //     // and set their version defintions
        //     await BuildDependencyModuleReferences(path, binaryDirectory, recipe, modules, defines);

        //     var source = new List<string>();

        //     // All files are dependent on the parent module and all referenced modules
        //     var sharedDependecies = new List<string>();
        //     sharedDependecies.AddRange(modules);

        //     // Check if the precompiled module should be compiled
        //     if (recipe.Type == RecipeType.Library)
        //     {
        //         // Add the precompile module to the list of shared dependencies
        //         // TODO: Could optimize this to not do file datetime checks over again
        //         var moduleFile = Path.Combine(path, objectDirectory, $"{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ModuleFileExtension}");
        //         sharedDependecies.Add(moduleFile);

        //         var moduleOutputFile = Path.Combine(path, objectDirectory, $"{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ObjectFileExtension}");
        //         if (force || BuildRequiredChecker.IsOutdated(path, moduleOutputFile, sharedDependecies))
        //         {
        //             source.Add(moduleFile);
        //         }
        //     }

        //     // Check if each source file is out of date and requires a rebuild
        //     foreach (var sourceFile in recipe.Source)
        //     {
        //         var outputFile = Path.Combine(objectDirectory, $"{Path.GetFileNameWithoutExtension(sourceFile)}.{_compiler.ObjectFileExtension}");
        //         if (force || BuildRequiredChecker.IsSourceFileOutdated(path, buildState, outputFile, sourceFile, sharedDependecies))
        //         {
        //             source.Add(sourceFile);
        //         }
        //     }

        //     if (source.Count == 0)
        //     {
        //         Log.Info("All source is up to date.");
        //         return false;
        //     }
        //     else
        //     {
        //         Log.Info("Compile Source");
        //         var args = new CompileArguments()
        //         {
        //             Standard = Compiler.LanguageStandard.Latest,
        //             RootDirectory = path,
        //             OutputDirectory = objectDirectory,
        //             PreprocessorDefinitions = defines,
        //             SourceFiles = source,
        //             IncludeDirectories = uniqueFolders,
        //             Modules = modules,
        //             GenerateIncludeTree = true,
        //         };

        //         // Ensure the object directory exists
        //         var objectDirectry = Path.Combine(args.RootDirectory, objectDirectory);
        //         if (!Directory.Exists(objectDirectry))
        //         {
        //             Directory.CreateDirectory(objectDirectry);
        //         }


        //         // Compile each file
        //         var result = await _compiler.CompileAsync(args);

        //         // Save the build state
        //         if (result.HeaderIncludeFiles != null)
        //             buildState.UpdateIncludeTree(result.HeaderIncludeFiles);

        //         return true;
        //     }
        // }

        // private async Task BuildDependencyModuleReferences(
        //     string path,
        //     string binaryDirectory,
        //     Recipe recipe,
        //     IList<string> modules,
        //     IList<string> defines)
        // {
        //     foreach (var dependecy in recipe.Dependencies)
        //     {
        //         // Load this package recipe
        //         var packagePath = VerifyDependencyPath(path, dependecy);
        //         var dependecyRecipe = await RecipeManager.LoadFromFileAsync(packagePath);

        //         modules.Add(Path.Combine(packagePath, binaryDirectory, BuildRecipeModuleFilename(dependecyRecipe)));
        //         defines.Add(BuildRecipeNamespaceDefine(dependecyRecipe));
        //     }
        // }

        // /// <summary>
        // /// Link the resulting object files
        // /// </summary>
        // private async Task CheckLinkLibraryAsync(
        //     string path,
        //     Recipe recipe,
        //     string objectDirectory,
        //     string binaryDirectory,
        //     bool force)
        // {
        //     var allFiles = new List<string>(recipe.Source);
        //     if (recipe.Type == RecipeType.Library)
        //     {
        //         allFiles.Add(recipe.Public);
        //     }

        //     // Convert all source files into objects
        //     var objectFiles = recipe.Source.Select(file => $"{objectDirectory.EnsureTrailingSlash()}{Path.GetFileNameWithoutExtension(file)}.{_compiler.ObjectFileExtension}").ToList();

        //     // Add the modules object too
        //     objectFiles.Add($"{objectDirectory.EnsureTrailingSlash()}{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ObjectFileExtension}");

        //     var targetLibraryFile = Path.Combine(binaryDirectory, $"{recipe.Name}.{_compiler.StaticLibraryFileExtension}");
        //     if (force || BuildRequiredChecker.IsOutdated(path, targetLibraryFile, objectFiles))
        //     {
        //         Log.Info("Link library");
        //         var args = new LinkerArguments()
        //         {
        //             Name = recipe.Name,
        //             RootDirectory = path,
        //             OutputDirectory = binaryDirectory,
        //             SourceFiles = objectFiles,
        //         };

        //         // Ensure the object directory exists
        //         var objectDirectry = Path.Combine(args.RootDirectory, binaryDirectory);
        //         if (!Directory.Exists(objectDirectry))
        //         {
        //             Directory.CreateDirectory(objectDirectry);
        //         }

        //         // Link
        //         await _compiler.LinkLibraryAsync(args);
        //     }
        //     else
        //     {
        //         Log.Info("Static library up to date.");
        //     }
        // }

        // /// <summary>
        // /// Link the executable
        // /// </summary>
        // private async Task LinkExecutableAsync(
        //     string path,
        //     Recipe recipe,
        //     string objectDirectory,
        //     string binaryDirectory)
        // {
        //     Log.Info("Link Executable");
        //     var allFiles = new List<string>(recipe.Source);
        //     if (recipe.Type == RecipeType.Library)
        //     {
        //         allFiles.Add(recipe.Public);
        //     }

        //     // Add all of the dependencies as module references
        //     var librarySet = new HashSet<string>();
        //     await GenerateDependencyLibrarySetAsync(path, binaryDirectory, recipe, librarySet);

        //     var objectFiles = recipe.Source.Select(file => $"{objectDirectory.EnsureTrailingSlash()}{Path.GetFileNameWithoutExtension(file)}.{_compiler.ObjectFileExtension}").ToList();
        //     var libraryFiles = librarySet.ToList();
        //     var args = new LinkerArguments()
        //     {
        //         Name = recipe.Name,
        //         RootDirectory = path,
        //         OutputDirectory = binaryDirectory,
        //         SourceFiles = objectFiles,
        //         LibraryFiles = libraryFiles,
        //     };

        //     // Ensure the object directory exists
        //     var objectDirectry = Path.Combine(args.RootDirectory, binaryDirectory);
        //     if (!Directory.Exists(objectDirectry))
        //     {
        //         Directory.CreateDirectory(objectDirectry);
        //     }

        //     // Link
        //     await _compiler.LinkExecutableAsync(args);
        // }

        // /// <summary>
        // /// Clone the module interface definition from the object directoy to the binary directory
        // /// </summary>
        // private void CloneModuleInterface(string path, Recipe recipe, string objectDirectory, string binaryDirectory)
        // {
        //     Log.Verbose("Clone Module Interface");
        //     var sourceModuleFile = Path.Combine(path, objectDirectory, $"{Path.GetFileNameWithoutExtension(recipe.Public)}.{_compiler.ModuleFileExtension}");
        //     var targetModuleFile = Path.Combine(path, binaryDirectory, BuildRecipeModuleFilename(recipe));

        //     // Ensure the object directory exists
        //     if (!File.Exists(sourceModuleFile))
        //     {
        //         throw new InvalidOperationException("The resulting module interface definition was missing.");
        //     }

        //     Log.Verbose($"Clone Module: {sourceModuleFile} -> {targetModuleFile}");
        //     File.Copy(sourceModuleFile, targetModuleFile, true);
        // }

        // private async Task GenerateDependencyLibrarySetAsync(
        //     string path,
        //     string binaryDirectory,
        //     Recipe recipe,
        //     HashSet<string> set)
        // {
        //     foreach (var dependecy in recipe.Dependencies)
        //     {
        //         // Load this package recipe
        //         var packagePath = VerifyDependencyPath(path, dependecy);
        //         var dependecyRecipe = await RecipeManager.LoadFromFileAsync(packagePath);

        //         // Get recursive dependencies
        //         await GenerateDependencyLibrarySetAsync(path, binaryDirectory, dependecyRecipe, set);

        //         set.Add(Path.Combine(packagePath, binaryDirectory, $"{dependecyRecipe.Name}.a").ToLower());
        //     }
        // }

        // private string VerifyDependencyPath(string path, PackageReference dependecy)
        // {
        //     string packagePath;
        //     if (dependecy.Path != null)
        //     {
        //         // Build the relative path
        //         Log.Verbose($"Local Dependecy: {dependecy.Path}");
        //         if (!Path.IsPathRooted(dependecy.Path))
        //         {
        //             packagePath = Path.GetFullPath(Path.Combine(path, dependecy.Path));
        //         }
        //         else
        //         {
        //             packagePath = dependecy.Path;
        //         }

        //         Log.Verbose($"Path: {packagePath}");

        //         // Verify the package exists
        //         if (!Directory.Exists(packagePath))
        //         {
        //             Log.Error($"The local package reference folder does not exist: {packagePath}");
        //             throw new InvalidOperationException();
        //         }
        //     }
        //     else
        //     {
        //         packagePath = PackageManager.BuildKitchenPackagePath(_config, dependecy);
        //     }

        //     return packagePath;
        // }

        // private string BuildRecipeNamespaceDefine(Recipe recipe)
        // {
        //     return $"{recipe.Name.Replace(".", "_")}_VersionNamespace={recipe.Name}::{GetNamespace(recipe.Version)}";
        // }

        // private string BuildRecipeModuleFilename(Recipe recipe)
        // {
        //     return $"{recipe.Name.Replace(".", "_")}.{_compiler.ModuleFileExtension}";
        // }

        // private string GetNamespace(SemanticVersion version)
        // {
        //     return $"v{version.Major}_{version.Minor}_{version.Patch}";
        // }

    private:
        std::shared_ptr<ICompiler> _compiler;
    };
}
