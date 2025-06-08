#pragma once
#include "recipe/RecipeSMLTests.h"

TestState RunRecipeSMLTests()
 {
	auto className = "RecipeSMLTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::RecipeSMLTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Deserialize_GarbageThrows", [&testClass]() { testClass->Deserialize_GarbageThrows(); });
	state += Soup::Test::RunTest(className, "Deserialize_Simple", [&testClass]() { testClass->Deserialize_Simple(); });
	state += Soup::Test::RunTest(className, "Deserialize_Comments", [&testClass]() { testClass->Deserialize_Comments(); });
	state += Soup::Test::RunTest(className, "Deserialize_AllProperties", [&testClass]() { testClass->Deserialize_AllProperties(); });
	state += Soup::Test::RunTest(className, "Serialize_Simple", [&testClass]() { testClass->Serialize_Simple(); });
	state += Soup::Test::RunTest(className, "Serialize_AllProperties", [&testClass]() { testClass->Serialize_AllProperties(); });

	return state;
}