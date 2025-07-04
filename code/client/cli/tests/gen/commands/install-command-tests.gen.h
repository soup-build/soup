#pragma once
#include "commands/install-command-tests.h"

TestState RunInstallCommandTests() 
 {
	auto className = "InstallCommandTests";
	auto testClass = std::make_shared<Soup::Client::UnitTests::InstallCommandTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Initialize", [&testClass]() { testClass->Initialize(); });

	return state;
}