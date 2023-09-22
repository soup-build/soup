﻿// <copyright file="GraphNode.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Soup.View.ViewModels
{
	public class GraphNodeViewModel
	{
		public GraphNodeViewModel(string title, uint id)
		{
			this.Title = title;
			this.Id = id;
		}

		public string Title { get; set; } = string.Empty;
		public uint Id { get; set; } = 0;
		public IList<uint> ChildNodes { get; set; } = new List<uint>();
	}
}