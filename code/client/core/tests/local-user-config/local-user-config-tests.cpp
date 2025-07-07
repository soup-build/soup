// <copyright file="local-user-config-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:LocalUserConfigTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class LocalUserConfigTests
	{
	public:
		// [[Fact]]
		void InitializerDefault()
		{
			auto uut = LocalUserConfig();

			Assert::IsFalse(uut.HasSDKs(), "Verify has no SDKs.");
		}

		// [[Fact]]
		void OperatorEqualDefault()
		{
			auto uut = LocalUserConfig();

			Assert::AreEqual(LocalUserConfig(), uut, "Verify equal.");
		}
	};
}
