// <copyright file="package-name-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:PackageNameTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.SML;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::SML;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class PackageNameTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto uut = PackageName(
				"user1",
				"my-package");

			Assert::AreEqual("user1", uut.GetOwner(), "Verify owner matches expected.");
			Assert::AreEqual("my-package", uut.GetName(), "Verify name matches expected.");
		}

		// [[Fact]]
		void OperatorEqual()
		{
			auto uut = PackageName(
				"user1",
				"my-package");

			Assert::AreEqual(
				PackageName(
					"user1",
					"my-package"),
				uut,
				"Verify are equal.");
		}

		// [[Fact]]
		void OperatorNotEqualOwner()
		{
			auto uut = PackageName(
				"user1",
				"my-package");

			Assert::AreNotEqual(
				PackageName(
					"user2",
					"my-package"),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualName()
		{
			auto uut = PackageName(
				"user1",
				"my-package");

			Assert::AreNotEqual(
				PackageName(
					"user1",
					"my-package2"),
				uut,
				"Verify are not equal.");
		}

		// [[Theory]]
		// [[InlineData("name", std::nullopt, "name")]]
		// [[InlineData("user1|name", "user1", "name")]]
		void ParseValues(std::string value, std::optional<std::string> owner, std::string name)
		{
			auto uut = PackageName::Parse(value);
			Assert::AreEqual(
				PackageName(owner, name),
				uut,
				"Verify matches expected values.");
		}

		// [[Theory]]
		// [[InlineData("package", true)]] // Success
		void TryParseValues(std::string value, bool expectedResult)
		{
			PackageReference uut;
			auto result = PackageReference::TryParse(value, uut);
			Assert::AreEqual(
				expectedResult,
				result,
				"Verify matches expected result.");
		}

		// [[Theory]]
		// [[InlineData(std::nullopt, "name", "name")]]
		// [[InlineData("user1", "name", "user1|name")]]
		void ToStringValues(std::optional<std::string> owner, std::string name, std::string expected)
		{
			auto uut = PackageName(owner, name);
			auto value = uut.ToString();
			Assert::AreEqual(
				expected,
				value,
				"Verify matches expected value.");
		}
	};
}
