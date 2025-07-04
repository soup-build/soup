// <copyright file="install-command-tests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "install-command.h"

namespace Soup::Client::UnitTests
{
	/// <summary>
	/// Install Command Tests
	/// </summary>
	class InstallCommandTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto options = InstallOptions();
			auto uut = InstallCommand(options);
		}
	};
}
