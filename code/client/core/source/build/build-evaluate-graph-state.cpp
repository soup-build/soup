// <copyright file="build-evaluate-graph-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <algorithm>
#include <condition_variable>
#include <format>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module Soup.Core:BuildEvaluateGraphState;

import :FileSystemState;
import :OperationInfo;
import :OperationGraph;
import :OperationResult;
import :OperationResults;
import Opal;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// Immutable operation state information used during evaluation
	/// </summary>
	export struct CurrentOperationState
	{
		OperationInfo& Info;
		OperationResult* PreviousResult;

		CurrentOperationState& operator=(CurrentOperationState other)
		{
			Info = other.Info;
			PreviousResult = other.PreviousResult;
			return *this;
		}
	};

	/// <summary>
	/// The core runtime state for tracking evaluation runtime state for an individual graph
	/// </summary>
	export class BuildEvaluateGraphState
	{
	private:
		std::mutex _mutex;
		bool _isComplete;
		size_t _activeOperations;
		std::condition_variable _conditionalVariable;
		std::exception_ptr _error;

		::Soup::Core::OperationResults& Results;

		// Running State
		std::unordered_map<OperationId, int32_t> RemainingDependencyCounts;
		std::queue<OperationId> ReadyOperations;
		bool _didAnyEvaluate;

		std::unordered_map<FileId, std::set<OperationId>> InputFileLookup;
		std::unordered_map<FileId, OperationId> OutputFileLookup;

	public:
		// TODO: Verify operation graph is an immutable initial state
		::Soup::Core::OperationGraph& Graph;
		const Path& TemporaryDirectory;
		const std::vector<Path>& GlobalAllowedReadAccess;
		const std::vector<Path>& GlobalAllowedWriteAccess;

	public:
		BuildEvaluateGraphState(
			OperationGraph& graph,
			OperationResults& results,
			const Path& temporaryDirectory,
			const std::vector<Path>& globalAllowedReadAccess,
			const std::vector<Path>& globalAllowedWriteAccess) :
			_mutex(),
			_isComplete(false),
			_activeOperations(0),
			_conditionalVariable(),
			Graph(graph),
			Results(results),
			TemporaryDirectory(temporaryDirectory),
			GlobalAllowedReadAccess(globalAllowedReadAccess),
			GlobalAllowedWriteAccess(globalAllowedWriteAccess),
			RemainingDependencyCounts(),
			ReadyOperations(),
			_didAnyEvaluate(false),
			InputFileLookup(),
			OutputFileLookup()
		{
			LoadOperationLookup();
		}

	public:
		void AddReadyOperations(const std::vector<OperationId>& operations)
		{
			std::unique_lock lock(_mutex);

			for (auto operationId : operations)
				ReadyOperations.push(operationId);
		}

		bool DidAnyEvaluate()
		{
			std::unique_lock lock(_mutex);

			if (_error)
				std::rethrow_exception(_error);

			return _didAnyEvaluate;
		}

		std::optional<CurrentOperationState> WaitNextOperation()
		{
			std::unique_lock lock(_mutex);

			// If nothing to do then wait
			if (ReadyOperations.empty() && !_isComplete)
			{
				_conditionalVariable.wait(
					lock, [&]{ return !ReadyOperations.empty() || _isComplete; });
			}

			if (_isComplete)
			{
				return std::nullopt;
			}
			else if (!ReadyOperations.empty())
			{
				auto currentOperationId = ReadyOperations.front();
				ReadyOperations.pop();
				_activeOperations++;

				auto& operationInfo = Graph.GetOperationInfo(currentOperationId);

				OperationResult* previousResult;
				if (!Results.TryFindResult(operationInfo.Id, previousResult))
				{
					previousResult = nullptr;
				}

				CurrentOperationState operationState = { operationInfo, previousResult };
				return operationState;
			}
			else
			{
				throw std::runtime_error("No Operations and not complete");
			}
		}

		void SetError(std::exception_ptr exception)
		{
			std::unique_lock lock(_mutex);
			_error = exception;
			_isComplete = true;
			_activeOperations--;
			_conditionalVariable.notify_all();
		}

		void UpdateOperationResult(
			OperationId operationId,
			OperationResult&& operationResult)
		{
			std::unique_lock lock(_mutex);

			// This is only called when an operation was built
			_didAnyEvaluate |= true;

			Results.AddOrUpdateOperationResult(
				operationId,
				std::move(operationResult));
		}

		/// <summary>
		/// Execute the collection of build operations
		/// </summary>
		void RegisterReadyChildren(const OperationInfo& operationInfo)
		{
			std::unique_lock lock(_mutex);
			_activeOperations--;

			size_t countAdded = 0;
			for (auto operationId : operationInfo.Children)
			{
				// Only register the operation when all of its dependencies have completed
				auto currentOperationSearch = RemainingDependencyCounts.find(operationId);
				int32_t remainingCount = -1;
				if (currentOperationSearch != RemainingDependencyCounts.end())
				{
					remainingCount = --currentOperationSearch->second;
				}
				else
				{
					// Get the cached total count and store the active count in the lookup
					auto& childOperationInfo = Graph.GetOperationInfo(operationId);
					remainingCount = childOperationInfo.DependencyCount - 1;
					auto insertResult = RemainingDependencyCounts.emplace(operationId, remainingCount);
					if (!insertResult.second)
						throw std::runtime_error("The operation id already existed in the remaining count lookup");
				}

				if (remainingCount == 0)
				{
					ReadyOperations.push(operationId);
					countAdded++;
				}
				else if (remainingCount < 0)
				{
					throw std::runtime_error("Remaining dependency count less than zero");
				}
				else
				{
					// This operation still has dependencies that have not finished
				}
			}

			if (countAdded > 0)
			{
				// Notify more work added
				_conditionalVariable.notify_all();
			}
			else if (_activeOperations == 0 && ReadyOperations.empty())
			{
				// We are the last operation
				_isComplete = true;
				_conditionalVariable.notify_all();
			}
		}

		bool TryGetInputFileOperations(
			FileId fileId,
			const std::set<OperationId>*& result) const
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
			OperationId& result) const
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

	private:
		void LoadOperationLookup()
		{
			for (auto& operation : Graph.GetOperations())
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
		}
	};
}