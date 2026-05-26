module;
#include <optional>
#include <string>
export module BSP:Message;

namespace BSP {
	export class Message {
	public:
		Message(std::optional<std::string> &&id, std::string &&method)
			: Id(id),
			  Method(method) {
		}

		std::optional<std::string> Id;
		std::string Method;
	};
}
