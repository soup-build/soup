// <copyright file="OperationGraphViewModel.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using GraphShape;
using ReactiveUI;
using Soup.Build.Utilities;
using Soup.View.Views;
using System.Collections.Generic;
using System.Linq;

namespace Soup.View.ViewModels;

public class OperationGraphViewModel : ContentPaneViewModel
{
	private readonly FileSystemState fileSystemState;
	private GraphNodeViewModel? selectedNode;
	private OperationDetailsViewModel? selectedOperation;
	private IList<GraphNodeViewModel>? graph;
	private readonly Dictionary<uint, OperationDetailsViewModel> operationDetailsLookup = [];

	public OperationGraphViewModel(FileSystemState fileSystemState)
	{
		this.fileSystemState = fileSystemState;
	}

	public IList<GraphNodeViewModel>? Graph
	{
		get => this.graph;
		private set => this.RaiseAndSetIfChanged(ref this.graph, value);
	}

	public GraphNodeViewModel? SelectedNode
	{
		get => this.selectedNode;
		set
		{
			if (CheckRaiseAndSetIfChanged(ref this.selectedNode, value))
			{
				this.SelectedOperation = this.selectedNode is not null ?
					this.operationDetailsLookup[this.selectedNode.Id] : null;
			}
		}
	}

	public OperationDetailsViewModel? SelectedOperation
	{
		get => this.selectedOperation;
		set => this.RaiseAndSetIfChanged(ref this.selectedOperation, value);
	}

	public void Load(
		OperationGraph? evaluateGraph,
		OperationResults? operationResults)
	{
		this.Graph = null;

		if (evaluateGraph is not null)
		{
			var activeGraph = BuildGraph(evaluateGraph, operationResults);
			this.Graph = activeGraph;
		}
	}

	private List<GraphNodeViewModel> BuildGraph(
		OperationGraph evaluateGraph,
		OperationResults? operationResults)
	{
		this.operationDetailsLookup.Clear();

		var graph = evaluateGraph.Operations
			.Select(value => (value.Value, value.Value.Children.Select(value => evaluateGraph.Operations[value])));

		// TODO: Should the layout be a visual aspect of the view? Yes, yes it should.
		var graphView = GraphBuilder.BuildDirectedAcyclicGraphView(
			graph,
			new Size(GraphViewer.NodeWidth, GraphViewer.NodeHeight));

		var graphNodes = BuildGraphNodes(graphView, operationResults);

		return graphNodes;
	}

	private List<GraphNodeViewModel> BuildGraphNodes(
		IDictionary<OperationInfo, Point> nodePositions,
		OperationResults? operationResults)
	{
		var result = new List<GraphNodeViewModel>();
		foreach (var (operation, position) in nodePositions)
		{
			var toolTop = operation.Title;
			var node = new GraphNodeViewModel()
			{
				Title = operation.Title,
				ToolTip = toolTop,
				Id = operation.Id.Value,
				ChildNodes = [.. operation.Children.Select(value => value.Value)],
				Position = position,
			};

			result.Add(node);

			// Check if there is a matching result
			OperationResult? operationResult = null;
			if (operationResults != null)
			{
				if (operationResults.TryFindResult(operation.Id, out var operationResultValue))
				{
					operationResult = operationResultValue;
				}
			}

			this.operationDetailsLookup.Add(
				operation.Id.Value,
				new OperationDetailsViewModel(this.fileSystemState, operation, operationResult));
		}

		return result;
	}
}