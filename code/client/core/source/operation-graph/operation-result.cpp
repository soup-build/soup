// <copyright file="operation-result.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <vector>

export module Soup.Core:OperationResult;

import Opal;
import :FileSystemState;
import :Value;

using namespace Opal;
using namespace std::chrono_literals;

namespace Soup::Core
{
	/// <summary>
	/// A node result that tacks the observed output from previous runs
	/// </summary>
	export class OperationResult
	{
	public:
		bool WasSuccessfulRun;
		std::chrono::time_point<std::chrono::file_clock> EvaluateTime;
		std::vector<FileId> ObservedInput;
		std::vector<FileId> ObservedOutput;

		// Implementation dependent set of values that allows an operation to
		// store intermediate state
		// Note: Used for preprocessor results
		std::optional<ValueTable> ObservedValues;

	public:
		OperationResult() :
			WasSuccessfulRun(false),
			EvaluateTime(std::chrono::time_point<std::chrono::file_clock>::min()),
			ObservedInput(),
			ObservedOutput(),
			ObservedValues(std::nullopt)
		{
		}

		OperationResult(
			bool wasSuccessfulRun,
			std::chrono::time_point<std::chrono::file_clock> evaluateTime,
			std::vector<FileId>&& observedInput,
			std::vector<FileId>&& observedOutput,
			std::optional<ValueTable>&& observedValues) :
			WasSuccessfulRun(wasSuccessfulRun),
			EvaluateTime(evaluateTime),
			ObservedInput(std::move(observedInput)),
			ObservedOutput(std::move(observedOutput)),
			ObservedValues(std::move(observedValues))
		{
		}

		bool operator ==(const OperationResult& rhs) const
		{
			return WasSuccessfulRun == rhs.WasSuccessfulRun &&
				EvaluateTime == rhs.EvaluateTime &&
				ObservedInput == rhs.ObservedInput &&
				ObservedOutput == rhs.ObservedOutput &&
				ObservedValues == rhs.ObservedValues;
		}
	};
}