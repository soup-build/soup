// <copyright file="GenerateResult.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

export module Soup.Core:GenerateResult;

import Opal;
import :CommandInfo;
import :OperationGraph;

using namespace Opal;

namespace Soup::Core 
{
	/// <summary>
	/// The generate result that represents the known operation graph that needs to 
	/// be evaluated and a boolean indication if this is a preprocessing run
	/// </summary>
	export class GenerateResult
	{
	private:
		OperationGraph _evaluateGraph;
		bool _isPreprocessor;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="GenerateResult"/> class.
		/// </summary>
		GenerateResult() :
			_evaluateGraph(),
			_isPreprocessor()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="GenerateResult"/> class.
		/// </summary>
		GenerateResult(
			OperationGraph evaluateGraph,
			bool isPreprocessor) :
			_evaluateGraph(std::move(evaluateGraph)),
			_isPreprocessor(isPreprocessor)
		{
		}

		/// <summary>
		/// Get the evaluation operation graph
		/// </summary>
		const OperationGraph& GetEvaluateGraph() const
		{
			return _evaluateGraph;
		}

		/// <summary>
		/// Get the evaluation operation graph
		/// </summary>
		OperationGraph& GetEvaluateGraph()
		{
			return _evaluateGraph;
		}

		/// <summary>
		/// Get a value indicating if the graph is for preprocessing files
		/// </summary>
		bool IsPreprocessor() const
		{
			return _isPreprocessor;
		}
	};
}