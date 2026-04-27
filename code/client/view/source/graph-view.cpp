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
	// Runtime state for laying out lines so they do not overlap
	struct GraphLineSegment
	{
		int Start;
		int End;
		bool IsForward;
	};

	// Final layout for lined edges
	enum class GraphLineTransition
	{
		None,
		Down,
		Up,
		Bidirectional,
	};

	// Render state for a line section
	struct GraphLineSectionState
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

	ftxui::Element DefaultOptionEdgeTransform(const GraphLineSectionState& state)
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

	bool IsOverlapping(GraphLineSegment a, GraphLineSegment b)
	{
		return a.Start <= b.End && b.Start <= a.End;
	}

	bool TryInsertSegment(GraphLineSegment segment, std::vector<GraphLineSegment>& line)
	{
		auto source = segment.IsForward ? segment.Start : segment.End;
		auto target = segment.IsForward ? segment.End : segment.Start;
		for (auto current = line.begin(); current != line.end(); ++current)
		{
			auto currentSource = current->IsForward ? current->Start : current->End;
			auto currentTarget = current->IsForward ? current->End : current->Start;
			if (current->Start > segment.End)
			{
				// Fits entirely before this one
				line.insert(current, segment);
				return true;
			}

			// Check if they have the same source
			if (source == currentSource)
			{
				// Add if ends before the current
				// Otherwise check the next iteration
				if (target < currentTarget)
				{
					// The new overlap ends after the current
					line.insert(current, segment);
					return true;
				}
			}
			else if (IsOverlapping(*current, segment))
			{
				return false;
			}
		}

		// No conflicts, add at the end
		line.push_back(segment);
		return true;
	}

	void InsertSegment(GraphLineSegment segment, std::vector<std::vector<GraphLineSegment>>& lines)
	{
		for (auto& line : lines)
		{
			if (TryInsertSegment(segment, line))
			{
				return;
			}
		}

		// Need an entire new line
		lines.push_back(std::vector<GraphLineSegment>({ segment }));
	}

	void VerifyAddTransition(GraphLineSectionState& state, GraphLineTransition transition)
	{
		if (state.Transition != transition)
		{
			switch (state.Transition)
			{
				case GraphLineTransition::None:
					// Safe to force replace
					state.Transition = transition;
					break;
				case GraphLineTransition::Bidirectional:
					switch (transition)
					{
						case GraphLineTransition::Up:
						case GraphLineTransition::Down:
							// Leave as bidirectional
							break;
						default:
							throw std::runtime_error("Can only set Up or Down to Bidirectional");
					}
					break;
				case GraphLineTransition::Up:
					switch (transition)
					{
						case GraphLineTransition::Down:
						case GraphLineTransition::Bidirectional:
							// Combine with up
							state.Transition = GraphLineTransition::Bidirectional;
							break;
						default:
							throw std::runtime_error("Can only set Down or Bidi to Up");
					}
					break;
				case GraphLineTransition::Down:
					switch (transition)
					{
						case GraphLineTransition::Up:
						case GraphLineTransition::Bidirectional:
							// Combine with down
							state.Transition = GraphLineTransition::Bidirectional;
							break;
						default:
							throw std::runtime_error("Can only set Up or Bidi to Down");
					}
					break;
				default:
					throw std::runtime_error("Cannot change transition");
			}
		}
	}

	void SetPassThrough(
		std::vector<std::vector<GraphLineSectionState>>& layers,
		int startY,
		int endY,
		int x)
	{
		for (auto y = startY; y <= endY; y++)
		{
			VerifyAddTransition(
				layers[y][x],
				GraphLineTransition::Bidirectional);
		}
	}

	std::vector<std::vector<GraphLineSectionState>> LayoutEdges(
		int width, const std::vector<GraphNode>& layer)
	{
		// Insert the edges on as few of rows so they do not collide
		auto activeLines = std::vector<std::vector<GraphLineSegment>>();
		for (auto x = 0; x < layer.size(); x++)
		{
			auto& node = layer[x];
			for (auto& target : node.Edges)
			{
				bool isForward = target > x;
				auto start = isForward ? x : target;
				auto end = isForward ? target : x;
				GraphLineSegment segment = { start, end, isForward };
				InsertSegment(segment, activeLines);
			}
		}

		// Convert to rendering states
		auto result = std::vector<std::vector<GraphLineSectionState>>();
		for (auto y = 0; y < activeLines.size(); y++)
		{
			result.push_back(
				std::vector<GraphLineSectionState>(
					width,
					{ false, false, GraphLineTransition::None, }));
		}

		for (auto y = 0; y < activeLines.size(); y++)
		{
			auto& line = activeLines[y];
			auto& layoutSegments = result[y];
			for (auto segment : line)
			{
				for (auto x = segment.Start; x <= segment.End; x++)
				{
					layoutSegments[x].HasLeft |= x > segment.Start;
					layoutSegments[x].HasRight |= x < segment.End;

					if (segment.Start == segment.End)
					{
						VerifyAddTransition(
							layoutSegments[x],
							GraphLineTransition::Bidirectional);
						SetPassThrough(result, y + 1, activeLines.size() - 1, x);
					}
					else if (x == segment.Start)
					{
						auto transition = segment.IsForward ?
							GraphLineTransition::Up :
							GraphLineTransition::Down;
						VerifyAddTransition(
							layoutSegments[x],
							transition);
						if (segment.IsForward)
						{
							SetPassThrough(result, 0, y, x);
						}
						else
						{
							SetPassThrough(result, y + 1, activeLines.size() - 1, x);
						}
					}
					else if (x == segment.End)
					{
						auto transition = segment.IsForward ?
							GraphLineTransition::Down :
							GraphLineTransition::Up;
						VerifyAddTransition(
							layoutSegments[x],
							transition);
						if (segment.IsForward)
						{
							SetPassThrough(result, y + 1, activeLines.size() - 1, x);
						}
						else
						{
							SetPassThrough(result, 0, y, x);
						}
					}
				}
			}
		}

		return result;
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

						auto isSelected = false;
						auto isFocused = false;

						auto hasInput = y > 0;
						auto hasOutput = node.Edges.size() > 0;

						const ftxui::EntryState state = {
							TruncateCenterLabel(_nodeValues[node.Index]), false, isSelected, isFocused, (int)node.Index,
						};

						auto element = DefaultOptionTransform(state, hasInput, hasOutput);
						layerElements.push_back(std::move(element));
					}

					content.push_back(ftxui::hbox(std::move(layerElements)));

					// Build the edges if we are not on the last row
					if (y + 1 != _layers.size())
					{
						// Make sure we have enough space
						auto width = std::max(layer.size(), _layers[y+1].size());
						auto lines = LayoutEdges(width, layer);

						for (auto& line : lines)
						{
							auto lineElements = ftxui::Elements();
							for (auto& lineSection : line)
							{
								lineElements.push_back(DefaultOptionEdgeTransform(lineSection));
							}

							content.push_back(ftxui::hbox(std::move(lineElements)));
						}
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
