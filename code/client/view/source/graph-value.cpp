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
		size_t Source;
		size_t Target;
	};

	struct Graph
	{
		size_t Vertices;
		std::vector<GraphEdge> Edges;
	};

	struct GraphPoint
	{
		size_t X;
		size_t Y;
	};
}