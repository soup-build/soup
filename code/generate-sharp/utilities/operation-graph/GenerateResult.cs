// <copyright file="GenerateResult.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;
using Path = Opal.Path;

namespace Soup.Build.Utilities;

/// <summary>
/// The cached operation graph that is used to track input/output mappings for previous build
/// executions to support incremental builds
/// </summary>
public class GenerateResult
{
	private readonly Dictionary<OperationProxyId, OperationProxyInfo> operationProxies;
	private readonly Dictionary<CommandInfo, OperationProxyId> operationProxyLookup;

	/// <summary>
	/// Initializes a new instance of the <see cref="GenerateResult"/> class.
	/// </summary>
	public GenerateResult()
	{
		this.ReferencedFiles = [];
		this.EvaluateGraph = new OperationGraph();
		this.operationProxies = [];
		this.operationProxyLookup = [];
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="GenerateResult"/> class.
	/// </summary>
	public GenerateResult(
		IList<(FileId FileId, Path Path)> referencedFiles,
		OperationGraph evaluateGraph,
		IList<OperationProxyInfo> operationProxies)
	{
		this.ReferencedFiles = referencedFiles;
		this.EvaluateGraph = evaluateGraph;
		this.operationProxies = [];
		this.operationProxyLookup = [];

		// Store the incoming vector of operations as a lookup for fast checks
		foreach (var info in operationProxies)
		{
			AddOperationProxy(info);
		}
	}

	/// <summary>
	/// Get the set of referenced file ids that map to their paths
	/// </summary>
	public IList<(FileId FileId, Path Path)> ReferencedFiles { get; init; }

	/// <summary>
	/// Get Evaluate Graph
	/// </summary>
	public OperationGraph EvaluateGraph { get; }


	/// <summary>
	/// Get Operations
	/// </summary>
	public IReadOnlyDictionary<OperationProxyId, OperationProxyInfo> OperationProxies => this.operationProxies;

	/// <summary>
	/// Add an operation proxy info
	/// </summary>
	public void AddOperationProxy(OperationProxyInfo info)
	{
		this.operationProxyLookup.Add(info.Command, info.Id);
		this.operationProxies.Add(info.Id, info);
	}
}