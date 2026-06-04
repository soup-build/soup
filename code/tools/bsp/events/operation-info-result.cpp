module;
#include <optional>
#include <string>
#include <vector>
export module BSP:OperationInfoResult;
import json11;
import Opal;

namespace BSP {
	export class OperationInfo {
	public:
		Opal::Path WorkingDirectory;
		Opal::Path Executable;
		std::vector<std::string> Arguments;

	public:
		json11::Json Serialize() {
			auto result = json11::Json::object();

			result.emplace("workingDirectory", WorkingDirectory.ToString());
			result.emplace("executable", Executable.ToString());

			auto arguments = json11::Json::array();
			for (auto &argument : Arguments) {
				arguments.push_back(argument);
			}
			result.emplace("arguments", std::move(arguments));

			return result;
		}
	};

	export class OperationInfoResult {
	public:
		/**
		 * The capabilities the language server provides.
		 */
		std::optional<OperationInfo> Info;

	public:
		json11::Json Serialize() {
			auto result = json11::Json::object();

			if (Info) {
				auto capabilities = Info->Serialize();
				result.emplace("info", std::move(capabilities));
			}

			return result;
		}
	};
}
