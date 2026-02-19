// <copyright file="digest.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <stdexcept>
#include <sstream>
#include <string>

export module Soup.Core:Digest;

namespace Soup::Core
{
	/// <summary>
	/// A digest object containing the function and hash value
	/// </summary>
	export class Digest
	{
	private:
		std::string _function;
		std::string _hash;

	public:
		/// <summary>
		/// Try parse a digest from the provided string
		/// </summary>
		static bool TryParse(const std::string& value, Digest& result)
		{
			// Parse the integer values
			auto firstSeparator = value.find_first_of(':');
			if (firstSeparator != std::string_view::npos)
			{
				auto hashFunction = value.substr(0, firstSeparator);
				auto hashValue = value.substr(firstSeparator + 1);
				result = Digest(std::move(hashFunction), std::move(hashValue));
				return true;
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Parse a digest from the provided string.
		/// </summary>
		static Digest Parse(const std::string& value)
		{
			Digest result;
			if (TryParse(value, result))
			{
				return result;
			}
			else
			{
				throw std::runtime_error("Invalid digest");
			}
		}

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Digest"/> class.
		/// </summary>
		Digest() :
			_function(),
			_hash()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="Digest"/> class.
		/// </summary>
		Digest(
			std::string function,
			std::string hash) :
			_function(std::move(function)),
			_hash(std::move(hash))
		{
		}

		/// <summary>
		/// Gets the function.
		/// </summary>
		const std::string& GetFunction() const
		{
			return _function;
		}

		/// <summary>
		/// Gets the hash.
		/// </summary>
		const std::string& GetHash() const
		{
			return _hash;
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const Digest& rhs) const
		{
			return _function == rhs._function &&
				_hash == rhs._hash;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const Digest& rhs) const
		{
			return _function != rhs._function ||
				_hash != rhs._hash;
		}

		/// <summary>
		/// Convert to string
		/// </summary>
		std::string ToString() const
		{
			std::stringstream stringBuilder;

			stringBuilder << _function << ':' << _hash;;

			return stringBuilder.str();
		}
	};
}
