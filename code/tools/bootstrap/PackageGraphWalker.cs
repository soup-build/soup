// <copyright file="PackageGraphWalker.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Path = Opal.Path;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Soup.Build.Utilities;
using Soup.Tools;

namespace Soup.Build.Bootstrap;

/// <summary>
/// Package Graph walker that ensures the packages are visited in a valid order
/// </summary>
public class PackageGraphWalker
{
	private readonly PackageProvider dependencyGraph;

	// Running State
	private readonly Dictionary<OperationId, int> remainingDependencyCounts;

	public PackageGraphWalker(PackageProvider dependencyGraph)
	{
		this.dependencyGraph = dependencyGraph;
		this.remainingDependencyCounts = new Dictionary<OperationId, int>();
	}

	public IEnumerable<PackageInfo> WalkGraph()
	{
		return InvertGraph();
	}

	private List<PackageInfo> InvertGraph()
	{
		var visitedSet = new HashSet<int>();
		var activeNodes = new Stack<int>();

		activeNodes.Push(this.dependencyGraph.GetRootPackageGraph().RootPackageId);

		var result = new List<PackageInfo>();

		while (activeNodes.Count > 0)
		{
			var currentNodeId = activeNodes.Peek();
			var package = this.dependencyGraph.GetPackageInfo(currentNodeId);

			bool allChildrenVisited = true;
			foreach (var child in GetChildren(package))
			{
				if (!visitedSet.Contains(child.Id))
				{
					allChildrenVisited = false;
					activeNodes.Push(child.Id);
				}
			}

			// Once all children are visited, remove this node and add to result
			// Otherwise a child was added and we will reevaluate this node after the children
			if (allChildrenVisited)
			{
				_ = activeNodes.Pop();
				visitedSet.Add(currentNodeId);
				result.Add(package);
			}
		}

		return result;
	}

	private List<PackageInfo> GetChildren(PackageInfo package)
	{
		var result = new List<PackageInfo>();
		foreach (var (dependencyType, children) in package.Dependencies)
		{
			foreach (var child in children)
			{
				if (!child.IsSubGraph)
				{
					result.Add(this.dependencyGraph.GetPackageInfo(child.PackageId ??
						throw new InvalidOperationException("Package child does not have package id")));
				}
			}
		}

		return result;
	}
}