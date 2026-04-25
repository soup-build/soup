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
				auto root = BuildTree();

				Add(std::move(root));
			}

		private:
			ftxui::Component BuildTree()
			{
				return ftxui::Renderer([&]
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

					constexpr size_t maxSize = 16;
					auto cell = [](std::string_view t)
					{
						if (t.size() > maxSize)
						{
							return ftxui::text(t.substr(0, maxSize)) | ftxui::border | ftxui::hcenter;
						}
						else
						{
							auto prefix = (maxSize - t.size()) / 2;
							auto postfix = maxSize - t.size() - prefix;
							auto content = std::string(prefix, ' ') + std::string(t) + std::string(postfix, ' ');
							return ftxui::text(content) | ftxui::border | ftxui::hcenter;
						}
					};
					for (auto i = 0; i < _positions.size(); i++)
					{
						auto point = _positions[i];
						content[point.Y - 1][point.X - 1] = cell(_nodeValues[i]);
					}

					auto grid = ftxui::gridbox(std::move(content));
					return grid;
				});
			}

		private:
			std::vector<GraphPoint> _positions;
			ftxui::ConstStringListRef _nodeValues;
		};

		return ftxui::Make<Impl>(std::move(positions), std::move(nodeValues));
	}
}
