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
			auto prefix = state.label.size() / 2 - 1;
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
			auto prefix = state.label.size() / 2 - 1;
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

	// Create custom collapsible so we can style it
	// TODO: Make collapsible extensible with options
	export ftxui::Component GraphView(
		std::vector<GraphPoint>&& positions,
		ftxui::ConstStringListRef nodeValues)
	{
		class Impl : public ftxui::ComponentBase
		{
		public:
			Impl(std::vector<GraphPoint>&& positions, ftxui::ConstStringListRef nodeValues)
			 : _positions(positions), _nodeValues(nodeValues)
			{
			}

		private:
			static std::string TruncateCenterLabel(std::string_view label)
			{
				constexpr size_t maxSize = 16;
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
				size_t maxWidth = 0;
				size_t maxHeight = 0;
				for (auto& point : _positions)
				{
					maxWidth = std::max(point.X, maxWidth);
					maxHeight = std::max(point.Y, maxHeight);
				}

				auto content = std::vector<ftxui::Elements>(maxHeight);
				for (auto y = 0; y < maxHeight; y++)
				{
					content[y].resize(maxWidth);
					for (auto x = 0; x < maxWidth; x++)
					{
						content[y][x] = ftxui::text("");
					}
				}
				for (auto i = 0; i < _positions.size(); i++)
				{
					auto point = _positions[i];

					auto is_selected = i == 0;
					auto is_focused = i == 1;

					auto hasInput = point.Y > 1;
					auto hasOutput = point.Y < maxHeight;

					const ftxui::EntryState state = {
						TruncateCenterLabel(_nodeValues[i]), false, is_selected, is_focused, i,
					};

					auto element = DefaultOptionTransform(state, hasInput, hasOutput);
					content[point.Y - 1][point.X - 1] = element;
				}

				auto grid = ftxui::gridbox(std::move(content));
				return grid;
			}

		private:
			std::vector<GraphPoint> _positions;
			ftxui::ConstStringListRef _nodeValues;
		};

		return ftxui::Make<Impl>(std::move(positions), std::move(nodeValues));
	}
}
