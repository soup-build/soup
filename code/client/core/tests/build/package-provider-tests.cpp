// <copyright file="package-provider-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:PackageProviderTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Soup::Core::UnitTests
{
	export class PackageProviderTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto packageGraphLookup = PackageGraphLookupMap(
			{
				{ 1, PackageGraph(1, 1, ValueTable()) },
			});
			auto packageRecipe = Recipe();
			auto packageLookup = PackageLookupMap(
			{
				{
					1,
					PackageInfo(
						1,
						PackageName("User1", "Package1"),
						false,
						Path(),
						&packageRecipe,
						PackageChildrenMap())
				},
			});
			auto packageTargetDirectories = PackageTargetDirectories();
			auto uut = PackageProvider(1, packageGraphLookup, packageLookup, packageTargetDirectories);

			Assert::AreEqual(
				1,
				uut.GetRootPackageGraph().Id,
				"Verify root package graph matches expected.");
		}
	};
}