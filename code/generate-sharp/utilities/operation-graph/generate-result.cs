// <copyright file="generate-result.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Soup.Build.Utilities;

/// <summary>
/// The cached operation graph that is used to track input/output mappings for previous build
/// executions to support incremental builds
/// </summary>
public class GenerateResult
{
	/// <summary>
	/// Initializes a new instance of the <see cref="GenerateResult"/> class.
	/// </summary>
	public GenerateResult()
	{
		this.EvaluateGraph = new OperationGraph();
		this.IsPreprocessor = false;
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="GenerateResult"/> class.
	/// </summary>
	public GenerateResult(
		OperationGraph evaluateGraph,
		bool isPreprocessor)
	{
		this.EvaluateGraph = evaluateGraph;
		this.IsPreprocessor = isPreprocessor;
	}

	/// <summary>
	/// Get Evaluate Graph
	/// </summary>
	public OperationGraph EvaluateGraph { get; }

	/// <summary>
	/// Get a value indicating if the graph is preprocessing
	/// </summary>
	public bool IsPreprocessor { get; }
}