// <copyright file="Main.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#include <iostream>
#include <memory>
#include <vector>

import Opal;
import reflex;
import Soup.ParseModules;

using namespace Opal;

void Parse(std::istream& stream)
{
	auto input = reflex::Input(stream);
	auto parser = Soup::ParseModules::ModuleParser(stream);
	if (parser.TryParse())
	{
		// return parser.GetResult();
	}
	else
	{
		auto line = parser.lineno();
		auto column = parser.columno();
		auto text = parser.text();

		std::stringstream message;
		message << "Failed to parse at " << line << ":" << column << " " << text;
		throw std::runtime_error(message.str());
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

		Log::Diag("ProgramStart");

		if (argc < 2)
		{
			Log::Error("Invalid parameters. Expected one or two parameter.");
			return -1;
		}

		auto scriptFile = Path::Parse(argv[1]);

		// Open the file to read from
		Log::Diag("Load File: {}", scriptFile.ToString());
		std::shared_ptr<System::IInputFile> file;
		if (!System::IFileSystem::Current().TryOpenRead(scriptFile, false, file))
		{
			Log::Warning("File does not exist");
			return -2;
		}

		Parse(file->GetInStream());
	}
	catch (const std::exception& ex)
	{
		Log::Error(ex.what());
		return -1;
	}
}