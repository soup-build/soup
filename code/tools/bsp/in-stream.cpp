module;
#include <string>
export module BSP:IInStream;

namespace BSP {
	export class IInStream {
	public:
		virtual ~IInStream() = default;

		virtual std::string ReadLine() = 0;
		virtual std::string Read(size_t size) = 0;
	};
}
