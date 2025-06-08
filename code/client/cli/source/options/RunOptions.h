﻿// <copyright file="RunOptions.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "SharedOptions.h"

namespace Soup::Client
{
	/// <summary>
	/// Run Command Options
	/// </summary>
	// TODO: [Verb("run")]
	class RunOptions : public SharedOptions
	{
	public:
		/// <summary>
		/// Gets or sets the path to run
		/// </summary>
		// [[Args::Option("path", Index = 0, HelpText = "Path to the package to run.")]]
		std::string Path;

		/// <summary>
		/// Gets or sets a value indicating what flavor to use
		/// </summary>
		// [[Args::Option('f', "flavor", Default = false, HelpText = "Flavor.")]]
		std::string Flavor;

		/// <summary>
		/// Gets or sets a value indicating what target architecture
		/// </summary>
		// [[Args::Option('a', "architecture", Default = false, HelpText = "Architecture.")]]
		std::string Architecture;

		/// <summary>
		/// Pass through all of the remaining arguments
		/// </summary>
		std::vector<std::string> Arguments;
	};
}
