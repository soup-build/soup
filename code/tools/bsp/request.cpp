module;
#include <optional>
#include <string>
export module BSP:Request;
import json11;

namespace BSP {
	export class Request {
	public:
		Request(std::optional<int> &&id, std::string &&method, json11::Json &&params)
			: Id(id),
			  Method(method),
			  Params(params) {
		}

		std::optional<int> Id;
		std::string Method;
		json11::Json Params;
	};
}
