// <copyright file="package-identifier-tests.cpp" company="Soup">
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

export module Soup.Core:PackageIdentifierTests;

import :PackageIdentifier;
import :PackageReference;

import Monitor.Host;
import Opal;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class PackageIdentifierTests
	{
	public:
		// [[Fact]]
		void Initialize()
		{
			auto uut = PackageIdentifier(
				"C#",
				"user1",
				"my-package");

			Assert::AreEqual("C#", uut.GetLanguage(), "Verify language matches expected.");
			Assert::AreEqual("user1", uut.GetOwner(), "Verify owner matches expected.");
			Assert::AreEqual("my-package", uut.GetName(), "Verify name matches expected.");
		}

		// [[Fact]]
		void OperatorEqual()
		{
			auto uut = PackageIdentifier(
				"C#",
				"user1",
				"my-package");

			Assert::AreEqual(
				PackageIdentifier(
					"C#",
					"user1",
					"my-package"),
				uut,
				"Verify are equal.");
		}

		// [[Fact]]
		void OperatorNotEqualLanguage()
		{
			auto uut = PackageIdentifier(
				"C#",
				"user1",
				"my-package");

			Assert::AreNotEqual(
				PackageIdentifier(
					"C++",
					"user1",
					"my-package"),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualOwner()
		{
			auto uut = PackageIdentifier(
				"C#",
				"user1",
				"my-package");

			Assert::AreNotEqual(
				PackageIdentifier(
					"C#",
					"User2",
					"my-package"),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualName()
		{
			auto uut = PackageIdentifier(
				"C#",
				"user1",
				"my-package");

			Assert::AreNotEqual(
				PackageIdentifier(
					"C#",
					"user1",
					"my-package2"),
				uut,
				"Verify are not equal.");
		}

		// [[Theory]]
		// [[InlineData("name", std::nullopt, std::nullopt, "name")]]
		// [[InlineData("[C#]name", "C#", std::nullopt, "name")]]
		// [[InlineData("user1|name", std::nullopt, "user1", "name")]]
		// [[InlineData("[C#]user1|name", "C#", "user1", "name")]]
		void ParseValues(std::string value, std::optional<std::string> language, std::optional<std::string> owner, std::string name)
		{
			auto uut = PackageIdentifier::Parse(value);
			Assert::AreEqual(
				PackageIdentifier(language, owner, name),
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
		// [[InlineData(std::nullopt, std::nullopt, "name", "name")]]
		// [[InlineData("C#", std::nullopt, "name", "[C#]name")]]
		// [[InlineData(std::nullopt, "user1", "name", "user1|name")]]
		// [[InlineData("C#", "user1", "name", "[C#]user1|name")]]
		void ToStringValues(std::optional<std::string> language, std::optional<std::string> owner, std::string name, std::string expected)
		{
			auto uut = PackageIdentifier(language, owner, name);
			auto value = uut.ToString();
			Assert::AreEqual(
				expected,
				value,
				"Verify matches expected value.");
		}
	};
}
