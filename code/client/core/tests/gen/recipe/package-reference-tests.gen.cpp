module;

#include <memory>
#include <optional>

export module Soup.Core:RunPackageReferenceTests;

import :PackageReferenceTests;
import Opal;
import Soup.Test.Assert;

export TestState RunPackageReferenceTests() 
{
	auto className = "PackageReferenceTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::PackageReferenceTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "InitializeNamed", [&testClass]() { testClass->InitializeNamed(); });
	state += Soup::Test::RunTest(className, "InitializePath", [&testClass]() { testClass->InitializePath(); });
	state += Soup::Test::RunTest(className, "OperatorEqualNameVersion", [&testClass]() { testClass->OperatorEqualNameVersion(); });
	state += Soup::Test::RunTest(className, "OperatorEqualPath", [&testClass]() { testClass->OperatorEqualPath(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualLanguage", [&testClass]() { testClass->OperatorNotEqualLanguage(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualOwner", [&testClass]() { testClass->OperatorNotEqualOwner(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualName", [&testClass]() { testClass->OperatorNotEqualName(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualVersion", [&testClass]() { testClass->OperatorNotEqualVersion(); });
	state += Soup::Test::RunTest(className, "OperatorNotEqualPath", [&testClass]() { testClass->OperatorNotEqualPath(); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"name\", std::nullopt, std::nullopt, \"name\", std::nullopt)", [&testClass]() { testClass->ParseNamedValues("name", std::nullopt, std::nullopt, "name", std::nullopt); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"name@1.2.3\", std::nullopt, std::nullopt, \"name\", SemanticVersion(1, 2, 3))", [&testClass]() { testClass->ParseNamedValues("name@1.2.3", std::nullopt, std::nullopt, "name", SemanticVersion(1, 2, 3)); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"[C#]name\", \"C#\", std::nullopt, \"name\", std::nullopt)", [&testClass]() { testClass->ParseNamedValues("[C#]name", "C#", std::nullopt, "name", std::nullopt); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"[C#]name@1.2.3\", \"C#\", std::nullopt, \"name\", SemanticVersion(1, 2, 3))", [&testClass]() { testClass->ParseNamedValues("[C#]name@1.2.3", "C#", std::nullopt, "name", SemanticVersion(1, 2, 3)); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"user1|name\", std::nullopt, \"user1\", \"name\", std::nullopt)", [&testClass]() { testClass->ParseNamedValues("user1|name", std::nullopt, "user1", "name", std::nullopt); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"user1|name@1.2.3\", std::nullopt, \"user1\", \"name\", SemanticVersion(1, 2, 3))", [&testClass]() { testClass->ParseNamedValues("user1|name@1.2.3", std::nullopt, "user1", "name", SemanticVersion(1, 2, 3)); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"[C#]user1|name\", \"C#\", \"user1\", \"name\", std::nullopt)", [&testClass]() { testClass->ParseNamedValues("[C#]user1|name", "C#", "user1", "name", std::nullopt); });
	state += Soup::Test::RunTest(className, "ParseNamedValues(\"[C#]user1|name@1.2.3\", \"C#\", \"user1\", \"name\", SemanticVersion(1, 2, 3))", [&testClass]() { testClass->ParseNamedValues("[C#]user1|name@1.2.3", "C#", "user1", "name", SemanticVersion(1, 2, 3)); });
	state += Soup::Test::RunTest(className, "ParsePathValues(\"../Path\")", [&testClass]() { testClass->ParsePathValues("../Path"); });
	state += Soup::Test::RunTest(className, "TryParseValues(\"package@1.2.3\", true)", [&testClass]() { testClass->TryParseValues("package@1.2.3", true); });
	state += Soup::Test::RunTest(className, "TryParseValues(\"package@2\", true)", [&testClass]() { testClass->TryParseValues("package@2", true); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(std::nullopt, std::nullopt, \"name\", std::nullopt, \"name\")", [&testClass]() { testClass->ToStringNamedValues(std::nullopt, std::nullopt, "name", std::nullopt, "name"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(std::nullopt, std::nullopt, \"name\", SemanticVersion(1, 2, 3), \"name@1.2.3\")", [&testClass]() { testClass->ToStringNamedValues(std::nullopt, std::nullopt, "name", SemanticVersion(1, 2, 3), "name@1.2.3"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(\"C#\", std::nullopt, \"name\", std::nullopt, \"[C#]name\")", [&testClass]() { testClass->ToStringNamedValues("C#", std::nullopt, "name", std::nullopt, "[C#]name"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(\"C#\", std::nullopt, \"name\", SemanticVersion(1, 2, 3), \"[C#]name@1.2.3\")", [&testClass]() { testClass->ToStringNamedValues("C#", std::nullopt, "name", SemanticVersion(1, 2, 3), "[C#]name@1.2.3"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(std::nullopt, \"user1\", \"name\", std::nullopt, \"user1|name\")", [&testClass]() { testClass->ToStringNamedValues(std::nullopt, "user1", "name", std::nullopt, "user1|name"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(std::nullopt, \"user1\", \"name\", SemanticVersion(1, 2, 3), \"user1|name@1.2.3\")", [&testClass]() { testClass->ToStringNamedValues(std::nullopt, "user1", "name", SemanticVersion(1, 2, 3), "user1|name@1.2.3"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(\"C#\", \"user1\", \"name\", std::nullopt, \"[C#]user1|name\")", [&testClass]() { testClass->ToStringNamedValues("C#", "user1", "name", std::nullopt, "[C#]user1|name"); });
	state += Soup::Test::RunTest(className, "ToStringNamedValues(\"C#\", \"user1\", \"name\", SemanticVersion(1, 2, 3), \"[C#]user1|name@1.2.3\")", [&testClass]() { testClass->ToStringNamedValues("C#", "user1", "name", SemanticVersion(1, 2, 3), "[C#]user1|name@1.2.3"); });
	state += Soup::Test::RunTest(className, "ToStringPathValues(\"../Path\")", [&testClass]() { testClass->ToStringPathValues("../Path"); });

	return state;
}