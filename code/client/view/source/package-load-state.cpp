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
		bool generatePhase1IsPreprocessor = false;
		if (Core::GenerateResultManager::TryLoadState(generatePhase1ResultFile, generatePhase1Result, fileSystemState))
		{
			generatePhase1IsPreprocessor = generatePhase1Result.HasPreprocessor();
			result.GeneratePhase1Result = std::move(generatePhase1Result);
		}

		auto evaluatePhase1ResultsFile = soupTargetDirectory + Core::Build::Constants::EvaluatePhase1ResultsFileName();
		auto evaluatePhase1Results = Core::OperationResults();
		if (Core::OperationResultsManager::TryLoadState(
			evaluatePhase1ResultsFile,
			evaluatePhase1Results,
			fileSystemState))
		{
			result.EvaluatePhase1Results = std::move(evaluatePhase1Results);
		}

		auto generatePhase1InfoFile = soupTargetDirectory + Core::Build::Constants::GeneratePhase1InfoFileName();
		auto generatePhase1Info = Core::ValueTable();
		if (Core::ValueTableManager::TryLoadState(generatePhase1InfoFile, generatePhase1Info))
		{
			result.GeneratePhase1Info = std::move(generatePhase1Info);
		}

		// Check for the optional evaluate graph if the initial phase was preprocessor
		if (generatePhase1IsPreprocessor)
		{
			auto generatePhase2ResultFile = soupTargetDirectory + Core::Build::Constants::GeneratePhase2ResultFileName();
			auto generatePhase2Result = Core::OperationGraph();
			if (Core::OperationGraphManager::TryLoadState(
				generatePhase2ResultFile,
				generatePhase2Result,
				fileSystemState))
			{
				result.GeneratePhase2Result = std::move(generatePhase2Result);
			}

			// Check for the optional phase2 results
			auto evaluatePhase2ResultsFile = soupTargetDirectory + Core::Build::Constants::EvaluatePhase2ResultsFileName();
			auto evaluatePhase2Results = Core::OperationResults();
			if (Core::OperationResultsManager::TryLoadState(
				evaluatePhase2ResultsFile,
				evaluatePhase2Results,
				fileSystemState))
			{
				result.EvaluatePhase2Results = std::move(evaluatePhase2Results);
			}

			auto generatePhase2InfoFile = soupTargetDirectory + Core::Build::Constants::GeneratePhase2InfoFileName();
			auto generatePhase2Info = Core::ValueTable();
			if (Core::ValueTableManager::TryLoadState(generatePhase2InfoFile, generatePhase2Info))
			{
				result.GeneratePhase2Info = std::move(generatePhase2Info);
			}
		}

		return result;
	}
}
