// <copyright file="version-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "version-options.h"

namespace Soup::Client
{
	/// <summary>
	/// Version Command
	/// </summary>
	class VersionCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="VersionCommand"/> class.
		/// </summary>
		VersionCommand(VersionOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("VersionsCommand::Run");

			// TODO var version = Assembly.GetExecutingAssembly().GetName().Version;
			// Log::Message($"{version.Major}.{version.Minor}.{version.Build}");
			Log::HighPriority("0.43.1");
		}

	private:
		VersionOptions _options;
	};
}
