module;
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
export module JsonRPC:Connection;
import :IInStream;
import :IOutStream;
import :Request;
import :Response;
import Opal;
import json11;

using namespace Opal;

namespace JsonRPC {
	struct HeaderPart {
		std::string Name;
		std::string Value;
	};

	export class Connection {
	private:
		std::unique_ptr<IInStream> m_inStream;
		std::unique_ptr<IOutStream> m_outStream;

	public:
		Connection(std::unique_ptr<IInStream> &&inStream, std::unique_ptr<IOutStream> &&outStream)
			: m_inStream(std::move(inStream)),
			  m_outStream(std::move(outStream)) {
		}

		std::vector<Request> ReadNextRequest() {
			Log::Info("ReadNextRequest");
			auto header = TryReadNextHeader();

			std::optional<int> contentLength;
			while (header.has_value()) {
				if (header->Name == "Content-Length") {
					Log::Info("Content length header found");
					contentLength = std::stoi(header->Value);
				} else if (header->Name == "Content-Type") {
					// TODO
					throw std::runtime_error("Content-Type not implemented");
				} else {
					throw std::runtime_error("Unknown message header: " + header->Name);
				}

				header = TryReadNextHeader();
			}

			Log::Info("End of headers");

			if (!contentLength.has_value()) {
				throw std::runtime_error("Missing required header part \"Content-Length\"");
			}

			auto content = m_inStream->Read(*contentLength);
			Log::Info(content);

			// Parse the json
			std::string errorMessage;
			auto json = json11::Json::parse(content, errorMessage);

			if (json.is_object()) {
				return {ParseRequest(json.object_items())};
			} else if (json.is_array()) {
				return ParseRequestBatch(json.array_items());
			} else {
				throw std::runtime_error("Request content must be a json object or array.");
			}
		}

		void SendResponse(Response &&response) {
			Log::Info("SendResponse");
			auto jsonRPC = json11::Json::object(
				{
					{"jsonrpc", "2.0"},
					{"id", std::move(response.Id)},
				});

			if (response.Result.has_value()) {
				jsonRPC["result"] = std::move(response.Result.value());
			} else if (response.Error.has_value()) {
				jsonRPC["error"] = std::move(response.Error.value());
			} else {
				throw std::runtime_error("Response must have a result of an error");
			}

			auto jsonMessage = json11::Json(std::move(jsonRPC));

			auto jsonContent = jsonMessage.dump();
			Log::Info(jsonContent);

			m_outStream->Write("Content-Length: ");
			m_outStream->Write(std::to_string(jsonContent.size()));
			m_outStream->Write("\r\n");

			m_outStream->Write("\r\n");

			m_outStream->Write(jsonContent);
			m_outStream->Flush();
		}
		void SendRequest(Request &&request) {
			Log::Info("SendRequest");
			auto jsonRPC = json11::Json::object(
				{
					{"jsonrpc", "2.0"},
					{"method", std::move(request.Method)},
					{"params", std::move(request.Params)},
				});

			if (request.Id.has_value()) {
				jsonRPC["id"] = std::move(request.Id.value());
			}

			auto jsonMessage = json11::Json(std::move(jsonRPC));

			auto jsonContent = jsonMessage.dump();
			Log::Info(jsonContent);

			m_outStream->Write("Content-Length: ");
			m_outStream->Write(std::to_string(jsonContent.size()));
			m_outStream->Write("\r\n");

			m_outStream->Write("\r\n");

			m_outStream->Write(jsonContent);
			m_outStream->Flush();
		}

	private:
		std::optional<HeaderPart> TryReadNextHeader() {
			Log::Info("TryReadNextHeader");
			auto headerLine = m_inStream->ReadLine();

			if (headerLine.length() == 0)
				return std::nullopt;

			auto separatorIndex = headerLine.find(": ");
			if (separatorIndex == std::string::npos)
				throw std::runtime_error("Header part missing separator");
			return std::optional<HeaderPart>({
				headerLine.substr(0, separatorIndex),
				headerLine.substr(separatorIndex + 1),
			});
		}

		Request ParseRequest(const json11::Json::object &request) {
			Log::Info("ParseRequest");
			if (request.at("jsonrpc").string_value() != "2.0") {
				throw std::runtime_error(
					"Unsupported jsonrpc version: " + request.at("jsonrpc").dump());
			}

			std::optional<int> id = std::nullopt;

			if (request.contains("id"))
				id = request.at("id").int_value();

			auto method = request.at("method").string_value();
			auto params = request.at("params");

			return Request(std::move(id), std::move(method), std::move(params));
		}

		std::vector<Request> ParseRequestBatch(const json11::Json::array &batch) {
			Log::Info("ParseRequestBatch");
			auto result = std::vector<Request>();
			for (const auto &request : batch) {
				result.push_back(ParseRequest(request.object_items()));
			}
			return result;
		}
	};
}
