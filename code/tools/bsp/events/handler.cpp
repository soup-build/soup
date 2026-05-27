module;
#include <optional>
#include <stdexcept>
#include <string>
export module BSP:Handler;
import Opal;
import JsonRPC;
import :InitializeParams;
import :InitializeResult;

using namespace Opal;
using namespace JsonRPC;

namespace BSP {
	void ProcessRequest(Connection &connection, int id, InitializeParams &params) {
		Log::Info("Initialize");
		auto result = InitializeResult();
		auto initializeResponse = Response(id, result.Serialize(), std::nullopt);
		connection.SendResponse(std::move(initializeResponse));
	}

	export void ProcessRequest(Connection &connection, Request &request) {
		if (request.Method == "initialize") {
			if (!request.Id.has_value())
				throw std::runtime_error("Required Id");
			auto params = InitializeParams::Parse(request.Params);
			ProcessRequest(connection, request.Id.value(), params);
		} else if (request.Method == "initialized") {
			Log::Info("Initialized");
		} else {
			Log::Warning("Unknown message method: {}", request.Method);
		}
	}
}
