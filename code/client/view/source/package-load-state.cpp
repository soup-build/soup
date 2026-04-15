// <copyright file="package-load-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <memory>
#include <optional>
#include <vector>

export module Soup.View:PackageLoadState;

import Opal;
import Soup.Core;

using namespace Opal;

namespace Soup::View
{
	export struct PackageLoadState
	{
		std::optional<Core::GenerateResult> GeneratePhase1Result;
		std::optional<Core::OperationResults> EvaluatePhase1Results;
		std::optional<Core::ValueTable> GeneratePhase1Info;
		std::optional<Core::OperationGraph> GeneratePhase2Result;
		std::optional<Core::OperationResults> EvaluatePhase2Results;
		std::optional<Core::ValueTable> GeneratePhase2Info;
	};
	
	export PackageLoadState LoadPackage(
		Core::FileSystemState& fileSystemState,
		Core::PackageProvider& packageProvider,
		int graphId,
		int packageId)
	{
		auto result = PackageLoadState();

		// Get the target directory
		const auto& targetDirectory = packageProvider.GetTargetDirectory(graphId, packageId);

		auto soupTargetDirectory = targetDirectory + Path("./.soup/");

		auto generatePhase1ResultFile = soupTargetDirectory + Core::Build::Constants::GeneratePhase1ResultFileName();
		auto generatePhase1Result = Core::GenerateResult();
		if (Core::GenerateResultManager::TryLoadState(generatePhase1ResultFile, generatePhase1Result, fileSystemState))
		{
			result.GeneratePhase1Result = std::move(generatePhase1Result);
		}

		auto evaluatePhase1ResultsFile = soupTargetDirectory + Core::Build::Constants::EvaluatePhase1ResultsFileName();
		auto evaluatePhase1Results = Core::OperationResults();
		if (Core::OperationResultsManager::TryLoadState(
			evaluatePhase1ResultsFile,
			evaluatePhase1Results,
			fileSystemState))
		{
			result.EvaluatePhase1Results = evaluatePhase1Results;
		}

		auto generatePhase1InfoFile = soupTargetDirectory + Core::Build::Constants::GeneratePhase1InfoFileName();
		auto generatePhase1Info = Core::ValueTable();
		if (Core::ValueTableManager::TryLoadState(generatePhase1InfoFile, generatePhase1Info))
		{
			result.GeneratePhase1Info = generatePhase1Info;
		}

		// // Check for the optional evaluate graph if the initial phase was preprocessor
		// OperationGraph? generatePhase2Result = null;
		// OperationResults? evaluatePhase2Results = null;
		// ValueTable? generatePhase2Info = null;
		// if (generatePhase1Result.IsPreprocessor)
		// {
		// 	var generatePhase2ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase2ResultFileName;
		// 	if (OperationGraphManager.TryLoadState(
		// 		generatePhase2ResultFile, this.fileSystemState, out var loadGeneratePhase2Result))
		// 	{
		// 		generatePhase2Result = loadGeneratePhase2Result;
		// 	}

		// 	// Check for the optional phase2 results
		// 	var evaluateResultsFile = soupTargetDirectory + BuildConstants.EvaluatePhase2ResultsFileName;
		// 	if (OperationResultsManager.TryLoadState(
		// 		evaluateResultsFile, this.fileSystemState, out var loadEvaluatePreprocessorResults))
		// 	{
		// 		evaluatePhase2Results = loadEvaluatePreprocessorResults;
		// 	}

		// 	var generatePhase2InfoFile = soupTargetDirectory + BuildConstants.GeneratePhase2InfoFileName;
		// 	if (ValueTableManager.TryLoadState(generatePhase2InfoFile, out var generatePhase2InfoTable))
		// 	{
		// 		generatePhase2Info = generatePhase2InfoTable;
		// 	}
		// }

		return result;
	}
}
