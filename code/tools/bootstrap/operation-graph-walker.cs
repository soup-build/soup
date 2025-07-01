// <copyright file="operation-graph-walker.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Path = Opal.Path;
using System;
using System.Collections;
using System.Collections.Generic;
using Soup.Build.Utilities;

namespace Soup.Build.Bootstrap;

/// <summary>
/// Operation Graph walker that ensures the operations are visited in a valid order
/// </summary>
public class OperationGraphWalker
{
	private readonly OperationGraph operationGraph;

	// Running State
	private readonly Dictionary<OperationId, int> remainingDependencyCounts;

	public OperationGraphWalker(OperationGraph operationGraph)
	{
		this.operationGraph = operationGraph;
		this.remainingDependencyCounts = new Dictionary<OperationId, int>();
	}

	public IEnumerable<OperationInfo> WalkGraph()
	{
		return WalkSubGraph(this.operationGraph.RootOperationIds);
	}

	public IEnumerable<OperationInfo> WalkSubGraph(IList<OperationId> operations)
	{
		var result = new List<OperationInfo>();
		foreach (var operationId in operations)
		{
			// Check if the operation was already a child from a different path
			// Only return the operation when all of its dependencies have completed
			var operationInfo = this.operationGraph.GetOperationInfo(operationId);
			var remainingCount = -1;
			if (remainingDependencyCounts.TryGetValue(operationId, out var currentRemainingCount))
			{
				remainingCount = --currentRemainingCount;
				remainingDependencyCounts[operationId] = remainingCount;
			}
			else
			{
				// Get the cached total count and store the active count in the lookup
				remainingCount = (int)operationInfo.DependencyCount - 1;
				remainingDependencyCounts.Add(operationId, remainingCount);
			}

			if (remainingCount == 0)
			{
				result.Add(operationInfo);

				// Recursively build all of the operation children
				result.AddRange(WalkSubGraph(operationInfo.Children));
			}
			else if (remainingCount < 0)
			{
				throw new InvalidOperationException("Remaining dependency count less than zero");
			}
			else
			{
				// This operation will be executed from a different path
			}
		}

		return result;
	}
}