#pragma once
#include "commands/initialize-command-tests.h"

TestState RunInitializeCommandTests() 
 {
	auto className = "InitializeCommandTests";
	auto testClass = std::make_shared<Soup::Client::UnitTests::InitializeCommandTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "NameIsCorrect", [&testClass]() { testClass->NameIsCorrect(); });

	return state;
}