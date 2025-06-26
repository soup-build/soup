#pragma once
#include "build/BuildEngineTests.h"

TestState RunBuildEngineTests() 
{
	auto className = "BuildEngineTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::BuildEngineTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Execute_NoDependencies", [&testClass]() { testClass->Execute_NoDependencies(); });
	state += Soup::Test::RunTest(className, "Execute_NoDependencies_UpToDate", [&testClass]() { testClass->Execute_NoDependencies_UpToDate(); });
	state += Soup::Test::RunTest(className, "Execute_NoDependencies_HasPreprocessors", [&testClass]() { testClass->Execute_NoDependencies_HasPreprocessors(); });
	state += Soup::Test::RunTest(className, "Execute_NoDependencies_HasPreprocessors_UpToDate", [&testClass]() { testClass->Execute_NoDependencies_HasPreprocessors_UpToDate(); });

	return state;
}