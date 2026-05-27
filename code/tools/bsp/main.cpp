#include <iostream>
#include <memory>
#include <optional>

import JsonRPC;
import BSP;
import json11;
import Opal;

using namespace Opal;
using namespace JsonRPC;

int main() {
	try {
		// Setup the filter
		auto defaultTypes = static_cast<uint32_t>(TraceEventFlag::Diagnostic) |
							static_cast<uint32_t>(TraceEventFlag::Information) |
							static_cast<uint32_t>(TraceEventFlag::HighPriority) |
							static_cast<uint32_t>(TraceEventFlag::Warning) |
							static_cast<uint32_t>(TraceEventFlag::Error) |
							static_cast<uint32_t>(TraceEventFlag::Critical);
		auto filter = std::make_shared<EventTypeFilter>(static_cast<TraceEventFlag>(defaultTypes));

		// Setup the real services
		System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());

		// Setup the console listener
		auto logFile = Opal::System::IFileSystem::Current().OpenWrite(
			Path("/home/mwasplund/repos/soup/code/tools/bsp/log.txt"), true);
		Log::RegisterListener(
			std::make_shared<FileTraceListener>(logFile, "Log", filter, false, false));

		Log::Info("Setup");

		auto inStream = std::make_unique<StdInStream>();
		auto outStream = std::make_unique<StdOutStream>();
		auto connection = Connection(std::move(inStream), std::move(outStream));

		while (true) {
			auto requests = connection.ReadNextRequest();
			for (auto &request : requests) {
				BSP::ProcessRequest(connection, request);
			}
		}

	} catch (const std::exception &ex) {
		Log::Error("Unhandled error {}", ex.what());
		return -1;
	}

	return 0;
}
