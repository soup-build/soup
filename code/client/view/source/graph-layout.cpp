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
		std::vector<GraphPoint>& positions,
		std::vector<std::vector<int>>& layers,
		size_t vertex)
	{
		size_t maxY = 1;
		for (auto& edge : graph.Edges)
		{
			if (edge.Target == vertex)
			{
				if (positions[edge.Source].Y == 0)
					RecursiveInitialLayers(graph, positions, layers, edge.Source);

				auto newY = positions[edge.Source].Y + 1;
				maxY = std::max(maxY, newY);
			}
		}

		if (layers.size() < maxY)
			layers.resize(maxY);

		layers[maxY - 1].push_back(vertex);
		positions[vertex].Y = maxY;
		positions[vertex].X = layers[maxY - 1].size();
	}

	/// <summary>
	/// Sugiyama Layout
	/// Note: We skip the cycle breaking as we assume a DAG
	/// </summary>
	export std::vector<GraphPoint> LayoutDAG(const Graph& graph)
	{
		// Set 1: Level setting
		auto positions = std::vector<GraphPoint>(graph.Vertices);
		auto layers = std::vector<std::vector<int>>();

		// Find all source nodes with no incoming edges
		for (size_t vertex = 0; vertex < graph.Vertices; vertex++)
		{
			RecursiveInitialLayers(graph, positions, layers, vertex);
		}

		return positions;
	}
}
