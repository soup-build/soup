// <copyright file="graph-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

export module Soup.View:GraphView;
import :GraphLayout;
import :GraphValue;

import ftxui;

namespace Soup::View
{
	enum class GraphLineTransition
	{
		None,
		Down,
		Up,
		Bidirectional,
	};

	struct GraphLineState
	{
		bool HasLeft;
		bool HasRight;
		GraphLineTransition Transition;
	};

	struct Charset
	{
		std::string TopLeft;
		std::string TopRight;
		std::string BottomLeft;
		std::string BottomRight;
		std::string Horizontal;
		std::string Vertical;
	};
	using Charsets = std::array<Charset, 6>;

	static Charsets simple_border_charset = {
		Charset{"┌", "┐", "└", "┘", "─", "│"},  // LIGHT
		Charset{"┏", "┓", "┗", "┛", "╍", "╏"},  // DASHED
		Charset{"┏", "┓", "┗", "┛", "━", "┃"},  // HEAVY
		Charset{"╔", "╗", "╚", "╝", "═", "║"},  // DOUBLE
		Charset{"╭", "╮", "╰", "╯", "─", "│"},  // ROUNDED
		Charset{" ", " ", " ", " ", " ", " "},  // EMPTY
	};

	std::string repeat(const std::string& input, int n)
	{
		if (n <= 0)
			return "";
		std::string result;
		// Pre-allocate memory for performance
		result.reserve(input.length() * n);
		for (int i = 0; i < n; ++i)
		{
			result += input;
		}

		return result;
	}

	ftxui::Element DefaultOptionTransform(const ftxui::EntryState& state, bool hasInput, bool hasOutput)
	{
		auto charsetType = state.active ? ftxui::HEAVY : ftxui::ROUNDED;
		auto& charset = simple_border_charset[charsetType];

		std::string topBorder;
		if (hasInput)
		{
			auto prefix = state.label.size() / 2;
			auto postfix = state.label.size() - 1 - prefix;
			topBorder = charset.TopLeft + repeat(charset.Horizontal, prefix) + "┴" + repeat(charset.Horizontal, postfix) + charset.TopRight;
		}
		else
		{
			topBorder = charset.TopLeft + repeat(charset.Horizontal, state.label.size()) + charset.TopRight;
		}

		std::string bottomBorder;
		if (hasOutput)
		{
			auto prefix = state.label.size() / 2;
			auto postfix = state.label.size() - 1 - prefix;
			bottomBorder = charset.BottomLeft + repeat(charset.Horizontal, prefix) + "┬" + repeat(charset.Horizontal, postfix) + charset.BottomRight;
		}
		else
		{
			bottomBorder = charset.BottomLeft + repeat(charset.Horizontal, state.label.size()) + charset.BottomRight;
		}

		auto labelElement = ftxui::text(std::move(state.label));
		if (state.focused) {
			labelElement |= ftxui::inverted;
		}
		if (state.active) {
			labelElement |= ftxui::bold;
		}

		ftxui::Element element = ftxui::vbox({
			ftxui::text(std::move(topBorder)),
			ftxui::hbox({
				ftxui::text(charset.Vertical),
				std::move(labelElement),
				ftxui::text(charset.Vertical),
			}),
			ftxui::text(std::move(bottomBorder)),
		});

		return element;
	}

	ftxui::Element DefaultOptionEdgeTransform(const GraphLineState& state)
	{
		auto charsetType = ftxui::ROUNDED;
		auto& charset = simple_border_charset[charsetType];

		auto size = 15;

		auto half = size / 2 + 1;

		std::string content;
		if (state.HasLeft)
		{
			content += repeat(charset.Horizontal, half);
		}
		else
		{
			content += std::string(half, ' ');
		}
		
		switch (state.Transition)
		{
			case GraphLineTransition::None:
				if (state.HasLeft != state.HasRight)
				{
					throw new std::runtime_error("None transition must have same on left and right");
				}

				content += state.HasLeft ? charset.Horizontal : " ";
				break;
			case GraphLineTransition::Down:
				if (state.HasLeft)
					content += state.HasRight ? "┬" : charset.TopRight;
				else if (state.HasRight)
					content += charset.TopLeft;
				else
					throw new std::runtime_error("Down transition must have at least one edge");
				break;
			case GraphLineTransition::Up:
				if (state.HasLeft)
					content += state.HasRight ? "┴" : charset.BottomRight;
				else if (state.HasRight)
					content += charset.BottomLeft;
				else
					throw new std::runtime_error("Up transition must have at least one edge");
				break;
			case GraphLineTransition::Bidirectional:
				if (state.HasLeft)
					content += state.HasRight ? "┼" : "┤";
				else
					content += state.HasRight ? "├" : charset.Vertical;
				break;
			default:
				throw new std::runtime_error("Unknown transition type");
		}

		if (state.HasRight)
		{
			content += repeat(charset.Horizontal, half);
		}
		else
		{
			content += std::string(half, ' ');
		}

		return ftxui::text(std::move(content));
	}

	// Create custom collapsible so we can style it
	// TODO: Make collapsible extensible with options
	export ftxui::Component GraphView(
		std::vector<std::vector<GraphNode>>&& layers,
		ftxui::ConstStringListRef nodeValues)
	{
		class Impl : public ftxui::ComponentBase
		{
		public:
			Impl(std::vector<std::vector<GraphNode>>&& layers, ftxui::ConstStringListRef nodeValues)
			 : _layers(layers), _nodeValues(nodeValues)
			{
			}

		private:
			static std::string TruncateCenterLabel(std::string_view label)
			{
				constexpr size_t maxSize = 15;
				if (label.size() > maxSize)
				{
					return std::string(label.substr(0, maxSize));
				}
				else
				{
					auto prefix = (maxSize - label.size()) / 2;
					auto postfix = maxSize - label.size() - prefix;
					auto content = std::string(prefix, ' ') + std::string(label) + std::string(postfix, ' ');
					return content;
				}
			}

 			ftxui::Element OnRender() override
			{
				auto content = ftxui::Elements();
				for (auto y = 0; y < _layers.size(); y++)
				{
					auto& layer = _layers[y];
					auto layerElements = ftxui::Elements();

					for (auto x = 0; x < layer.size(); x++)
					{
						auto& node = layer[x];

						auto is_selected = y == 0;
						auto is_focused = y == 1;

						auto hasInput = y > 0;
						auto hasOutput = y < _layers.size();

						const ftxui::EntryState state = {
							TruncateCenterLabel(_nodeValues[node.Index]), false, is_selected, is_focused, (int)node.Index,
						};

						auto element = DefaultOptionTransform(state, hasInput, hasOutput);
						layerElements.push_back(std::move(element));
					}

					content.push_back(ftxui::hbox(std::move(layerElements)));

					// Build the edges
					if (y + 1 != _layers.size())
					{
						auto edgeElements = ftxui::Elements();

						GraphLineState state = {
							false,
							true,
							GraphLineTransition::Bidirectional,
						};

						edgeElements.push_back(DefaultOptionEdgeTransform(state));

						content.push_back(ftxui::hbox(std::move(edgeElements)));
					}
				}

				return ftxui::vbox(std::move(content));
			}

		private:
			std::vector<std::vector<GraphNode>> _layers;
			ftxui::ConstStringListRef _nodeValues;
		};

		return ftxui::Make<Impl>(std::move(layers), std::move(nodeValues));
	}
}
