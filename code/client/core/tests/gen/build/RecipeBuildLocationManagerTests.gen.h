#pragma once
#include "build/RecipeBuildLocationManagerTests.h"

TestState RunRecipeBuildLocationManagerTests() 
 {
	auto className = "RecipeBuildLocationManagerTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::RecipeBuildLocationManagerTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "GetOutputDirectory_Simple", [&testClass]() { testClass->GetOutputDirectory_Simple(); });
	state += Soup::Test::RunTest(className, "GetOutputDirectory_RootRecipe", [&testClass]() { testClass->GetOutputDirectory_RootRecipe(); });

	return state;
}