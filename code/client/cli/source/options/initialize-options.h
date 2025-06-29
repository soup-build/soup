// <copyright file="initialize-options.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "shared-options.h"

namespace Soup::Client
{
	/// <summary>
	/// Initialize Command Options
	/// </summary>
	// TODO: [Verb("initialize")]
	class InitializeOptions : public SharedOptions
	{
	public:
		/// <summary>
		/// Gets or sets the path to initialize
		/// </summary>
		// [[Args::Option("path", Index = 0, HelpText = "Path to the package to initialize.")]]
		std::string Path;
	};
}
