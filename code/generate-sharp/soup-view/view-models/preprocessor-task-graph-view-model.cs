// <copyright file="preprocessor-task-graph-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using GraphShape;
using ReactiveUI;
using Soup.Build.Utilities;
using Soup.View.Views;
using System.Collections.Generic;
using System.Linq;
using ValueType = Soup.Build.Utilities.ValueType;

namespace Soup.View.ViewModels;

public class PreprocessorTaskGraphViewModel : ContentPaneViewModel
{
	private sealed class TaskDetails
	{
		public required string Name { get; set; }
		public required uint Id { get; set; }
		public required ValueTable TaskInfo { get; set; }
		public required List<TaskDetails> Children { get; init; }
	}

	private uint uniqueId;
	private readonly Dictionary<uint, TaskDetailsViewModel> taskDetailsLookup = [];

	public IList<GraphNodeViewModel>? Graph
	{
		get;
		private set => this.RaiseAndSetIfChanged(ref field, value);
	}

	public GraphNodeViewModel? SelectedNode
	{
		get;
		set
		{
			if (CheckRaiseAndSetIfChanged(ref field, value))
			{
				this.SelectedTask = field is not null ? this.taskDetailsLookup[field.Id] : null;
			}
		}
	}

	public TaskDetailsViewModel? SelectedTask
	{
		get;
		set => this.RaiseAndSetIfChanged(ref field, value);
	}

	public void Load(ValueTable? generateInfoTable)
	{
		this.Graph = null;

		if (generateInfoTable is not null)
		{
			var activeGraph = TryBuildGraph(generateInfoTable);
			this.Graph = activeGraph;
		}
	}

	private List<GraphNodeViewModel>? TryBuildGraph(ValueTable generateInfoTable)
	{
		this.taskDetailsLookup.Clear();
		this.uniqueId = 1;

		if (!generateInfoTable.TryGetValue("TaskInfo", out var taskInfoTable) || taskInfoTable.Type != ValueType.Table)
		{
			NotifyError($"Generate Info Table missing TaskInfo Table");
			return null;
		}

		var activeGraph = BuildGraph(taskInfoTable.AsTable());

		return activeGraph;
	}

	private List<GraphNodeViewModel> BuildGraph(
		ValueTable taskInfoTable)
	{
		var tasks = new Dictionary<string, TaskDetails>();

		// Add each task to its own column
		foreach (var (taskName, taskInfo) in taskInfoTable)
		{
			tasks.Add(
				taskName,
				new TaskDetails()
				{
					Name = taskName,
					TaskInfo = taskInfo.AsTable(),
					Id = this.uniqueId++,
					Children = [],
				});
		}

		var graph = new List<(TaskDetails Node, IEnumerable<TaskDetails> Children)>();

		// Add each task to its own column
		foreach (var (_, task) in tasks)
		{
			graph.Add((task, task.Children));
		}

		// TODO: Should the layout be a visual aspect of the view? Yes, yes it should.
		var graphView = GraphBuilder.BuildDirectedAcyclicGraphView(
			graph,
			new Size(GraphViewer.NodeWidth, GraphViewer.NodeHeight));

		var graphNodes = BuildGraphNodes(graphView);
		return graphNodes;
	}

	private List<GraphNodeViewModel> BuildGraphNodes(
		IDictionary<TaskDetails, Point> nodePositions)
	{
		var result = new List<GraphNodeViewModel>();
		foreach (var (task, position) in nodePositions)
		{
			var toolTop = task.Name;
			var node = new GraphNodeViewModel()
			{
				Title = task.Name,
				ToolTip = toolTop,
				Id = task.Id,
				ChildNodes = [.. task.Children.Select(value => value.Id)],
				Position = position,
			};

			result.Add(node);

			this.taskDetailsLookup.Add(
				task.Id,
				new TaskDetailsViewModel(task.TaskInfo));
		}

		return result;
	}
}