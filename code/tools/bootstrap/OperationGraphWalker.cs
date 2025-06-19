// <copyright file="OperationGraphWalker.cs" company="Soup">
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
		foreach (var operation in this.operationGraph.Operations)
		{
			yield return operation.Value;
		}
	}
}