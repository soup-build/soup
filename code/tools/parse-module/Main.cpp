// <copyright file="Main.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>

import Opal;
import reflex;
import Soup.ParseModules;

using namespace Opal;

#pragma warning(disable:4996)

std::vector<std::string> Parse(const Path& file)
{
	// Use the c api file so the input auto detects the format and converts to utf8 if necessary
	auto stream = std::fopen(file.ToString().c_str(), "r");
	if (stream == nullptr)
		throw new std::runtime_error("Faild to open file");

	auto input = reflex::Input(stream);
	auto parser = Soup::ParseModules::ModuleParser(input);
	if (parser.TryParse())
	{
		return parser.GetResult();
	}
	else
	{
		auto line = parser.lineno();
		auto column = parser.columno();
		auto text = parser.text();

		std::stringstream message;
		message << "FAILED: " << line << ":" << column << " " << text;
		Log::Info(message.str());

		return std::vector<std::string>();
	}
}

int main(int argc, char** argv)
{
	try
	{
		// Setup the filter
		auto defaultTypes =
			static_cast<uint32_t>(TraceEventFlag::Diagnostic) |
			static_cast<uint32_t>(TraceEventFlag::Information) |
			static_cast<uint32_t>(TraceEventFlag::HighPriority) |
			static_cast<uint32_t>(TraceEventFlag::Warning) |
			static_cast<uint32_t>(TraceEventFlag::Error) |
			static_cast<uint32_t>(TraceEventFlag::Critical);
		auto filter = std::make_shared<EventTypeFilter>(
			static_cast<TraceEventFlag>(defaultTypes));

		// Setup the console listener
		Log::RegisterListener(
			std::make_shared<ConsoleTraceListener>(
				"Log",
				filter,
				false,
				false));

		// Setup the real services
		System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());

		if (argc < 3)
		{
			Log::Error("Invalid parameters. Expected two parameter.");
			return -1;
		}

		auto resultFilePath = Path::Parse(argv[1]);
		auto sourceFilePath = Path::Parse(argv[2]);
		auto result = Parse(sourceFilePath);

		auto resultFile = std::fstream(resultFilePath.ToString(), std::ios::out);
		bool isFirst = true;
		for (auto& line : result)
		{
			if (!isFirst)
			{
				resultFile << '\n';
			}

			resultFile << line;
			isFirst = false;
		}

		resultFile.close();
	}
	catch (const std::exception& ex)
	{
		Log::Error(ex.what());
		return -1;
	}
}