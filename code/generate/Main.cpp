// <copyright file="Main.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <wren/wren.hpp>

import Opal;
import Soup.Core;

using namespace Opal;

#include "generate-engine.h"

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

		if (argc != 3)
		{
			Log::Error("Invalid parameters. Expected two parameters.");
			return -1;
		}

		auto generatePhase = std::string(argv[1]);
		auto isFirstRun = generatePhase == "true";
		auto soupTargetDirectory = Path::Parse(argv[2]);

		auto generateEngine = Soup::Core::Generate::GenerateEngine();
		generateEngine.Run(isFirstRun, soupTargetDirectory);

		return 0;
	}
	catch (const std::exception& ex)
	{
		Log::Error(ex.what());
		return -1;
	}
}