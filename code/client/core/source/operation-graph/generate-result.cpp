// <copyright file="generate-result.cpp" company="Soup">
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
		OperationGraph _graph;
		bool _isPreprocessor;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="GenerateResult"/> class.
		/// </summary>
		GenerateResult() :
			_graph(),
			_isPreprocessor()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="GenerateResult"/> class.
		/// </summary>
		GenerateResult(
			OperationGraph graph,
			bool isPreprocessor) :
			_graph(std::move(graph)),
			_isPreprocessor(isPreprocessor)
		{
		}

		/// <summary>
		/// Get the operation graph
		/// </summary>
		const OperationGraph& GetGraph() const
		{
			return _graph;
		}

		/// <summary>
		/// Get the operation graph
		/// </summary>
		OperationGraph& GetGraph()
		{
			return _graph;
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