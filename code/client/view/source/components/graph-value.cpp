// <copyright file="graph-value.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <vector>

export module Soup.View:GraphValue;

import Opal;

export namespace Soup::View
{
	struct GraphEdge
	{
		int Source;
		int Target;
	};

	struct Graph
	{
		int Vertices;
		std::vector<GraphEdge> Edges;
	};

	struct GraphNode
	{
		int Index;

		// The set of edges in the next row
		std::vector<int> Edges;
	};
}