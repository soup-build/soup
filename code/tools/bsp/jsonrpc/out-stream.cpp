module;
#include <string>
export module JsonRPC:IOutStream;

namespace JsonRPC {
	export class IOutStream {
	public:
		virtual ~IOutStream() = default;

		virtual void Write(std::string_view content) = 0;
		virtual void Flush() = 0;
	};
}
