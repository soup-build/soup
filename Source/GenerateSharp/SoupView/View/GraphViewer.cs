﻿// <copyright file="GraphViewer.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Shapes;
using SoupView.ViewModel;
using Windows.Foundation;
using Windows.UI;

namespace SoupView.View
{
	public sealed class GraphViewer : Control
	{
		private static int NodeWidth = 200;
		private static int NodeHeight = 50;

		private static int NodeSpacingHorizontal = 80;
		private static int NodeSpacingVertical = 50;

		private static int InternalPadding = 20;

		/// <summary>
		/// Identifies the <see cref="Graph"/> dependency property.
		/// </summary>
		public static readonly DependencyProperty GraphProperty = DependencyProperty.Register(
			nameof(Graph),
			typeof(IList<IList<GraphNode>>),
			typeof(GraphViewer),
			new PropertyMetadata(null, new PropertyChangedCallback(OnPropertyChanged)));

		/// <summary>
		/// Identifies the <see cref="SelectedNode"/> dependency property.
		/// </summary>
		public static readonly DependencyProperty SelectedNodeProperty = DependencyProperty.Register(
			nameof(SelectedNode),
			typeof(GraphNode),
			typeof(GraphViewer),
			new PropertyMetadata(null));

		public GraphViewer()
		{
			this.DefaultStyleKey = typeof(GraphViewer);
			this.SizeChanged += GraphViewer_SizeChanged;
		}

		/// <summary>
		/// Gets or sets the Graph
		/// </summary>
		public IList<IList<GraphNode>> Graph
		{
			get { return (IList<IList<GraphNode>>)GetValue(GraphProperty); }
			set { SetValue(GraphProperty, value); }
		}

		/// <summary>
		/// Gets or sets the Selected Node
		/// </summary>
		public GraphNode SelectedNode
		{
			get { return (GraphNode)GetValue(SelectedNodeProperty); }
			set { SetValue(SelectedNodeProperty, value); }
		}

		private static void OnPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			((GraphViewer)d).LayoutGraph();
		}

		private void GraphViewer_SizeChanged(object sender, SizeChangedEventArgs e)
		{
			this.LayoutGraph();
		}

		protected override void OnApplyTemplate()
		{
			this.LayoutGraph();
			base.OnApplyTemplate();
		}

		private void LayoutGraph()
		{
			var canvas = this.GetTemplateChild("RootCanvas") as Canvas;
			if (canvas is null)
				return;

			canvas.Children.Clear();

			if (this.Graph is null)
				return;

			int maxHeight = 0;
			int currentOffsetX = 0;
			int currentOffsetY;
			var nodeState = new Dictionary<uint, (GraphViewerItem Item, Point InConnect, Point OutConnect)>();
			foreach (var column in this.Graph)
			{
				// Reset vertical offset for each column
				currentOffsetY = 0;

				// Update X offset for the current column location
				if (currentOffsetX == 0)
					currentOffsetX += InternalPadding;
				else
					currentOffsetX += NodeSpacingHorizontal;

				foreach (var value in column)
				{
					// Update Y for the current row location
					if (currentOffsetY == 0)
						currentOffsetY += InternalPadding;
					else
						currentOffsetY += NodeSpacingVertical;

					if (value is not null)
					{
						var node = new GraphViewerItem()
						{
							Title = value.Title,
							Width = NodeWidth,
							Height = NodeHeight,
						};
						node.DataContext = value;
						node.Click += Node_Click;

						Canvas.SetLeft(node, currentOffsetX);
						Canvas.SetTop(node, currentOffsetY);

						canvas.Children.Add(node);

						// Save the node state
						var inConnect = new Point(currentOffsetX, currentOffsetY + (NodeHeight / 2));
						var outConnect = new Point(currentOffsetX + NodeWidth, currentOffsetY + (NodeHeight / 2));
						nodeState.Add(value.Id, (node, inConnect, outConnect));
					}

					// Update for the next row location
					currentOffsetY += NodeHeight;
				}

				// Update for the next column location
				currentOffsetX += NodeWidth;

				// Check the max height
				maxHeight = Math.Max(maxHeight, currentOffsetY);
			}

			// Connect all the known nodes
			foreach (var column in Graph)
			{
				foreach (var value in column)
				{
					if (value is not null)
					{
						var startNode = nodeState[value.Id];
						foreach (var child in value.ChildNodes)
						{
							var endNode = nodeState[child];
							var path = ConnectNodes(startNode.OutConnect, endNode.InConnect);
							canvas.Children.Add(path);
						}
					}
				}
			}

			// Add the final internal padding to get the total size
			canvas.Width = currentOffsetX + InternalPadding;
			canvas.Height = maxHeight + InternalPadding;
		}

		private void Node_Click(object sender, RoutedEventArgs e)
		{
			var node = (GraphViewerItem)sender;
			this.SelectedNode = (GraphNode)node.DataContext;
		}

		private Path ConnectNodes(Point start, Point end)
		{
			var control1 = new Point(
				start.X + (NodeSpacingHorizontal / 2),
				start.Y);
			var control2 = new Point(
				end.X - (NodeSpacingHorizontal / 2),
				end.Y);

			return CreateBezier(start, control1, control2, end);
		}

		private Path CreateBezier(Point start, Point control1, Point control2, Point end)
		{
			var path = new Path()
			{
				Stroke = new SolidColorBrush((Color)Resources["SystemBaseLowColor"]),
				StrokeThickness = 1,
				Data = new PathGeometry()
				{
					Figures = new PathFigureCollection()
					{
						new PathFigure()
						{
							StartPoint = start,
							Segments = new PathSegmentCollection()
							{
								new BezierSegment()
								{
									Point1 = control1,
									Point2 = control2,
									Point3 = end,
								}
							},
						},
					},
				},
			};

			return path;
		}
	}
}