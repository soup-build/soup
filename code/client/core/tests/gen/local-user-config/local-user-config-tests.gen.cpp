module;

#include <memory>

export module Soup.Core.Tests:RunLocalUserConfigTests;

import :LocalUserConfigTests;

export TestState RunLocalUserConfigTests() 
{
	auto className = "LocalUserConfigTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::LocalUserConfigTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "InitializerDefault", [&testClass]() { testClass->InitializerDefault(); });
	state += Soup::Test::RunTest(className, "OperatorEqualDefault", [&testClass]() { testClass->OperatorEqualDefault(); });

	return state;
}