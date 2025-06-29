// <copyright file="build-command-tests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "build-command.h"

namespace Soup::Client::UnitTests
{
	/// <summary>
	/// Build Command Tests
	/// </summary>
	class BuildCommandTests
	{
	public:
		// [[Fact]]
		void NameIsCorrect()
		{
			auto options = BuildOptions();
			auto uut = BuildCommand(options);
		}
	};
}
