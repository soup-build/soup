module;

#include <memory>

export module Soup.Core.Tests:RunLocalUserConfigExtensionsTests;

import :LocalUserConfigExtensionsTests;

export TestState RunLocalUserConfigExtensionsTests() 
{
	auto className = "LocalUserConfigExtensionsTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::LocalUserConfigExtensionsTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "TryLoadLocalUserConfigFromFile_MissingFile", [&testClass]() { testClass->TryLoadLocalUserConfigFromFile_MissingFile(); });
	state += Soup::Test::RunTest(className, "TryLoadLocalUserConfigFromFile_GarbageFile", [&testClass]() { testClass->TryLoadLocalUserConfigFromFile_GarbageFile(); });
	state += Soup::Test::RunTest(className, "TryLoadLocalUserConfigFromFile_SimpleFile", [&testClass]() { testClass->TryLoadLocalUserConfigFromFile_SimpleFile(); });

	return state;
}