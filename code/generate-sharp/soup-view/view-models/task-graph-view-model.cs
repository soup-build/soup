// <copyright file="task-graph-view-model.cs" company="Soup">
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

public class TaskGraphViewModel : ContentPaneViewModel
{
	private sealed class TaskDetails
	{
		public required string Name { get; set; }
		public required uint Id { get; set; }
		public required ValueTable TaskInfo { get; set; }
		public required List<TaskDetails> Children { get; init; }
	}

	private GraphNodeViewModel? selectedNode;
	private TaskDetailsViewModel? selectedTask;
	private uint uniqueId;
	private IList<GraphNodeViewModel>? graph;
	private readonly Dictionary<uint, TaskDetailsViewModel> taskDetailsLookup = [];

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
				this.SelectedTask = this.selectedNode is not null ? this.taskDetailsLookup[this.selectedNode.Id] : null;
			}
		}
	}

	public TaskDetailsViewModel? SelectedTask
	{
		get => this.selectedTask;
		set => this.RaiseAndSetIfChanged(ref this.selectedTask, value);
	}

	public void Load(ValueTable? generateInfoTable)
	{
		this.Graph = null;

		if (generateInfoTable is not null)
		{
			var activeGraph = BuildGraph(generateInfoTable);
			this.Graph = activeGraph;
		}
	}

	private List<GraphNodeViewModel>? BuildGraph(ValueTable generateInfoTable)
	{
		this.taskDetailsLookup.Clear();
		this.uniqueId = 1;

		if (!generateInfoTable.TryGetValue("RuntimeOrder", out var runtimeOrderList) || runtimeOrderList.Type != ValueType.List)
		{
			NotifyError($"Generate Info Table missing RuntimeOrder List");
			return null;
		}

		if (!generateInfoTable.TryGetValue("TaskInfo", out var taskInfoTable) || taskInfoTable.Type != ValueType.Table)
		{
			NotifyError($"Generate Info Table missing TaskInfo Table");
			return null;
		}

		if (!generateInfoTable.TryGetValue("GlobalState", out var globalStateTable) || globalStateTable.Type != ValueType.Table)
		{
			NotifyError($"Generate Info Table missing GlobalState Table");
			return null;
		}

		var activeGraph = BuildGraph(runtimeOrderList.AsList(), taskInfoTable.AsTable(), globalStateTable.AsTable());

		return activeGraph;
	}

	private List<GraphNodeViewModel> BuildGraph(
		ValueList runtimeOrderList,
		ValueTable taskInfoTable,
		ValueTable globalStateTable)
	{
		var tasks = new Dictionary<string, TaskDetails>();

		// Add each task to its own column
		foreach (var taskNameValue in runtimeOrderList)
		{
			var taskName = taskNameValue.AsString();

			// Find the Task Info
			var taskInfo = taskInfoTable[taskName].AsTable();

			// TODO: Have a custom view for the global state
			taskInfo["GlobalState"] = new Value(globalStateTable);

			tasks.Add(
				taskName,
				new TaskDetails()
				{
					Name = taskName,
					TaskInfo = taskInfo,
					Id = this.uniqueId++,
					Children = [],
				});
		}

		// Build up the children
		foreach (var (taskName, task) in tasks)
		{
			// Build up the children set
			var runAfterClosure = task.TaskInfo["RunAfterClosureList"].AsList();

			foreach (var parent in runAfterClosure)
			{
				tasks[parent.AsString()].Children.Add(task);
			}
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