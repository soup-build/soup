// <copyright file="package-load-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <optional>

export module Soup.BuildDatabase:PackageLoadState;

import Opal;
import Soup.Core;

using namespace Opal;
using namespace Soup::Core;

export std::optional<OperationGraph>
LoadPackage(FileSystemState &fileSystemState, PackageProvider &packageProvider,
			int graphId, int packageId) {
	// Get the target directory
	const auto &targetDirectory =
		packageProvider.GetTargetDirectory(graphId, packageId);

	auto soupTargetDirectory = targetDirectory + Path("./.soup/");

	auto generatePhase1ResultFile =
		soupTargetDirectory + Build::Constants::GeneratePhase1ResultFileName();
	auto generatePhase1Result = GenerateResult();
	bool generatePhase1IsPreprocessor = false;
	if (GenerateResultManager::TryLoadState(
			generatePhase1ResultFile, generatePhase1Result, fileSystemState)) {
		generatePhase1IsPreprocessor = generatePhase1Result.HasPreprocessor();
		if (generatePhase1IsPreprocessor) {
			// Check for the optional evaluate graph if the initial phase was
			// preprocessor
			auto generatePhase2ResultFile =
				soupTargetDirectory +
				Build::Constants::GeneratePhase2ResultFileName();
			auto generatePhase2Result = OperationGraph();
			if (OperationGraphManager::TryLoadState(generatePhase2ResultFile,
													generatePhase2Result,
													fileSystemState)) {
				return generatePhase2Result;
			}
		} else {
			return generatePhase1Result.GetGraph();
		}
	}

	return std::nullopt;
}
