module;
#include <iostream>
export module BSP:StdOutStream;
import :IOutStream;

namespace BSP {
	export class StdOutStream : public IOutStream {
	public:
		void Write(std::string_view content) override final {
			std::cout << content;
		}

		void Flush() override final {
			std::cout << std::flush;
		}
	};
}
