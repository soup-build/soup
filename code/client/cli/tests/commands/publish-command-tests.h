// <copyright file="publish-command-tests.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "publish-command.h"

namespace Soup::Client::UnitTests
{
	/// <summary>
	/// Publish Command Tests
	/// </summary>
	class PublishCommandTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto options = PublishOptions();
			auto uut = PublishCommand(options);
		}
	};
}
