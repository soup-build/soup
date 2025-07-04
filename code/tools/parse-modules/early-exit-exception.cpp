// <copyright file="EarlyExitException.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <stdexcept>

export module parse.modules:EarlyExitException;

namespace Soup::ParseModules
{
	/// <summary>
	/// A special exception overload that indicates an early exit for the application that was handled
	/// </summary>
	export class EarlyExitException : public std::runtime_error
	{
	public:
		/// <summary>
		/// Initialize a new instance of the EarlyExitException class
		/// </summary>
		EarlyExitException(const std::string& what_arg) :
			std::runtime_error(what_arg)
		{
		}

		EarlyExitException(const EarlyExitException& other) :
			std::runtime_error(other)
		{
		}

		virtual ~EarlyExitException() noexcept
		{
		}
	};
}