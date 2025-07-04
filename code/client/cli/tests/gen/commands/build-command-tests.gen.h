#pragma once
#include "commands/build-command-tests.h"

TestState RunBuildCommandTests() 
 {
	auto className = "BuildCommandTests";
	auto testClass = std::make_shared<Soup::Client::UnitTests::BuildCommandTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "NameIsCorrect", [&testClass]() { testClass->NameIsCorrect(); });

	return state;
}