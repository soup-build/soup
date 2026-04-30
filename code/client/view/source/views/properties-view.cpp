// <copyright file="properties-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <string>
#include <stdexcept>
#include <vector>

export module Soup.View:PropertiesView;

import ftxui;
import Soup.Core;
import :RecipeTreeConverter;
import :TreeValue;
import :TreeView;

namespace Soup::View
{
	TreeValueTable ToTreeValue(const Core::PackageChildrenMap& children)
	{
		auto dependencyProperties = TreeValueTable();
		for (auto& [dependencyType, dependencies] : children)
		{
			auto dependencyItems = TreeValueList();
			for (auto& dependency : dependencies)
			{
				dependencyItems.push_back(TreeValue(dependency.OriginalReference.ToString()));
			}

			dependencyProperties.Insert(dependencyType, TreeValue(std::move(dependencyItems)));
		}

		return dependencyProperties;
	}
	
	export ftxui::Component LayoutProperties(const Core::PackageInfo& packageInfo)
	{
		auto properties = TreeValueTable();

		properties.Insert("Id", TreeValue(std::to_string(packageInfo.Id)));
		properties.Insert("Name", TreeValue(packageInfo.Name.ToString()));
		properties.Insert("Root", TreeValue(packageInfo.PackageRoot.ToString()));

		auto dependencyProperties = ToTreeValue(packageInfo.Dependencies);
		properties.Insert("Dependencies", TreeValue(std::move(dependencyProperties)));

		auto recipeProperties = RecipeTreeConverter::ToTreeValue(packageInfo.Recipe->GetTable());
		properties.Insert("Recipe", TreeValue(std::move(recipeProperties)));

		auto propertiesList = ScrollFrame(TreeView(std::move(properties)));

		return propertiesList;
	}
}
