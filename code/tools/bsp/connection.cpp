module;
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
export module BSP:Connection;
import :IInStream;
import :IOutStream;
import :Message;

import json11;

namespace BSP {
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

		std::vector<Message> ReadNextMessage() {
			auto header = TryReadNextHeader();

			std::optional<int> contentLength;
			while (header.has_value()) {
				if (header->Name == "Content-Length") {
					contentLength = std::stoi(header->Value);
				} else if (header->Name == "Content-Type") {
					// TODO
					throw std::runtime_error("Content-Type not implemented");
				} else {
					throw std::runtime_error("Unknown message header: " + header->Name);
				}
			}

			if (contentLength.has_value()) {
				throw std::runtime_error("Missing required header part \"Content-Length\"");
			}

			auto content = m_inStream->Read(*contentLength);

			// Parse the json
			std::string errorMessage;
			auto json = json11::Json::parse(content, errorMessage);

			if (json.is_object()) {
				return {ParseMessage(json)};
			} else if (json.is_array()) {
				return ParseMessageBatch(json.array_items());
			} else {
				throw std::runtime_error("Message content must be a json object or array.");
			}
		}

		void SendMessage(Message &message) {
			m_outStream->Write("Content-Length: ");
			m_outStream->Flush();
		}

	private:
		std::optional<HeaderPart> TryReadNextHeader() {
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

		Message ParseMessage(const json11::Json &message) {
			if (message["jsonrpc"].string_value() != "2.0") {
				throw std::runtime_error(
					"Unsupported jsonrpc version: " + message["jsonrpc"].dump());
			}

			std::optional<std::string> id = std::nullopt;

			auto method = message["method"].string_value();

			return Message(std::move(id), std::move(method));
		}

		std::vector<Message> ParseMessageBatch(const json11::Json::array &batch) {
			auto result = std::vector<Message>();
			for (const auto &message : batch) {
				result.push_back(ParseMessage(message));
			}
			return result;
		}
	};
}
