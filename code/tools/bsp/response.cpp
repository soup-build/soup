module;
#include <optional>
#include <string>
export module BSP:Response;
import json11;

namespace BSP {
	export class Response {
	public:
		Response(int id, std::optional<json11::Json> &&result, std::optional<json11::Json> &&error)
			: Id(id),
			  Result(result),
			  Error(error) {
		}

		int Id;
		std::optional<json11::Json> Result;
		std::optional<json11::Json> Error;
	};
}
