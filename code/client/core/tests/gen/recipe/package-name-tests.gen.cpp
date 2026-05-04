module;

#include <memory>
#include <optional>

export module Soup.Core:RunPackageNameTests;

import :PackageNameTests;
import Soup.Test.Assert;

export TestState RunPackageNameTests() 
{
	auto className = "PackageNameTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::PackageNameTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });
	state += Soup::Test::RunTest(className, "OperatorEqual", [&testClass]() { testClass->OperatorEqual(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualOwner", [&testClass]() { testClass->OperatorNotEqualOwner(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualName", [&testClass]() { testClass->OperatorNotEqualName(); });
	state += Soup::Test::RunTest(className, "ParseValues(\"name\", std::nullopt, \"name\")", [&testClass]() { testClass->ParseValues("name", std::nullopt, "name"); });
	state += Soup::Test::RunTest(className, "ParseValues(\"user1|name\", \"user1\", \"name\")", [&testClass]() { testClass->ParseValues("user1|name", "user1", "name"); });
	state += Soup::Test::RunTest(className, "TryParseValues(\"package@1.2.3\", true)", [&testClass]() { testClass->TryParseValues("package", true); });
	state += Soup::Test::RunTest(className, "ToStringValues(std::nullopt, \"name\", \"name\")", [&testClass]() { testClass->ToStringValues(std::nullopt, "name", "name"); });
	state += Soup::Test::RunTest(className, "ToStringValues(\"user1\", \"name\", \"user1|name\")", [&testClass]() { testClass->ToStringValues("user1", "name", "user1|name"); });

	return state;
}