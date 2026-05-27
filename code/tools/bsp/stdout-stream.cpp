module;
#include <iostream>
export module BSP:StdOutStream;
import :IOutStream;
import Opal;

using namespace Opal;

namespace BSP {
	export class StdOutStream : public IOutStream {
	public:
		void Write(std::string_view content) override final {
			Log::Diag(content);
			std::cout << content;
		}

		void Flush() override final {
			Log::Diag("Flush");
			std::cout << std::flush;
		}
	};
}
