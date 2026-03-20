// <copyright file="build-evaluate-graph-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <algorithm>
#include <format>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module Soup.Core:BuildEvaluateGraphState;

import :OperationInfo;
import :OperationGraph;
import :OperationResult;
import :OperationResults;
import Opal;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The core runtime state for tracking evaluation runtime state for an individual graph
	/// </summary>
	export class BuildEvaluateGraphState
	{
	public:
		BuildEvaluateGraphState(
			const OperationGraph& operationGraph,
			OperationResults& operationResults,
			const Path& temporaryDirectory,
			const std::vector<Path>& globalAllowedReadAccess,
			const std::vector<Path>& globalAllowedWriteAccess) :
			OperationGraph(operationGraph),
			OperationResults(operationResults),
			TemporaryDirectory(temporaryDirectory),
			GlobalAllowedReadAccess(globalAllowedReadAccess),
			GlobalAllowedWriteAccess(globalAllowedWriteAccess),
			RemainingDependencyCounts(),
			ReadyOperations(),
			DidAnyEvaluate(false),
			LookupLoaded(false),
			InputFileLookup(),
			OutputFileLookup()
		{
		}

		const ::Soup::Core::OperationGraph& OperationGraph;
		::Soup::Core::OperationResults& OperationResults;

		const Path& TemporaryDirectory;

		const std::vector<Path>& GlobalAllowedReadAccess;
		const std::vector<Path>& GlobalAllowedWriteAccess;

		// Running State
		std::unordered_map<OperationId, int32_t> RemainingDependencyCounts;
		std::queue<OperationId> ReadyOperations;
		bool DidAnyEvaluate;

		bool LookupLoaded;
		std::unordered_map<FileId, std::set<OperationId>> InputFileLookup;
		std::unordered_map<FileId, OperationId> OutputFileLookup;

		void EnsureOperationLookupLoaded()
		{
			if (LookupLoaded)
				return;

			for (auto& operation : OperationGraph.GetOperations())
			{
				auto& operationInfo = operation.second;

				for (auto fileId : operationInfo.DeclaredInput)
				{
					auto findResult = InputFileLookup.find(fileId);
					if (findResult != InputFileLookup.end())
					{
						findResult->second.insert(operationInfo.Id);
					}
					else
					{
						auto [insertIterator, wasInserted] = InputFileLookup.emplace(
							fileId,
							std::set<OperationId>());
						insertIterator->second.insert(operationInfo.Id);
					}
				}

				for (auto fileId : operationInfo.DeclaredOutput)
				{
					OutputFileLookup.emplace(fileId, operationInfo.Id);
				}
			}

			LookupLoaded = true;
		}

		bool TryGetInputFileOperations(
			FileId fileId,
			const std::set<OperationId>*& result)
		{
			auto findResult = InputFileLookup.find(fileId);
			if (findResult != InputFileLookup.end())
			{
				result = &findResult->second;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool TryGetOutputFileOperation(
			FileId fileId,
			OperationId& result)
		{
			auto findResult = OutputFileLookup.find(fileId);
			if (findResult != OutputFileLookup.end())
			{
				result = findResult->second;
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}