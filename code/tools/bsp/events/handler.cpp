module;
#include <optional>
#include <stdexcept>
export module BSP:Handler;
import Opal;
import JsonRPC;
import BuildSystem;
import :InitializeParams;
import :InitializeResult;
import :OperationInfoParams;
import :OperationInfoResult;

using namespace Opal;
using namespace JsonRPC;

namespace BSP {
	export class Handler {
	private:
		Connection &_connection;
		BuildSystem::BuildSystemMonitor _buildSystemMonitor;

	public:
		Handler(Connection &connection)
			: _connection(connection),
			  _buildSystemMonitor() {
		}

		bool ProcessRequest(Request &request) {
			if (request.Method == "initialize") {
				if (!request.Id.has_value())
					throw std::runtime_error("Required Id");
				auto params = InitializeParams::Parse(request.Params);
				OnInitialize(request.Id.value(), params);
				return true;
			} else if (request.Method == "initialized") {
				Log::Info("Initialized");
				return true;
			} else if (request.Method == "exit") {
				OnExit();
				return false;
			} else if (request.Method == "textDocument/operation/get") {
				if (!request.Id.has_value())
					throw std::runtime_error("Required Id");
				auto params = OperationInfoParams::Parse(request.Params);
				OnGetOperationInfo(request.Id.value(), params);
				return true;
			} else {
				Log::Warning("Unknown message method: {}", request.Method);
				return true;
			}
		}

	private:
		void OnInitialize(int id, InitializeParams &params) {
			Log::Info("Initialize");
			auto result = InitializeResult();
			auto response = Response(id, result.Serialize(), std::nullopt);
			_connection.SendResponse(std::move(response));
		}

		void OnExit() {
			Log::Info("exit");
		}

		void OnGetOperationInfo(int id, OperationInfoParams &params) {
			Log::Info("GetOperationInfo");

			auto result = OperationInfoResult();

			auto operationInfo = _buildSystemMonitor.TryFindFileOperationInfo(params.File);
			if (operationInfo) {
				auto info = OperationInfo();
				info.WorkingDirectory = operationInfo->Command.WorkingDirectory;
				info.Executable = operationInfo->Command.Executable;

				for (auto &argument : operationInfo->Command.Arguments) {
					info.Arguments.push_back(argument);
				}

				// info.DeclaredInput = operationInfo->DeclaredInput;
				// info.DeclaredOutput = operationInfo->DeclaredOutput;

				result.Info = std::move(info);
			}

			auto response = Response(id, result.Serialize(), std::nullopt);
			_connection.SendResponse(std::move(response));
		}
	};
}
