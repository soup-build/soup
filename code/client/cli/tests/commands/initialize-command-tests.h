// <copyright file="initialize-command-tests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "initialize-command.h"

namespace Soup::Client::UnitTests
{
	/// <summary>
	/// Initialize Command Tests
	/// </summary>
	class InitializeCommandTests
	{
	public:
		// [[Fact]]
		void NameIsCorrect()
		{
			auto options = InitializeOptions();
			auto uut = InitializeCommand(options);
		}
	};
}
