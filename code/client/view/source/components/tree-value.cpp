// <copyright file="tree-value.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

export module Soup.View:TreeValue;

import Opal;

export namespace Soup::View
{
	class TreeValue;
	using TreeValueList = std::vector<TreeValue>;
	using TreeValueTable = Opal::SequenceMap<std::string, TreeValue>;

	enum class TreeValueType : uint64_t
	{
		Table = 1,
		List = 2,
		String = 3,
	};

	/// <summary>
	/// Tree Value
	/// </summary>
	class TreeValue
	{
	private:
		std::variant<
			TreeValueTable,
			TreeValueList,
			std::string> _value;

	public:
		/// <summary>
		/// Initializes a new instance of the tree value class
		/// </summary>
		TreeValue(const TreeValueTable& table) :
			_value(table)
		{
		}
		TreeValue(TreeValueTable&& table) :
			_value(std::move(table))
		{
		}

		TreeValue(const TreeValueList& list) :
			_value(list)
		{
		}
		TreeValue(TreeValueList&& list) :
			_value(std::move(list))
		{
		}

		TreeValue(const std::string& value) :
			_value(value)
		{
		}
		TreeValue(std::string&& value) :
			_value(std::move(value))
		{
		}

		/// <summary>
		/// Type checker methods
		/// </summary>
		TreeValueType GetType() const
		{
			switch (_value.index())
			{
				case 0:
					return TreeValueType::Table;
				case 1:
					return TreeValueType::List;
				case 2:
					return TreeValueType::String;
				default:
					throw std::runtime_error("Unknown value type.");
			}
		}

		bool IsTable() const
		{
			return GetType() == TreeValueType::Table;
		}

		bool IsList() const
		{
			return GetType() == TreeValueType::List;
		}

		bool IsString() const
		{
			return GetType() == TreeValueType::String;
		}

		/// <summary>
		/// Internal accessors
		/// </summary>
		std::string ToString();

		TreeValueTable& AsTable()
		{
			if (GetType() == TreeValueType::Table)
			{
				return std::get<TreeValueTable>(_value);
			}
			else
			{
				// Wrong type
				throw std::runtime_error("Attempt to access value as Table with incorrect type.");
			}
		}

		TreeValueList& AsList()
		{
			if (GetType() == TreeValueType::List)
			{
				return std::get<TreeValueList>(_value);
			}
			else
			{
				// Wrong type
				throw std::runtime_error("Attempt to access value as List with incorrect type.");
			}
		}

		const TreeValueTable& AsTable() const
		{
			if (GetType() == TreeValueType::Table)
			{
				return std::get<TreeValueTable>(_value);
			}
			else
			{
				// Wrong type
				throw std::runtime_error("Attempt to access value as Table with incorrect type.");
			}
		}

		const TreeValueList& AsList() const
		{
			if (GetType() == TreeValueType::List)
			{
				return std::get<TreeValueList>(_value);
			}
			else
			{
				// Wrong type
				throw std::runtime_error("Attempt to access value as List with incorrect type.");
			}
		}

		const std::string& AsString() const
		{
			if (GetType() == TreeValueType::String)
			{
				return std::get<std::string>(_value);
			}
			else
			{
				// Wrong type
				throw std::runtime_error("Attempt to access value as String with incorrect type.");
			}
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const TreeValue& rhs) const
		{
			if (GetType() == rhs.GetType())
			{
				switch (GetType())
				{
					case TreeValueType::Table:
						return std::get<TreeValueTable>(_value) == std::get<TreeValueTable>(rhs._value);
					case TreeValueType::List:
						return std::get<TreeValueList>(_value) == std::get<TreeValueList>(rhs._value);
					case TreeValueType::String:
						return std::get<std::string>(_value) == std::get<std::string>(rhs._value);
					default:
						throw std::runtime_error("Unkown TreeValueType for comparison.");
				}
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const TreeValue& rhs) const
		{
			return !(*this == rhs);
		}
	};
}