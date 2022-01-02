// <copyright file="Value.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <any>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

module Soup.Core;

using namespace Opal;

namespace Soup::Core
{

Value::Value() :
	_value()
{
}

Value::Value(int64_t value) :
	_value(ValuePrimitive<int64_t>(value))
{
}

Value::Value(double value) :
	_value(ValuePrimitive<double>(value))
{
}

Value::Value(bool value) :
	_value(ValuePrimitive<bool>(value))
{
}

Value::Value(std::string value) :
	_value(ValuePrimitive<const char*>(std::move(value)))
{
}

Value::Value(ValueList list) :
	_value(std::move(list))
{
}

Value::Value(ValueTable table) :
	_value(std::move(table))
{
}

ValueType Value::GetType() const noexcept
{
	if (_value.has_value())
	{
		auto& valueType = _value.type();
		if (valueType == typeid(ValueTable))
			return ValueType::Table;
		else if (valueType == typeid(ValueList))
			return ValueType::List;
		else if (valueType == typeid(ValuePrimitive<const char*>))
			return ValueType::String;
		else if (valueType == typeid(ValuePrimitive<int64_t>))
			return ValueType::Integer;
		else if (valueType == typeid(ValuePrimitive<double>))
			return ValueType::Float;
		else if (valueType == typeid(ValuePrimitive<bool>))
			return ValueType::Boolean;
		else
			// TODO: Remove
			return ValueType::Table;
	}
	else
	{
		// TODO: Remove
		return ValueType::Table;
	}
}

void Value::SetType(ValueType type)
{
	auto currentType = GetType();

	// Ignore requests to set to same type
	if (currentType != type)
	{
		switch (type)
		{
			case ValueType::Table:
				_value = ValueTable();
				break;
			case ValueType::List:
				_value = ValueList();
				break;
			case ValueType::String:
				_value = ValuePrimitive<const char*>();
				break;
			case ValueType::Integer:
				_value = ValuePrimitive<int64_t>();
				break;
			case ValueType::Float:
				_value = ValuePrimitive<double>();
				break;
			case ValueType::Boolean:
				_value = ValuePrimitive<bool>();
				break;
			default:
				// Unknown type
				throw std::runtime_error("Unknown type");
		}
	}
}

std::string Value::ToString()
{
	switch (GetType())
	{
		case ValueType::Table:
			return std::any_cast<ValueTable&>(_value).ToString();
		case ValueType::List:
			return std::any_cast<ValueList&>(_value).ToString();
		case ValueType::String:
			return std::any_cast<ValuePrimitive<const char*>&>(_value).ToString();
		case ValueType::Integer:
			return std::any_cast<ValuePrimitive<int64_t>&>(_value).ToString();
		case ValueType::Float:
			return std::any_cast<ValuePrimitive<double>&>(_value).ToString();
		case ValueType::Boolean:
			return std::any_cast<ValuePrimitive<bool>&>(_value).ToString();
		default:
			return "UnknownType";
	}
}

ValueTable& Value::AsTable()
{
	if (_value.type() == typeid(ValueTable))
	{
		return std::any_cast<ValueTable&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as Table with incorrect type.");
	}
}

ValueList& Value::AsList()
{
	if (_value.type() == typeid(ValueList))
	{
		return std::any_cast<ValueList&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as List with incorrect type.");
	}
}

const ValueTable& Value::AsTable() const
{
	if (_value.type() == typeid(ValueTable))
	{
		return std::any_cast<const ValueTable&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as Table with incorrect type.");
	}
}

const ValueList& Value::AsList() const
{
	if (_value.type() == typeid(ValueList))
	{
		return std::any_cast<const ValueList&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as List with incorrect type.");
	}
}

const ValuePrimitive<const char*>& Value::AsString() const
{
	if (_value.type() == typeid(ValuePrimitive<const char*>))
	{
		return std::any_cast<const ValuePrimitive<const char*>&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as String with incorrect type.");
	}
}

const ValuePrimitive<int64_t>& Value::AsInteger() const
{
	if (_value.type() == typeid(ValuePrimitive<int64_t>))
	{
		return std::any_cast<const ValuePrimitive<int64_t>&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as Integer with incorrect type.");
	}
}

const ValuePrimitive<double>& Value::AsFloat() const
{
	if (_value.type() == typeid(ValuePrimitive<double>))
	{
		return std::any_cast<const ValuePrimitive<double>&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as Float with incorrect type.");
	}
}

const ValuePrimitive<bool>& Value::AsBoolean() const
{
	if (_value.type() == typeid(ValuePrimitive<bool>))
	{
		return std::any_cast<const ValuePrimitive<bool>&>(_value);
	}
	else
	{
		// Wrong type
		throw std::runtime_error("Attempt to access value as Boolean with incorrect type.");
	}
}

ValueTable& Value::EnsureTable()
{
	if (_value.type() == typeid(ValueTable))
	{
		return std::any_cast<ValueTable&>(_value);
	}
	else
	{
		_value = ValueTable();
		return std::any_cast<ValueTable&>(_value);
	}
}

bool Value::operator ==(const Value& rhs) const
{
	if (GetType() == rhs.GetType())
	{
		switch (GetType())
		{
			case ValueType::Table:
				return std::any_cast<const ValueTable&>(_value) == std::any_cast<const ValueTable&>(rhs._value);
			case ValueType::List:
				return std::any_cast<const ValueList&>(_value) == std::any_cast<const ValueList&>(rhs._value);
			case ValueType::String:
				return std::any_cast<const ValuePrimitive<const char*>&>(_value) == std::any_cast<const ValuePrimitive<const char*>&>(rhs._value);
			case ValueType::Integer:
				return std::any_cast<const ValuePrimitive<int64_t>&>(_value) == std::any_cast<const ValuePrimitive<int64_t>&>(rhs._value);
			case ValueType::Float:
				return std::any_cast<const ValuePrimitive<double>&>(_value) == std::any_cast<const ValuePrimitive<double>&>(rhs._value);
			case ValueType::Boolean:
				return std::any_cast<const ValuePrimitive<bool>&>(_value) == std::any_cast<const ValuePrimitive<bool>&>(rhs._value);
			default:
				throw std::runtime_error("Unkown ValueType for comparison.");
		}
	}
	else
	{
		return false;
	}
}

bool Value::operator !=(const Value& rhs) const
{
	return !(*this == rhs);
}

}
