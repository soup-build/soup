// <copyright file="release-command-tests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "release-command.h"

namespace Soup::Client::UnitTests
{
	/// <summary>
	/// Release Command Tests
	/// </summary>
	class ReleaseCommandTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto options = ReleaseOptions();
			auto uut = ReleaseCommand(options);
		}
	};
}
