module;
#include <iostream>
#include <string>
export module BSP:StdInStream;
import :IInStream;
import Opal;

using namespace Opal;

namespace BSP {
	export class StdInStream : public IInStream {
	public:
		std::string ReadLine() override final {
			Log::Diag("StdInStream::Readline");
			std::string line;
			std::getline(std::cin, line, '\n');

			Log::Diag(line);
			if (!line.empty() && line.back() == '\r') {
				line.pop_back();
			} else {
				Log::Diag("Missing \\r");
			}

			return line;
		}

		std::string Read(std::size_t size) override final {
			Log::Diag("StdInStream::Read");
			auto content = std::string();
			content.resize(size);
			std::cin.read(content.data(), size);
			return content;
		}
	};
}
