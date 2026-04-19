// <copyright file="tree-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <vector>

export module Soup.View:TreeView;
import :TreeValue;
import :CustomStyle;

import ftxui;

namespace Soup::View
{
	// Create custom collapsible so we can style it
	// TODO: Make collapsible extensible with options
	export ftxui::Component TreeView(TreeValueTable&& root)
	{
		class Impl : public ftxui::ComponentBase
		{
		public:
			Impl(TreeValueTable&& root) : root_(std::move(root))
			{
				auto components = ftxui::Components();

				BuildTree(root_, components);

				Add(ftxui::Container::Vertical(std::move(components)));
			}

		private:
			static void BuildTree(const TreeValueTable& table, ftxui::Components& components)
			{
				for (const auto& [key, value] : table)
				{
					switch (value.GetType())
					{
						case TreeValueType::Table:
							components.push_back(CreateSingleItemMenuEntry("▼ " + key));
							BuildTree(value.AsTable(), components);
							break;
						case TreeValueType::List:
							components.push_back(CreateSingleItemMenuEntry("▼ " + key));
							BuildTree(value.AsList(), components);
							break;
						case TreeValueType::String:
							components.push_back(CreateSingleItemMenuEntry(key + ": " + value.AsString()));
							break;
						default:
							throw std::runtime_error("Unkown TreeValueType for comparison.");
					}
				}
			}

			static void BuildTree(const TreeValueList& list, ftxui::Components& components)
			{
				for (const auto& value : list)
				{
					switch (value.GetType())
					{
						case TreeValueType::Table:
							components.push_back(CreateSingleItemMenuEntry("▼ "));
							BuildTree(value.AsTable(), components);
							break;
						case TreeValueType::List:
							components.push_back(CreateSingleItemMenuEntry("▼ "));
							BuildTree(value.AsList(), components);
							break;
						case TreeValueType::String:
							components.push_back(CreateSingleItemMenuEntry(value.AsString()));
							break;
						default:
							throw std::runtime_error("Unkown TreeValueType for comparison.");
					}
				}
			}

		private:
			TreeValueTable root_;
		};

		return ftxui::Make<Impl>(std::move(root));
	}
}
