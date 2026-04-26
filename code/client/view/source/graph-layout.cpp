// <copyright file="graph-layout.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <algorithm>
#include <string>
#include <vector>

export module Soup.View:GraphLayout;

import :GraphValue;

namespace Soup::View
{
	void RecursiveInitialLayers(
		const Graph& graph,
		std::vector<size_t>& vertexRowLookup,
		std::vector<std::vector<GraphNode>>& layers,
		size_t vertex)
	{
		size_t maxY = 1;
		for (auto& edge : graph.Edges)
		{
			if (edge.Target == vertex)
			{
				if (vertexRowLookup[edge.Source] == 0)
					RecursiveInitialLayers(graph, vertexRowLookup, layers, edge.Source);

				auto newY = vertexRowLookup[edge.Source] + 1;
				maxY = std::max(maxY, newY);
			}
		}

		if (layers.size() < maxY)
			layers.resize(maxY);

		layers[maxY - 1].push_back({ vertex, std::vector<size_t>() });
		vertexRowLookup[vertex] = maxY;
	}

	/// <summary>
	/// Sugiyama Layout
	/// Note: We skip the cycle breaking as we assume a DAG
	/// </summary>
	export std::vector<std::vector<GraphNode>> LayoutDAG(const Graph& graph)
	{
		// Set 1: Level setting
		auto vertexRowLookup = std::vector<size_t>(graph.Vertices);
		auto layers = std::vector<std::vector<GraphNode>>();

		// Find all source nodes with no incoming edges
		for (size_t vertex = 0; vertex < graph.Vertices; vertex++)
		{
			RecursiveInitialLayers(graph, vertexRowLookup, layers, vertex);
		}

		return layers;
	}
}
