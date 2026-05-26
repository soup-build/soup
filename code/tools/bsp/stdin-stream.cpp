module;
#include <iostream>
#include <string>
export module BSP:StdInStream;
import :IInStream;

namespace BSP {
	export class StdInStream : public IInStream {
	public:
		std::string ReadLine() override final {
			std::string line;
			std::getline(std::cin, line);
			return line;
		}

		std::string Read(std::size_t size) override final {
			auto content = std::string();
			content.resize(size);
			std::cin.read(content.data(), size);
			return content;
		}
	};
}
