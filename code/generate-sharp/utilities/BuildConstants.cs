// <copyright file="BuildConstants.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Path = Opal.Path;

namespace Soup.Build.Utilities;

/// <summary>
/// The constants used throughout the build
/// </summary>
public static class BuildConstants
{
	public static Path EvaluateBuildResultsFileName => new Path("./evaluate-build.bor");

	public static Path EvaluatePreprocessorResultsFileName => new Path("./evaluate-preprocessor.bor");

	public static Path GenerateInputFileName => new Path("./generate-input.bvt");

	public static Path GeneratePhase1InfoFileName => new Path("./generate-phase1-info.bvt");

	public static Path GeneratePhase1OperationResultFileName => new Path("./generate-phase1.bor");

	public static Path GeneratePhase1ResultFileName => new Path("./generate-phase1.bgr");

	public static Path GeneratePhase2InfoFileName => new Path("./generate-phase2-info.bvt");

	public static Path GeneratePhase2OperationResultFileName => new Path("./generate-phase2.bor");

	public static Path GeneratePhase2ResultFileName => new Path("./generate-phase2.bog");

	public static Path GenerateSharedStateFileName => new Path("./generate-shared-state.bvt");

	public static Path LocalUserConfigFileName => new Path("./LocalUserConfig.sml");

	public static Path PackageLockFileName => new Path("./PackageLock.sml");

	public static Path RecipeFileName => new Path("./Recipe.sml");

	public static Path SoupLocalStoreDirectory => new Path("./.soup/");

	public static Path SoupTargetDirectory => new Path("./.soup/");

	public static Path TemporaryFolderName => new Path("./temp/");
}