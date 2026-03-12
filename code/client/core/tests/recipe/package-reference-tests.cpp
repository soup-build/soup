// <copyright file="package-reference-tests.cpp" company="Soup">
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

export module Soup.Core.Tests:PackageReferenceTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class PackageReferenceTests
	{
	public:
		// [[Fact]]
		void InitializeNamed()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::IsFalse(uut.IsLocal(), "Verify is not local.");
			Assert::AreEqual("C#", uut.GetLanguage(), "Verify language matches expected.");
			Assert::AreEqual("user1", uut.GetOwner(), "Verify owner matches expected.");
			Assert::AreEqual("my-package", uut.GetName(), "Verify name matches expected.");
			Assert::AreEqual(SemanticVersion(1, 2, 3), uut.GetVersion(), "Verify version matches expected.");
		}

		// [[Fact]]
		void InitializePath()
		{
			auto uut = PackageReference(Path("../my-package"));

			Assert::IsTrue(uut.IsLocal(), "Verify is local.");
			Assert::AreEqual(Path("../my-package"), uut.GetPath(), "Verify path matches expected.");
		}

		// [[Fact]]
		void OperatorEqualNameVersion()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::AreEqual(
				PackageReference(
					"C#",
					"user1",
					"my-package",
					SemanticVersion(1, 2, 3)),
				uut,
				"Verify are equal.");
		}

		// [[Fact]]
		void OperatorEqualPath()
		{
			auto uut = PackageReference(Path("../my-package"));

			Assert::AreEqual(
				PackageReference(Path("../my-package")),
				uut,
				"Verify are equal.");
		}

		// [[Fact]]
		void OperatorNotEqualLanguage()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::AreNotEqual(
				PackageReference(
					"C++",
					"user1",
					"my-package",
					SemanticVersion(1, 2, 3)),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualOwner()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::AreNotEqual(
				PackageReference(
					"C#",
					"user2",
					"my-package",
					SemanticVersion(1, 2, 3)),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualName()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::AreNotEqual(
				PackageReference(
					"C#",
					"user1",
					"my-package2",
					SemanticVersion(1, 2, 3)),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualVersion()
		{
			auto uut = PackageReference(
				"C#",
				"user1",
				"my-package",
				SemanticVersion(1, 2, 3));

			Assert::AreNotEqual(
				PackageReference(
					"C#",
					"user1",
					"my-package",
					SemanticVersion(11, 2, 3)),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualPath()
		{
			auto uut = PackageReference(Path("../my-package"));

			Assert::AreNotEqual(
				PackageReference(Path("../my-package2")),
				uut,
				"Verify are not equal.");
		}

		// [[Theory]]
		// [[InlineData("name", std::nullopt, std::nullopt, "name", std::nullopt)]]
		// [[InlineData("name@1.2.3", std::nullopt, std::nullopt, "name", Soup::SemanticVersion(1, 2, 3))]]
		// [[InlineData("[C#]name", "C#", std::nullopt, "name", std::nullopt)]]
		// [[InlineData("[C#]name@1.2.3", "C#", std::nullopt, "name", Soup::SemanticVersion(1, 2, 3))]]
		// [[InlineData("user1|name", std::nullopt, "user1", "name", std::nullopt)]]
		// [[InlineData("user1|name@1.2.3", std::nullopt, "user1", "name", Soup::SemanticVersion(1, 2, 3))]]
		// [[InlineData("[C#]user1|name", "C#", "user1", "name", std::nullopt)]]
		// [[InlineData("[C#]user1|name@1.2.3", "C#", "user1", "name", Soup::SemanticVersion(1, 2, 3))]]
		void ParseNamedValues(std::string value, std::optional<std::string> language, std::optional<std::string> owner, std::string name, std::optional<SemanticVersion> version)
		{
			auto uut = PackageReference::Parse(value);
			Assert::AreEqual(
				PackageReference(language, owner, name, version),
				uut,
				"Verify matches expected values.");
		}

		// [[Theory]]
		// [[InlineData("../Path")]]
		void ParsePathValues(std::string value)
		{
			auto uut = PackageReference::Parse(value);
			Assert::AreEqual(
				PackageReference(Path(value)),
				uut,
				"Verify matches expected values.");
		}

		// [[Theory]]
		// [[InlineData("package@1.2.3", true)]] // Success
		// [[InlineData("package@2", true)]] // Success
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
		// [[InlineData(std::nullopt, std::nullopt, "name", std::nullopt, "name")]]
		// [[InlineData(std::nullopt, std::nullopt, "name", Soup::SemanticVersion(1, 2, 3), "name@1.2.3")]]
		// [[InlineData("C#", std::nullopt, "name", std::nullopt, "[C#]name")]]
		// [[InlineData("C#", std::nullopt, "name", Soup::SemanticVersion(1, 2, 3), "[C#]name@1.2.3")]]
		// [[InlineData(std::nullopt, "user1", "name", std::nullopt, "user1|name")]]
		// [[InlineData(std::nullopt, "user1", "name", Soup::SemanticVersion(1, 2, 3), "user1|name@1.2.3")]]
		// [[InlineData("C#", "user1", "name", std::nullopt, "[C#]user1|name")]]
		// [[InlineData("C#", "user1", "name", Soup::SemanticVersion(1, 2, 3), "[C#]user1|name@1.2.3")]]
		void ToStringNamedValues(std::optional<std::string> language, std::optional<std::string> owner, std::string name, std::optional<SemanticVersion> version, std::string expected)
		{
			auto uut = PackageReference(language, owner, name, version);
			auto value = uut.ToString();
			Assert::AreEqual(
				expected,
				value,
				"Verify matches expected value.");
		}

		// [[Theory]]
		// [[InlineData("../Path")]]
		void ToStringPathValues(std::string path)
		{
			auto uut = PackageReference(Path(path));
			auto value = uut.ToString();
			Assert::AreEqual(
				path,
				value,
				"Verify matches expected value.");
		}
	};
}
