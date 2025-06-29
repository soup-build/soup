#pragma once
#include "commands/version-command-tests.h"

TestState RunVersionCommandTests() 
 {
	auto className = "VersionCommandTests";
	auto testClass = std::make_shared<Soup::Client::UnitTests::VersionCommandTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });

	return state;
}