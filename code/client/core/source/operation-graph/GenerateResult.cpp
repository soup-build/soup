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
import :OperationProxyInfo;

using namespace Opal;

namespace Soup::Core 
{
	/// <summary>
	/// The operation generate state that represents the known operation graph and any operation proxies that need to 
	/// be evaluated
	/// </summary>
	export class GenerateResult
	{
	private:
		OperationGraph _evaluateGraph;

		std::map<OperationProxyId, OperationProxyInfo> _operationProxies;
		std::unordered_map<CommandInfo, OperationProxyId> _operationProxyLookup;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="OperationGraph"/> class.
		/// </summary>
		GenerateResult() :
			_evaluateGraph(),
			_operationProxies(),
			_operationProxyLookup()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="GenerateResult"/> class.
		/// </summary>
		GenerateResult(
			OperationGraph evaluateGraph,
			std::vector<OperationProxyInfo> operationProxies) :
			_evaluateGraph(std::move(evaluateGraph)),
			_operationProxies(),
			_operationProxyLookup()
		{
			// Store the incoming vector of operation proxies as a lookup for fast checks
			for (auto& info : operationProxies)
			{
				AddOperationProxy(std::move(info));
			}
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
		/// Get Operation Proxies
		/// </summary>
		const std::map<OperationProxyId, OperationProxyInfo>& GetOperationProxies() const
		{
			return _operationProxies;
		}

		/// <summary>
		/// Get Operation Proxies
		/// </summary>
		std::map<OperationProxyId, OperationProxyInfo>& GetOperationProxies()
		{
			return _operationProxies;
		}

		/// <summary>
		/// Gets a value indicating if there are any proxy operations
		/// </summary>
		bool HasProxyOperations()
		{
			return !_operationProxies.empty();
		}

		/// <summary>
		/// Find an operation info
		/// </summary>
		bool HasOperationProxyCommand(const CommandInfo& command) const
		{
			return _operationProxyLookup.contains(command);
		}

		/// <summary>
		/// Add an operation proxy info
		/// </summary>
		OperationProxyInfo& AddOperationProxy(OperationProxyInfo info)
		{
			auto insertLookupResult = _operationProxyLookup.emplace(info.Command, info.Id);
			if (!insertLookupResult.second)
				throw std::runtime_error("The provided command already exists in the graph");

			auto [insertIterator, wasInserted] = _operationProxies.emplace(info.Id, std::move(info));
			if (!wasInserted)
				throw std::runtime_error("The provided operation proxy id already exists in the graph");

			return insertIterator->second;
		}
	};
}