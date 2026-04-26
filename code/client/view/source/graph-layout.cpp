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
	struct VertexState
	{
		int Layer;
		int Column;
	};

	void RecursiveInitialLayers(
		const Graph& graph,
		std::vector<VertexState>& vertexLookup,
		std::vector<std::vector<GraphNode>>& layers,
		int vertex)
	{
		int maxY = 0;
		for (auto& edge : graph.Edges)
		{
			if (edge.Target == vertex)
			{
				// Compute recursive if not already set
				if (vertexLookup[edge.Source].Layer < 0)
					RecursiveInitialLayers(graph, vertexLookup, layers, edge.Source);

				auto newY = vertexLookup[edge.Source].Layer + 1;
				maxY = std::max(maxY, newY);
			}
		}

		if (layers.size() < maxY + 1)
			layers.resize(maxY + 1);

		vertexLookup[vertex].Layer = maxY;
		vertexLookup[vertex].Column = layers[maxY].size();

		layers[maxY].push_back({ vertex, std::vector<int>() });
	}

	/// <summary>
	/// Sugiyama Layout
	/// Note: We skip the cycle breaking as we assume a DAG
	/// </summary>
	export std::vector<std::vector<GraphNode>> LayoutDAG(const Graph& graph)
	{
		// Set 1: Level setting
		auto vertexLookup = std::vector<VertexState>(graph.Vertices, { -1, -1 });
		auto layers = std::vector<std::vector<GraphNode>>();

		// Initialize all layers with maximum distance required to reach each edge
		for (size_t vertex = 0; vertex < graph.Vertices; vertex++)
		{
			RecursiveInitialLayers(graph, vertexLookup, layers, vertex);
		}

		// Build up edges to next layers
		// Add fake intermediate nodes for edges that span multiple layers
		for (auto& edge : graph.Edges)
		{
			auto& sourceVertex = vertexLookup[edge.Source];
			auto& targetVertex = vertexLookup[edge.Target];

			auto& sourceNode = layers[sourceVertex.Layer][sourceVertex.Column];

			if (sourceVertex.Layer == targetVertex.Layer - 1)
			{
				sourceNode.Edges.push_back(targetVertex.Column);
			}
		}

		return layers;
	}
}
