#pragma once
#include "value-table/ValueTableWriterTests.h"

TestState RunValueTableWriterTests() 
{
	auto className = "ValueTableWriterTests";
	auto testClass = std::make_shared<Soup::Core::UnitTests::ValueTableWriterTests>();
	TestState state = { 0, 0 };
	state += Soup::Test::RunTest(className, "Serialize_Empty", [&testClass]() { testClass->Serialize_Empty(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleTable", [&testClass]() { testClass->Serialize_SingleTable(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleList", [&testClass]() { testClass->Serialize_SingleList(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleString", [&testClass]() { testClass->Serialize_SingleString(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleInteger", [&testClass]() { testClass->Serialize_SingleInteger(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleFloat", [&testClass]() { testClass->Serialize_SingleFloat(); });
	state += Soup::Test::RunTest(className, "Serialize_SingleBoolean", [&testClass]() { testClass->Serialize_SingleBoolean(); });
	state += Soup::Test::RunTest(className, "Serialize_Complex", [&testClass]() { testClass->Serialize_Complex(); });

	return state;
}