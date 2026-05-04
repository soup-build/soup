// <copyright file="recipe-tree-converter.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <stdexcept>
#include <string>

export module Soup.View:RecipeTreeConverter;

import Soup.Core;
import :TreeValue;

namespace Soup::View
{
	/// <summary>
	/// Converter from recipe values to tree values
	/// </summary>
	export class RecipeTreeConverter
	{
	public:
		static TreeValue ToTreeValue(const Core::RecipeValue& value)
		{
			switch (value.GetType())
			{
				case Core::RecipeValueType::Table:
					return TreeValue(ToTreeValue(value.AsTable()));
				case Core::RecipeValueType::List:
					return TreeValue(ToTreeValue(value.AsList()));
				case Core::RecipeValueType::String:
					return TreeValue(value.AsString());
				case Core::RecipeValueType::Integer:
					return TreeValue(std::to_string(value.AsInteger()));
				case Core::RecipeValueType::Float:
					return TreeValue(std::to_string(value.AsFloat()));
				case Core::RecipeValueType::Boolean:
					return TreeValue(std::to_string(value.AsBoolean()));
				case Core::RecipeValueType::Version:
					return TreeValue(value.AsVersion().ToString());
				case Core::RecipeValueType::PackageReference:
					return TreeValue(value.AsPackageReference().ToString());
				case Core::RecipeValueType::LanguageReference:
					return TreeValue(value.AsLanguageReference().ToString());
				default:
					throw std::runtime_error("Unkown Recipe ValueType.");
			}
		}

		static TreeValueTable ToTreeValue(const Core::RecipeTable& table)
		{
			auto valueTable = TreeValueTable();
			for (auto& [key, value] : table)
			{
				auto valueValue = ToTreeValue(value);
				valueTable.Insert(key, std::move(valueValue));
			}

			return valueTable;
		}

		static TreeValueList ToTreeValue(const Core::RecipeList& list)
		{
			auto valueList = TreeValueList();
			for (auto& value : list)
			{
				auto valueValue = ToTreeValue(value);
				valueList.push_back(std::move(valueValue));
			}

			return valueList;
		}
	};
}
