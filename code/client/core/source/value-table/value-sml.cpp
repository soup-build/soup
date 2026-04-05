// <copyright file="recipe-sml.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

export module Soup.Core:ValueSML;

import Opal;
import :Value;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The value SML serialize manager
	/// </summary>
	export class ValueSML
	{
	public:
		/// <summary>
		/// Load from stream
		/// </summary>
		static ValueTable Deserialize(
			const Path& valueFile,
			std::istream& stream)
		{
			try
			{
				// Read the entire file for fastest read operation
				stream.seekg(0, std::ios_base::end);
				auto size = stream.tellg();
				stream.seekg(0, std::ios_base::beg);

				auto contentBuffer = std::vector<char>(size);
				stream.read(contentBuffer.data(), size);

				// Read the contents of the recipe file
				auto root = SMLDocument::Parse(contentBuffer.data(), size);

				// Load the entire root table
				auto table = ValueTable();
				Parse(table, root.GetRoot());

				return table;
			}
			catch(const std::exception& ex)
			{
				throw std::runtime_error(
					std::format("Parsing the Recipe SML failed: {} {}", ex.what(), valueFile.ToString()));
			}
		}

	private:
		static Value Parse(const SMLValue& source)
		{
			switch (source.GetType())
			{
				case SMLValueType::Boolean:
				{
					return Value(source.AsBoolean());
				}
				case SMLValueType::Integer:
				{
					return Value(source.AsInteger());
				}
				case SMLValueType::Float:
				{
					return Value(source.AsFloat());
				}
				case SMLValueType::String:
				{
					return Value(source.AsString());
				}
				case SMLValueType::Array:
				{
					auto valueList = ValueList();
					Parse(valueList, source.AsArray());
					return Value(std::move(valueList));
				}
				case SMLValueType::Table:
				{
					auto valueTable = ValueTable();
					Parse(valueTable, source.AsTable());
					return Value(std::move(valueTable));
				}
				case SMLValueType::Version:
				{
					return Value(source.AsVersion());
				}
				case SMLValueType::PackageReference:
				{
					return Value(source.AsPackageReference());
				}
				case SMLValueType::LanguageReference:
				{
					return Value(source.AsLanguageReference());
				}
				default:
				{
					throw std::runtime_error("Unknown SML type.");
				}
			}
		}

		static void Parse(ValueTable& target, const SMLTable& source)
		{
			for (const auto& [key, value] : source.GetValue())
			{
				auto recipeValue = Parse(value);
				target.emplace(key, std::move(recipeValue));
			}
		}

		static void Parse(ValueList& target, const SMLArray& source)
		{
			target.reserve(source.GetSize());
			for (size_t i = 0; i < source.GetSize(); i++)
			{
				auto value = Parse(source[i]);
				target.push_back(std::move(value));
			}
		}
	};
}
