// <copyright file="value-tree-converter.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <stdexcept>
#include <string>

export module Soup.View:ValueTreeConverter;

import Soup.Core;
import :TreeValue;

namespace Soup::View
{
	/// <summary>
	/// Converter from values to tree values
	/// </summary>
	export class ValueTreeConverter
	{
	public:
		static TreeValue ToTreeValue(const Core::Value& value)
		{
			switch (value.GetType())
			{
				case Core::ValueType::Table:
					return TreeValue(ToTreeValue(value.AsTable()));
				case Core::ValueType::List:
					return TreeValue(ToTreeValue(value.AsList()));
				case Core::ValueType::String:
					return TreeValue(value.AsString());
				case Core::ValueType::Integer:
					return TreeValue(std::to_string(value.AsInteger()));
				case Core::ValueType::Float:
					return TreeValue(std::to_string(value.AsFloat()));
				case Core::ValueType::Boolean:
					return TreeValue(std::to_string(value.AsBoolean()));
				case Core::ValueType::Version:
					return TreeValue(value.AsVersion().ToString());
				case Core::ValueType::PackageReference:
					return TreeValue(value.AsPackageReference().ToString());
				case Core::ValueType::LanguageReference:
					return TreeValue(value.AsLanguageReference().ToString());
				default:
					throw std::runtime_error("Unkown ValueType for comparison.");
			}
		}

		static TreeValueTable ToTreeValue(const Core::ValueTable& table)
		{
			auto valueTable = TreeValueTable();
			for (auto& [key, value] : table)
			{
				auto valueValue = ToTreeValue(value);
				valueTable.Insert(key, std::move(valueValue));
			}

			return valueTable;
		}

		static TreeValueList ToTreeValue(const Core::ValueList& list)
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
