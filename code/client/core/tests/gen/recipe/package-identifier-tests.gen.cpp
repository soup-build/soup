module;

#include <memory>
#include <optional>

export module Soup.Core:RunPackageIdentifierTests;

import :PackageIdentifierTests;
import Soup.Test.Assert;

export TestState RunPackageIdentifierTests() 
{
	auto className = "PackageIdentifierTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::PackageIdentifierTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });
	state += Soup::Test::RunTest(className, "OperatorEqual", [&testClass]() { testClass->OperatorEqual(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualLanguage", [&testClass]() { testClass->OperatorNotEqualLanguage(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualOwner", [&testClass]() { testClass->OperatorNotEqualOwner(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualName", [&testClass]() { testClass->OperatorNotEqualName(); });
	state += Soup::Test::RunTest(className, "ParseValues(\"name\", std::nullopt, std::nullopt, \"name\")", [&testClass]() { testClass->ParseValues("name", std::nullopt, std::nullopt, "name"); });
	state += Soup::Test::RunTest(className, "ParseValues(\"[C#]name\", \"C#\", std::nullopt, \"name\")", [&testClass]() { testClass->ParseValues("[C#]name", "C#", std::nullopt, "name"); });
	state += Soup::Test::RunTest(className, "ParseValues(\"user1|name\", std::nullopt, \"user1\", \"name\")", [&testClass]() { testClass->ParseValues("user1|name", std::nullopt, "user1", "name"); });
	state += Soup::Test::RunTest(className, "ParseValues(\"[C#]user1|name\", \"C#\", \"user1\", \"name\")", [&testClass]() { testClass->ParseValues("[C#]user1|name", "C#", "user1", "name"); });
	state += Soup::Test::RunTest(className, "TryParseValues(\"package@1.2.3\", true)", [&testClass]() { testClass->TryParseValues("package", true); });
	state += Soup::Test::RunTest(className, "ToStringValues(std::nullopt, std::nullopt, \"name\", \"name\")", [&testClass]() { testClass->ToStringValues(std::nullopt, std::nullopt, "name", "name"); });
	state += Soup::Test::RunTest(className, "ToStringValues(\"C#\", std::nullopt, \"name\", \"[C#]name\")", [&testClass]() { testClass->ToStringValues("C#", std::nullopt, "name", "[C#]name"); });
	state += Soup::Test::RunTest(className, "ToStringValues(std::nullopt, \"user1\", \"name\", \"user1|name\")", [&testClass]() { testClass->ToStringValues(std::nullopt, "user1", "name", "user1|name"); });
	state += Soup::Test::RunTest(className, "ToStringValues(\"C#\", \"user1\", \"name\", \"[C#]user1|name\")", [&testClass]() { testClass->ToStringValues("C#", "user1", "name", "[C#]user1|name"); });

	return state;
}