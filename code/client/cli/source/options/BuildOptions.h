﻿// <copyright file="BuildOptions.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "SharedOptions.h"

namespace Soup::Client
{
	/// <summary>
	/// Build Command Options
	/// </summary>
	// TODO: [[Verb("build")]]
	class BuildOptions : public SharedOptions
	{
	public:
		/// <summary>
		/// Gets or sets the path to build
		/// </summary>
		// [[Args::Option("path", Index = 0, HelpText = "Path to the package to build.")]]
		std::string Path;

		/// <summary>
		/// Gets or sets a value indicating whether to skip the build generate phase
		/// </summary>
		// [[Args::Option("skipGenerate", Default = false, HelpText = "Do not run the build generate phase.")]]
		bool SkipGenerate;

		/// <summary>
		/// Gets or sets a value indicating whether to skip the build evaluate phase
		/// </summary>
		// [[Args::Option("skipEvaluate", Default = false, HelpText = "Do not run the build evaluate phase.")]]
		bool SkipEvaluate;

		/// <summary>
		/// Gets or sets a value indicating whether to disable monitoring
		/// </summary>
		// [[Args::Option("disableMonitor", Default = false, HelpText = "Do not monitor usage for incremental builds.")]]
		bool DisableMonitor;

		/// <summary>
		/// Gets or sets a value indicating whether to partial monitoring
		/// </summary>
		// [[Args::Option("partialMonitor", Default = false, HelpText = "Do not monitor usage for incremental builds.")]]
		bool PartialMonitor;

		/// <summary>
		/// Gets or sets a value indicating whether to force a build
		/// </summary>
		// [[Args::Option("force", Default = false, HelpText = "Force a rebuild.")]]
		bool Force;

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
	};
}
