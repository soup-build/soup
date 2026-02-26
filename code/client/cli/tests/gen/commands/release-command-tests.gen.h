#pragma once
#include "commands/release-command-tests.h"

TestState RunReleaseCommandTests() 
 {
	auto className = "ReleaseCommandTests";
	auto testClass = std::make_shared<Soup::Client::UnitTests::ReleaseCommandTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });

	return state;
}